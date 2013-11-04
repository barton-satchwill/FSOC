//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|---e[phototrans]c-----o 5v
//            10k   
//----------------------------------------------------

#define LEDSENSOR A0
//-------------------------
volatile int CLOCK_COUNTER = 0;
volatile boolean g_sample = false;
volatile boolean g_write_bit = false;
volatile boolean g_syncd = false;
int g_baudrate = 15; 
int g_sampleWindowEdge = 4;
int g_sampleWindowOpen = g_sampleWindowEdge;
int g_sampleWindowClose = g_baudrate - g_sampleWindowEdge;
//-------------------------
volatile int g_samplecount = 0;
volatile int g_samplevalue = 0;
volatile int g_bitcount = 0;
volatile int g_bitvalue=0;
volatile byte g_aByte;
//-------------------------

void setup() {
  setupTimer();
  Serial.begin(115200);
  pinMode(LEDSENSOR, INPUT);
  Serial.println("=========== Reciever ===========");
}


void loop() {
  receive();
  if (Serial.available()){
    char c = Serial.read();
    if (c == '!') {
      configure();
    }
  }
}

void receive(){
  if (g_syncd && g_write_bit){
    write_a_bit();
    g_write_bit = false;
  }
  if (g_syncd && g_sample){
    g_sample_a_bit();
    g_sample = false;
  }
}




void g_sample_a_bit(){
  g_samplecount++;  // very sloppy using this global
  int sensorReading = getSensorReading(LEDSENSOR);
  g_samplevalue += sensorReading;
}


void write_a_bit(){
  resynchronise();
  bitWrite(g_aByte, g_bitcount, g_bitvalue);
  Serial.print(g_bitvalue);
  g_samplecount = 0;
  g_samplevalue = 0;
  g_bitcount++;
  if (g_bitcount == 7){
    //------ debugging ------   
    Serial.print("--->");
    Serial.write(g_aByte);
    Serial.println();
    //----------------------
    g_bitcount = 0; 
    g_aByte = 0;
  }
}


int getSensorReading(int sensorPin){
  return digitalRead(sensorPin);
}


void synchronise(){
  while (!g_syncd){
    int state = getSensorReading(LEDSENSOR);
    if (state) {
      g_syncd = true;
      g_bitcount = 6;
      CLOCK_COUNTER = 0;
    }    
  }
  Serial.println("synchronised!");
}


void resynchronise(){
  volatile double sampleAverage = (g_samplevalue/(double)g_samplecount);
  volatile double confidence;
  if(sampleAverage > 0.5){
    confidence = sampleAverage;
    g_bitvalue = 1;
  } else{
    confidence = 1-sampleAverage;
    g_bitvalue = 0;
  }
    CLOCK_COUNTER-=(1-confidence)*g_baudrate;
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
    g_write_bit = true;
    CLOCK_COUNTER = 0;
  }

  if (CLOCK_COUNTER > g_sampleWindowOpen && CLOCK_COUNTER < g_sampleWindowClose) { 
    g_sample = true;
  }
}


