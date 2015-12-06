#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

#define DEBUG

#define led 12
#define laser 13

volatile boolean tx = false;
Timer t = Timer(1, clock, 1);


void setup() { 
  Serial.begin(9600); 
  pinMode(laser, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(laser, LOW);
  digitalWrite(led, LOW);
  t.startClock();
  Serial.println("=========== Transmitter ===========");
} 


void loop() {
  for(char c=33; c<127; c++) {
    sendChar(c);
  }
}


void sendChar(char c){
  int bitcount = 0;
  while (bitcount < 8){
    if (tx) {
      tx=false;
      Serial.print(bitRead(c,bitcount));
      digitalWrite(led, bitRead(c,bitcount));
      digitalWrite(laser, bitRead(c,bitcount));
      bitcount++;
      #ifdef DEBUG
      if (bitcount == 8) { 
        Serial.print("-->[");
        Serial.write(c);
        Serial.print("]");
        Serial.println(); 
      }
      #endif
    }
  }
}


void clock(int x){
  tx = true;
}