//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|---e[phototrans]c-----o 5v
//            10k   
//----------------------------------------------------

#define LEDSENSOR A0
#define LEDrx 11
//-------------------------
volatile int CLOCK_COUNTER = 0;
volatile boolean moresample = false;
volatile boolean sample = false;
volatile boolean write_bit = false;
volatile boolean syncd = false;
volatile int g_previousstate = 0;
int g_baudrate = 150; 
//-------------------------
//int samplesize = 10;
//int threshold = 450;
volatile int g_samplecount = 0;
volatile int g_samplevalue = 0;
volatile int g_bitcount = 0;
volatile int g_bitvalue=0;
volatile int g_oldbit=0;
volatile byte g_aByte;
volatile long phantom = 0;
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
  pinMode(8, INPUT);
//  syncd = true;
}


void loop() {
  //  test();
  receive();
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
    write_bit = false;
    moresample = true;
    if (g_bitcount == 7){
      g_bitcount = 0;
    }      
  }
  if (syncd && sample){
    sample_a_bit();
    sample = false;
    moresample = false;
  }
}

void test(){
  syncd = true;
  if (syncd && write_bit){
    Serial.print("write");
    interval(writeclock);
    delayMicroseconds(100);
    g_bitcount++;
    write_bit = false;
    if (g_bitcount == 8){
      g_bitcount = 0;
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
  g_samplecount++;  // very sloppy using this global
  int sensorReading = getSensorReading(LEDSENSOR);
//  if(g_oldbit != sensorReading) {
//    CLOCK_COUNTER=0;
//  }
  g_oldbit = sensorReading;
  int p = digitalRead(8);
  g_samplevalue += sensorReading;
  phantom += p;
}


void write_a_bit(){
  debugBitSample();
  if ( (g_samplevalue/(double)g_samplecount) > 0.5){
    g_bitvalue = 1;
  } else{
    g_bitvalue = 0;
  }
  bitWrite(g_aByte, g_bitcount, g_bitvalue);
  moresample = true;
  Serial.print(g_bitvalue);
  g_samplecount = 0;
  g_bitvalue = 0;
  g_samplevalue = 0;
  g_bitcount++;
  if (g_bitcount == 7){
    //------ debugging ------   
    Serial.print("--->");
    Serial.write(g_aByte);
    Serial.println();
    //interval(bitclock);
    //----------------------
    g_bitcount = 0; 
    g_aByte = 0;
  }
}


int getSensorReading(int sensorPin){
  return digitalRead(sensorPin);
}



void synchronise(){
  int fliplimit = 9;
  int flipcount = 0;

  while (!syncd){
    int state = getSensorReading(LEDSENSOR);
    if (state != g_previousstate ) {
      bitWrite(g_aByte, flipcount, state);
//      Serial.print("setting bit "); 
//      Serial.print(flipcount);
//      Serial.print("-->"); 
//      for(int i=7; i>=0; i--){
//        Serial.print(bitRead(g_aByte, i));
//      }
//      Serial.println();
      g_previousstate = state; 
      flipcount++; 
    }
    if (g_aByte == 85){
      syncd = true;
      g_bitcount = 5;
      CLOCK_COUNTER = 0;
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

  if (CLOCK_COUNTER == g_baudrate) { 
    write_bit = true;
    CLOCK_COUNTER = 0;
  }

  if (CLOCK_COUNTER > (110) || CLOCK_COUNTER < (g_baudrate-110)) { 
//    if(moresample)
    sample = true;
  }
}


