#include <Timer.h>

//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|-------e[phototrans]c-------o 5v
//            10k
//----------------------------------------------------

// #define DEBUG

#define led 13
#define sensor A0

volatile boolean rx = false;
Timer t = Timer(1, clock);
byte frameByte = B01010101;


// speed test
long time = 0;
long count = 100000;
long counter = 0;

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
  char c = receiveChar();
  // Serial.write(c);
  if (c == B00000000) {
    sync();
  }
}


char receiveChar() {
  int bitcount = 0;
  int value = 0;
  char c;
  while (bitcount < 8) {
    if (rx){
      //rx = false;
      if (counter++ > count ){
        counter = 0;
        interval(time, count);
      }
      value = getSensor();
      bitWrite(c, bitcount, value);
      digitalWrite(led, value);
      bitcount++;
    }
  }
  return c;
}

int getSensor() {
  int samples = 8;
  int reading = 0;
  for (int i = 0; i < samples; i++) {
    reading = reading + digitalRead(sensor);
  }
  if (reading >= samples/2)
    return 1;
  else
    return 0;
}


void sync() {
  // do nothing
}


void clock(int x) {
  rx = true;
}



void interval(long & time, long i){
  long t = micros() - time;
  time = micros();
  Serial.print("\t");  Serial.print(t/i);  Serial.println(" microseconds ");
}
