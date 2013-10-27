
#include <SoftwareSerial.h>

//----------------------------------------------------
//
//    o------/\/\/\-----o-----[photocell]------o
//   gnd      10k       A0                     5v
//
//----------------------------------------------------
//
//    o-----[led]----/\/\/\------o
//   gnd              10k       A1  
//
//----------------------------------------------------

/************************************************************************************************************
 * http://www.instructables.com/id/Arduino-Timer-Interrupts/
 * timer speed (Hz) = (Arduino clock speed (16MHz)) / prescaler
 * prescaler can be either 1, 8, 64, 256 or 1024
 * prescaler can be either 2^0, 2^3, 2^6, 2^7 or 2^8
 * interrupt frequency (Hz) = (Arduino clock speed 16,000,000Hz) / (prescaler * (compare match register + 1))
 * compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
 * 
 * CS12    CS11    CS10      Prescaler
 * ------------------------------------------------------------------------------
 * 0       0       0       Timer off/no clock source
 * 0       0       1       1:1 prescaler/this is the same as not having prescaler
 * 0       1       0       1:8 prescaler
 * 0       1       1       1:64 prescaler
 * 1       0       0       1:256 prescaler
 * 1       0       1       1:1024 prescaler
 ************************************************************************************************************/

#define LEDlaser 13
#define LEDtx 12
#define LEDrx 11
#define LEDr 4
#define LEDw 5
#define rx 3
#define tx 2
#define no_such_pin 255
#define LEDSENSOR A1
#define SAMPLESIZE 10
//-------------------------
volatile int EIGHT_HZ = 0;
enum timer_action {TRANSMIT, RECIEVE};
char* timer_actions[] = {"TX", "RX"};
volatile timer_action timerAction = TRANSMIT;
volatile timer_action tempAction;
//-------------------------
int bitDelay = 5;
int thisByte = '\~'; 
long t;
byte low = 0;
int threshold = 200;

SoftwareSerial mySerial(rx, tx); 

void setup(){  
  setupTimer();
  for (int i =3; i<=13;i++){ 
    pinMode(i,OUTPUT); 
    digitalWrite(i,LOW);
  }
  pinMode(LEDSENSOR, INPUT);
  digitalWrite(LEDr, HIGH);
  digitalWrite(LEDw, LOW);
  Serial.begin(9600);
  mySerial.begin(9600);
  t = millis();
  tune();
}


void loop(){ 
  //if (tempAction != timerAction) {tempAction = timerAction; Serial.println(timer_actions[timerAction]);}
  //----------------------------------------------------------------------
  if (timerAction == TRANSMIT) { 
    if (Serial.available() == 0) {
      mySerial.write(thisByte);   
      Serial.write(thisByte);   
      if(thisByte == 126) { thisByte = 32; } 
      thisByte++;
      Serial.print("-->");  
    }
  }
  //----------------------------------------------------------------------
  if (timerAction == RECIEVE) {
    if (Serial.available()){
      byte data = Serial.read();
      low = 0;
      for (byte mask = 00000001; mask>0; mask <<= 1) {
        digitalWrite(LEDlaser,(data & mask) > 0);
        digitalWrite(LEDtx,(data & mask) > 0);
        delay(bitDelay);
        
        // receive a single bit of transmitted data
        int val = getSensorReading(LEDSENSOR);
        low += (val * mask);
        digitalWrite(LEDrx, val);
        if(digitalRead(LEDrx) != digitalRead(LEDtx)) { Serial.print("x");}
      }
        Serial.print((char)low);
      if (Serial.available()==0){Serial.println();}
    }
  }
}


int getSensorReading(int sensorPin){
  long val=0;
  for (int i=0; i<SAMPLESIZE; i++){
    val += (analogRead(sensorPin));
    delay(1);
  }
  val = (val/SAMPLESIZE);
  //Serial.println(val);
  if (val < 420) {
    val = 0;
  } else {
    val = 1;
  }
  return val;
}


void printBits(byte data){
  for (byte mask = 00000001; mask>0; mask <<= 1) { 
    if (data & mask){
      digitalWrite(LEDlaser,HIGH); 
      //Serial.print("1");
    } else {
      digitalWrite(LEDlaser,LOW); 
      //Serial.print("0");
    }
    delayMicroseconds(bitDelay);
//    delay(bitDelay);
  }
}




void tune() {
  Serial.print("tuning...");
  for (byte data = '\!'; data <= '\~'; data++){
    for (byte mask = 00000001; mask>0; mask <<= 1){
      digitalWrite(13, (data & mask) > 0);
      digitalWrite(12, (data & mask) > 0);
      delayMicroseconds(80);
      int bitValue = getSensorReading(LEDSENSOR);
      if (((data & mask) > 0) > bitValue){ (--threshold); }
      if (((data & mask) > 0) < bitValue){ (++threshold); }
    }
  }
  Serial.print("new threshold is ");
  Serial.println(threshold);
}




//--------------------------------------------------------------------------

void setupTimer() {
  noInterrupts();
  // 8 bit timer
  TCCR2A = 0;              // set entire TCCR2A register to 0
  TCCR2B = 0;              // same for TCCR2B
  TCNT2  = 0;              //initialize counter value to 0
                           // set compare match register for 8khz increments
  OCR2A = 249;             // = (16*10^6) / (1000 * 64) - 1 (must be <256)
  TCCR2A |= (1 << WGM21);  // turn on CTC mode
  TCCR2B |= (1 << CS12);   // | (1<<CS10);    // Set CS10 and CS12 bit for 64 prescaler
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
  interrupts();
}  


ISR(TIMER2_COMPA_vect){
  if (EIGHT_HZ++ == (100)) { 
    EIGHT_HZ = 0;
    digitalWrite(LEDr, digitalRead(LEDr) ^ 1);
    digitalWrite(LEDw, digitalRead(LEDw) ^ 1);
    if (timerAction == RECIEVE){
      timerAction = TRANSMIT;
    } 
    else {
      timerAction = RECIEVE;
    } 
  }
}

//--------------------------------------------------------------------------

