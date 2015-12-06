#include <Timer.h>

//----------------------------------------------------
// A0  o-----------|
// gnd o----/\/\/\-|-------e[phototrans]c-------o 5v
//            10k   
//----------------------------------------------------

#define DEBUG

#define led 13
#define sensor A0

volatile boolean rx = false;
Timer t = Timer(1, clock, 1);
long i = 0;

int previous = 0;
int current = 0;

void setup() {
  Serial.begin(9600); 
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  t.startClock();
  Serial.println("============= Receiver ============"); 
}


void loop() { 
  int bitcount = 0;
  char c;
  while (bitcount < 8) {
    if (rx){
      rx = false;
      current = digitalRead(sensor);
      bitWrite(c, bitcount, current);
      bitcount++;
    }
  }
  for (int i=0; i<8; i++){
    Serial.print(bitRead(c,i));
  }
  Serial.println();
}


void clock(int x) {
  rx = true;
}