#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

// #define DEBUG

#define led 12
#define laser 13

volatile boolean tx = false;
Timer t = Timer(1, clock, 1);
byte frameByte = B01010101;
int skip = 0;


void setup() {
  #ifdef DEBUG
  for (int i = 3; i<14; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i,LOW);
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);
  Serial.println("=========== Transmitter ===========");
  #endif

  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  delay(5000);
  t.startClock();
  sync();
}


void loop() {
  // for(char c=33; c<127; c++) {
  for(char c=65; c<91; c++) {
    sendChar(c);
  }
  sync();
}


void sendChar(char c){
  int bitcount = 0;
  while (bitcount < 8){
    if (tx) {
      tx=false;
      digitalWrite(laser, bitRead(c,bitcount));

      #ifdef DEBUG
      Serial.print(bitRead(c,bitcount));
      digitalWrite(led, bitRead(c,bitcount));
      digitalWrite(3, digitalRead(3)^1);
      if (bitcount == 8) {
        Serial.print("-->[");
        Serial.write(c);
        Serial.print("]");
        Serial.print(" : ");
        Serial.print(skip);
        Serial.println();
      }
      skip = 0;
      #endif

      bitcount++;
    }
    else
      skip++;
  }
}


void sync(){
  #ifdef DEBUG
  Serial.println("----- syncing ------");
  #endif

  sendChar(B00000000);
  sendChar(B10000000);
  sendChar(frameByte);
}

void clock(int x){
  tx = true;
}
