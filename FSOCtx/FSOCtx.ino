#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

//#define DEBUG

#define led 12
#define laser 13

volatile boolean tx = false;
Timer t = Timer(1, clock);
byte frameByte = B01010101;

//speed tests
long time = 0;
long count = 1000000;
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
  }
}


void sendChar(char c) {
  while (true) {
    if (tx) {
      tx=false;
      digitalWrite(laser, digitalRead(laser)^1);
    }
  }
}


void sync(){
  sendChar(B00000000);
  sendChar(B10000000);
  sendChar(frameByte);
}

void clock(){
  tx = true;
}


void interval(long & t, long & counter, long limit, char * msg){
  if (counter++ > limit){
    long interval = micros() - t;
    t = micros();
    counter = 0;

    Serial.print(msg);
    Serial.print("\t");
    Serial.print(interval/limit);
    Serial.println(" microseconds ");
  }
}
