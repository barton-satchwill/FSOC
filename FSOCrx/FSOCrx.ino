
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
int baudrate = 300; 
//-------------------------
int samplesize = 10;
int threshold = 450;
volatile int samplecount = 0;
volatile int bitcount = 0;
volatile int bitvalue=0;
volatile  byte aByte;
//-------------------------
long tclock = 0;
long writeclock = 0;
long bitclock = 0;
long byteclock = 0;

void setup() {
  setupTimer();
  Serial.begin(115200);
  pinMode(LEDSENSOR, INPUT);
  pinMode(LEDrx, OUTPUT);
  Serial.println("=========== Reciever ===========");
  tclock = micros();
  writeclock = micros();
  bitclock = micros();
  byteclock = micros();
}
  
  
void loop() {
  test();
  if (Serial.available()){
    char c = Serial.read();
    if (c == '!') {
      configure();
    }
  }
}

void receive(){
  if (syncd && write_bit){
    write_a_bit();
    bitcount++;
    write_bit = false;
    if (bitcount == 8){
      bitcount = 0;
    }      
  }

  if (syncd && sample){
    sample_a_bit();
    sample = false;
  }
}

void test(){
  syncd = true;
  if (syncd && write_bit){
    Serial.print("write");
    interval(writeclock);
    delayMicroseconds(100);
    bitcount++;
    write_bit = false;
    if (bitcount == 8){
      bitcount = 0;
      Serial.print("byte");
      interval(byteclock);
      Serial.println();
    }      
  }

  if (syncd && sample){
    Serial.print("\tsample");
    interval(bitclock);
    delayMicroseconds(100);
    sample = false;
  }
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
}


int getSensorReading(int sensorPin){
  long bitValue=0;
  for (int i=0; i<samplesize; i++){
    bitValue += (analogRead(sensorPin));
    delayMicroseconds(10);
  }
  bitValue = (bitValue/samplesize);
  return (bitValue > threshold);
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
//      sample = true;
//      write_bit = false;
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

  if (CLOCK_COUNTER == baudrate) { 
    write_bit = true;
    CLOCK_COUNTER = 0;
  }
  else
  if (CLOCK_COUNTER%(baudrate/5) == 0) { 
    sample = true;
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
      digitalWrite(13, (data & mask) > 0);
      digitalWrite(12, (data & mask) > 0);
      delayMicroseconds(80);
      int val = getSensorReading(LEDSENSOR);
      if (((data & mask) > 0) > val){ 
        --threshold; 
      }
      if (((data & mask) > 0) < val){ 
        ++threshold; 
      }
    }
  }
  Serial.print("new threshold is ");
  Serial.println(threshold);
}

void interval(long & time){
    long t = micros() - time;
    time = micros();
    Serial.print("\t");  Serial.print(t);  Serial.println(" microseconds "); 
}


