#include <Timer.h>

//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|-------e[phototrans]c-------o 5v
//            10k
//----------------------------------------------------

// #define DEBUG

#define led 13
#define sensor A0
const char frameStart = B00111100;
const char frameEnd   = B00111110;
// const char frameStart = B10101010;
// const char frameEnd = B10000000;
volatile boolean rx = false;
Timer t = Timer(15000, clock);
int current = 0;


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

  if (c != frameStart && c != frameEnd && c != B00000000) {
    #ifdef DEBUG
    for (int i=7; i>=0; i--){
      Serial.print(bitRead(c,i));
    }
    Serial.print("-->[");
    #endif

    Serial.write(c);

    #ifdef DEBUG
    Serial.print("]");
    Serial.println();
    #endif
  }

  if (c == B00000000) {
    sync();
  }
}


char receiveChar() {
  int bitcount = 0;
  char c;
  while (bitcount < 8) {
    if (rx){
      rx = false;
      current = getSensor();
      bitWrite(c, bitcount, current);
      digitalWrite(led, current);
      bitcount++;
    }
  }
  return c;
}


int getSensor() {
  int samples = 4;
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
  #ifdef DEBUG
  Serial.println("----- syncing ------");
  #endif

  while (!digitalRead(sensor)) {
    ; // do nothing
  }
  t.resetClock();
  rx=false;

  #ifdef DEBUG
  Serial.println("clock started");
  #endif
}


void clock() {
  rx = true;
}
