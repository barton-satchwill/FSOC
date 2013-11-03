
//----------------------------------------------------
//
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//
//----------------------------------------------------

#define LEDlaser 12
volatile int CLOCK_COUNTER = 0;
volatile boolean send_bit = false;
volatile int baudrate = 15;
int bitcount = 0;
long bitclock = 0;

void setup() { 
  Serial.begin(115200); 
  setupTimer();
  pinMode(LEDlaser, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(LEDlaser, LOW);
  Serial.println("=========== Transmitter ===========");
} 

void loop() { 
  // send the message header
  transmit_byte(B01010101);
//----------- debugging -----------
  for(char c=33; c<127; c++){
    transmit_byte(c);
  }
//---------------------------------
  while (Serial.available()){
    char c = Serial.read();
    if (c == '!') {
      configure();
    } else {
      transmit_byte(c);
//      test();
    }
  }
  //send the trailer
  transmit_byte(B00000000);
}



void test(){
  // once you start to send a bit, 
  bitcount = 0;
  while (bitcount < 7){
    if(send_bit){
      digitalWrite(13, digitalRead(13)^1);
      bitcount++;
      Serial.print("send bit");
      interval(bitclock);
      send_bit = false;
    }
  }
  if (bitcount ==7) {
    Serial.println();
  }
}

void transmit_byte(byte data) {
  bitcount = 0;
  while (bitcount < 7){
    if(send_bit){
//        interval(bitclock);

      digitalWrite(13, bitRead(data, bitcount));
      digitalWrite(LEDlaser, bitRead(data, bitcount++));
      Serial.println(digitalRead(LEDlaser));
      send_bit = false;
      //----------- debugging -----------
      if (bitcount == 7) { 
        Serial.print("-->[");
        Serial.write(data);
        Serial.print("]");
//        Serial.print(data, BIN);
        Serial.println(); 
        interval(bitclock);
      }
      //---------------------------------
    }
  }
}

void preamble(){
  Serial.println("sending preamble");
  byte preambleByte = B01010101;
  transmit_byte(preambleByte);
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
  CLOCK_COUNTER++;

  if (CLOCK_COUNTER == baudrate) { 
    CLOCK_COUNTER = 0;
    send_bit = true;
  }
  // close the window
  if (CLOCK_COUNTER == 50) { 
    send_bit = false;
  }
}
//--------------------------------------------------------------------------

void configure(){
  status();
  while(!Serial.available()){
    ; // do nothing
  }
  while(Serial.available()){
    byte s = Serial.read();
    switch(s){
    case 'b':
      baudrate = Serial.parseInt();
      CLOCK_COUNTER = 0;
      Serial.print("baud rate = "); 
      Serial.println(baudrate);   
      break;
    case 't':
      Serial.println("transmit");
//      for(int i=0; i<15; i++){
//        transmit_byte(buffer[i]);
//      }
      Serial.println();
      break;
    default:
      Serial.println("default");
      break;
    }
  }
}

void status(){
  Serial.println("------------------------------------------");
  Serial.print("send_bit = ");  
  Serial.println(send_bit);   
  Serial.print("baud rate = "); 
  Serial.println(baudrate);   
  Serial.println("------------------------------------------");
}


void interval(long & time){
    long t = micros() - time;
    time = micros();
    Serial.print("\t");  Serial.print(t);  Serial.println(" microseconds "); 
}



