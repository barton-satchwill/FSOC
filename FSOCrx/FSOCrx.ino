
//----------------------------------------------------
// gnd o\---------[sensor]-----o A0
//       \----------[led]------o 13 tell-tale
//        \---------[led]------o 12 for tuning
//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|---e[phototrans]c-----o 5v
//            10k   
//----------------------------------------------------


#define LEDSENSOR A0
#define LEDrx 11
//-------------------------
volatile int CLOCK_COUNTER = 0;
volatile boolean sample = false;
volatile boolean write_bit = false;
volatile boolean syncd = false;
volatile int previousstate = 0;
int baudrate = 10; 
//-------------------------
int samplesize = 1;
int threshold = 450;
volatile int samplecount = 0;
volatile int bitcount = 0;
volatile int bitvalue=0;
volatile  byte aByte;


void setup() {
  setupTimer();
  Serial.begin(9600);
  pinMode(LEDSENSOR, INPUT);
  pinMode(LEDrx, OUTPUT);
  Serial.println("=========== Reciever ===========");
}

void loop() {
  if (Serial.available()){
    char c = Serial.read();
    if (c == '!') {
      configure();
    }
  }
  if (syncd && write_bit){
    write_a_bit();
  }

    sample_a_bit();
}


void sample_a_bit(){
  samplecount++;
  int sensorReading = getSensorReading(LEDSENSOR);
  bitvalue += sensorReading;
}


void write_a_bit(){
  if ( (bitvalue / (double)samplecount) > 0.5){
    bitvalue = 1;
  }
  else{
    bitvalue = 0;
  }
  bitWrite(aByte, bitcount, bitvalue);
  samplecount = 0;
  bitcount++;
  if (bitcount == 7){
    bitcount = 0; 
    Serial.write(aByte);
  aByte = 0;
  }
  bitvalue=0;
  write_bit = false;
}


int getSensorReading(int sensorPin){
  return (analogRead(sensorPin) > threshold);
}



void synchronise(){
  int fliplimit = 8;
  int flipcount = 0;
  
  while (!syncd){
    int state = getSensorReading(LEDSENSOR);
    if (state != previousstate ) {
      previousstate = state; 
      flipcount++; 
    }
    if (flipcount == fliplimit){
      syncd = true;
      CLOCK_COUNTER = (baudrate/10)-1;
    }    
  }
  Serial.println("synchronised!");
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

  if (syncd && CLOCK_COUNTER == baudrate) { 
    write_bit = true;
    CLOCK_COUNTER = 0;
    digitalWrite(13, digitalRead(13)^1);
  }
}

//--------------------------------------------------------------------------
void configure(){
  status();
  while(!Serial.available()) {
    ;
  }
  while(Serial.available())
  {
    byte s = Serial.read();
    switch(s){
    case 'b':
      baudrate = Serial.parseInt();
      Serial.print("baud rate = ");  Serial.println(baudrate);
      break;
    case 's':
      Serial.print("synchronise...");
      syncd = false;
      synchronise();
      break;
    case 't':
      tune();
      break;
    case 'z':
      samplesize = Serial.parseInt();
      Serial.print("sample size = "); Serial.println(samplesize);
      break;
    case '\\':
      Serial.println();
      Serial.println("--------------------------------------------------------");
      break;
    default:
      Serial.println("default");
      break;
    }
  }
}


void status(){
  Serial.println("---------------------------------------------");
  Serial.print("baud rate = ");    Serial.println(baudrate);
  Serial.print("sample size = ");  Serial.println(samplesize);
  Serial.print("threshold = ");    Serial.println(threshold);
  Serial.println("---------------------------------------------");
}


void tune() {
  Serial.print("tuning...");
  for (byte data = '\!'; data <= '\~'; data++){
    for (byte mask = 00000001; mask>0; mask <<= 1){
      int sentValue = ((data & mask) > 0);
      digitalWrite(13, sentValue);
      digitalWrite(12, sentValue);
      delayMicroseconds(80);
      int recievedValue = getSensorReading(LEDSENSOR);
      if (sentValue > recievedValue){ 
        --threshold; 
      }
      if (sentValue < recievedValue){ 
        ++threshold; 
      }
    }
  }
  Serial.print("new threshold is ");
  Serial.println(threshold);
}

