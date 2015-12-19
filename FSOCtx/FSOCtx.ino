#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

//#define DEBUG

#define led 12
#define laser 13

volatile boolean tx = false;
Timer t = Timer(1, clock, 1);
byte frameByte = B01010101;

//speed tests
long time = 0;
long count = 100000;
long counter = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("=========== Transmitter ===========");

  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  delay(5000);
  t.startClock();
  sync();
  time = 0;
}


void loop() {
  for(char c=65; c<115; c++) {
    sendChar(c);

    if (counter++ > count){
      counter = 0;
      interval(time, count);
    }
  }
  sync();
}


void sendChar(char c){
  int bitcount = 0;
  while (bitcount < 8){
    if (tx) {
      //tx=false;
      digitalWrite(laser, bitRead(c,bitcount));
      bitcount++;
    }
  }
}


void sync(){
  sendChar(B00000000);
  sendChar(B10000000);
  sendChar(frameByte);
}

void clock(int x){
  tx = true;
}


void interval(long & time, long i){
  long t = micros() - time;
  time = micros();
  Serial.print("\t");  Serial.print(t/i);  Serial.println(" microseconds ");
}
