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
byte frameByte = B01010101;
  int wordcounter = 0;
int skip = 0;


void setup() { 
  Serial.begin(9600); 
  pinMode(laser, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(laser, LOW);
  digitalWrite(led, LOW);
  t.startClock();
  Serial.println("=========== Transmitter ===========");
  sync();
} 


void loop() {
  for(char c=33; c<127; c++) {
      if(wordcounter++ == 4) { wordcounter = 0; sync(); }
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
        Serial.print(" : ");
        Serial.print(skip);
        Serial.println(); 
      }
      skip = 0;
      #endif
    }
    else
      skip++;
  }
}


void sync(){
  sendChar(B00000000);
  sendChar(B00000000);
  sendChar(frameByte);
}

void clock(int x){
  tx = true;
}