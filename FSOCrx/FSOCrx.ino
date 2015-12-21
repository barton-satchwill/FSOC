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
long count = 100;
long counter = 0;
int expected = 0;
long success = 0;
long failure = 0;
long total = 0;

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
}


char receiveChar() {
  int actual = 0;
  char c;
  while (true) {
    if (rx) {
      rx = false;
      actual = getSensor(); //digitalRead(sensor);
      // Serial.print("expected "); Serial.print(expected); Serial.print(", saw "); Serial.print(actual);
      // Serial.println();
      if (actual == expected) {success++; }

      expected = expected ^ 1; 
      total++;
      counter++;

      if (counter == count ) {
        counter = 0;
        Serial.print(((float)success / (float)total) * 100); Serial.println("%");
        success = 0;
        failure = 0;
        total = 0;
      }
    }
  }
  return c;
}

int getSensor() {
  int samples = 2;
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
  Serial.println();
  while (!digitalRead(sensor)) {
    ; // do nothing
  }
  t.resetClock();
  rx = false;
  expected = 0;

  Serial.println("clock started");
}


void clock() {
  rx = true;
}



void interval(long & time, long i) {
  long t = micros() - time;
  time = micros();
  Serial.print("\t");  Serial.print(t / i);  Serial.println(" microseconds ");
}
