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
byte frameByte = B01010101;

long i = 0;

int previous = 0;
int current = 0;
  int wordcounter = 0;

void setup() {
  Serial.begin(9600); 
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  t.startClock();
  Serial.println("============= Receiver ============"); 
  sync();
}


void loop() { 
  char c = receiveChar();

  if (wordcounter++ == 4) { wordcounter = 0; sync(); }

  #ifdef DEBUG
  for (int i=0; i<8; i++){
    Serial.print(bitRead(c,i));
  }
  Serial.print("-->[");
  Serial.write(c);
  Serial.print("]");

  if (c == frameByte) {Serial.print(" ----> Frame Byte"); }// else { sync(); }
  Serial.println();
  #endif  
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
  Serial.println("----------------");
  int bitcount = 0;
  char c = B00000000;
  while (c != frameByte) {
    if (rx){
      rx = false;
      current = digitalRead(sensor);
      bitWrite(c, bitcount, current);
      digitalWrite(led, current);

      #ifdef DEBUG
      for (int i=7; i>=0; i--){ Serial.print(bitRead(c,i)); }
      Serial.print(" --- ");
      Serial.println(bitcount);
      #endif  

      if (c != frameByte) {
        Serial.print("*");
        c = c << 1;
      }
      else {
        Serial.println("sync'd!");
      }
    }
  }
}


void clock(int x) {
  rx = true;
}