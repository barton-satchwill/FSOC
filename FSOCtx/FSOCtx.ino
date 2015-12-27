#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

#define DEBUG

#define led 12
#define laser 13
#define lenPayload 5
const char frameStart = B00111100;
const char frameEnd = B00111110;
// const char frameStart = B10101010;
// const char frameEnd = B10000000;
volatile boolean tx = false;
Timer t = Timer(1, clock);


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
  char msg[] = "What hath God wrought?";
  int len = strlen(msg);
  enframe(msg);
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
      if (bitcount == 7) {
        char str[20];
        sprintf(str, "-->[%c]\n", c);
        Serial.print(str);
      }
      #endif
      bitcount++;
    }
  }
}


void enframe(char *input){
  int offset=0;
  int lenInput = strlen(input);
  // 3 extra characters for framing and end of string delimiter
  char txbuffer[lenPayload+3];

  while (offset < lenInput){
    txbuffer[0] = frameStart;
    strncpy(txbuffer+1, input+offset, lenPayload);
    int end = min(lenPayload+1, strlen(txbuffer));
    txbuffer[end] = frameEnd;
    txbuffer[end + 1] = '\0';
    offset = offset + lenPayload;
    for (int j = 0 ; j<lenPayload+2; j++){
      sendChar(txbuffer[j]);
    }
    sync();
  }
}


void sync(){
  #ifdef DEBUG
  Serial.println("----- syncing ------");
  #endif

  sendChar(B00000000);
  sendChar(B10000000);
}

void clock(){
  tx = true;
}
