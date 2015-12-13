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
Timer t = Timer(1, clock, 1);
byte frameByte = B01010101;

long i = 0;

int previous = 0;
int current = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("============= Receiver ============");
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  sync();
}


void loop() {
  char c = receiveChar();

  #ifdef DEBUG
  for (int i=7; i>=0; i--){
    Serial.print(bitRead(c,i));
  }
  Serial.print("-->[");
  #endif

  Serial.write(c);

  #ifdef DEBUG
  Serial.print("]");
  if (c == frameByte) {Serial.print(" ----> Frame Byte"); }
  Serial.println();
  #endif

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
      current = digitalRead(sensor);
      bitWrite(c, bitcount, current);
      digitalWrite(led, current);
      bitcount++;
    }
  }
  return c;
}


void sync() {
  #ifdef DEBUG
  Serial.println("----- syncing ------");
  #endif

  t.stopClock();
  Serial.println();
  while (!digitalRead(sensor)) {
    ; // do nothing
  }
  t.startClock();

  #ifdef DEBUG
  Serial.println("clock started");
  #endif
}


void clock(int x) {
  rx = true;
}
