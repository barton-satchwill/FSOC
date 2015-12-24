#include <Timer.h>

//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|-------e[phototrans]c-------o 5v
//            10k
//----------------------------------------------------

// #define DEBUG

#define led 13
#define sensor A0
#define samples 4

volatile boolean rx = false;
Timer t = Timer(1, clock);
byte frameByte = B01010101;


// speed test
long counter = 0;
int expected = 0;
long success = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("============= Receiver ============");
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  t.startClock();
  sync();
}


void loop() {
  receiveChar();
}


char receiveChar() {
  int actual = 0;
  char c;
  while (true) {
    if (rx) {
      rx = false;
      actual = getSensor(); //digitalRead(sensor);
      counter++;

      if (actual == expected) { 
        success++;    
        expected = expected ^ 1;
      } else {
        Serial.println(counter);
        counter = 0;        
        sync();
      }
    }
  }
  return c;
}

int getSensor() {
  int reading = 0;
  for (int i = 0; i < samples; i++) {
    reading = reading + digitalRead(sensor);
  }
  if (reading >= samples / 2)
    return 1;
  else
    return 0;
}


void sync() {
  // Serial.println();
  while (!digitalRead(sensor)) {
    ; // do nothing, as quickly as possible
  }
  t.resetClock();
  rx = false;
  // we just received a '1', 
  // so we expect the next cycle to transmit a '0'
  expected = 0;
}


void clock() {
  rx = true;
}



void interval(long & time, long i) {
  long t = micros() - time;
  time = micros();
  Serial.print("\t");  Serial.print(t / i);  Serial.println(" microseconds ");
}
