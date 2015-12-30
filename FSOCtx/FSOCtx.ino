#include <Timer.h>

//----------------------------------------------------
//   gnd o\---------[laser]-----o 13
//         \---------[led]------o 12
//----------------------------------------------------

// #define DEBUG

#define led 12
#define laser 13
#define lenPayload 30
const char frameStart = B00111100;
const char frameEnd = B00111110;
// const char frameStart = B10101010;
// const char frameEnd = B10000000;
volatile boolean tx = false;
Timer t = Timer(15000, clock);

char testMsg[] =
"Ham: I am myself indifferent honest, but yet I could accuse\n"
"  me of such things that it were better my mother had not borne me.\n"
"  I am very proud, revengeful, ambitious; with more offences at my\n"
"  beck than I have thoughts to put them in, imagination to give\n"
"  them shape, or time to act them in. What should such fellows as I\n"
"  do, crawling between earth and heaven? We are arrant knaves all;\n"
"  believe none of us. Go thy ways to a nunnery. Where's your\n"
"  father?\n"
"Oph: At home, my lord.\n"
"Ham: Let the doors be shut upon him, that he may play the fool\n"
"  nowhere but in's own house. Farewell.\n"
"Oph: O, help him, you sweet heavens!\n"
"Ham: If thou dost marry, I'll give thee this plague for thy dowry:\n"
"  be thou as chaste as ice, as pure as snow, thou shalt not escape\n"
"  calumny. Get thee to a nunnery. Go, farewell. Or if thou wilt\n"
"  needs marry, marry a fool; for wise men know well enough what\n"
"  monsters you make of them. To a nunnery, go; and quickly too.\n"
"  Farewell.\n"
"Oph: O heavenly powers, restore him!\n"
"-----------------------\n\n"; 


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

  Serial.begin(115200);
  pinMode(laser, OUTPUT);
  digitalWrite(laser, LOW);
  delay(5000);
  t.startClock();
  sync();
}


void loop() {
  char * message = getInput();
  send(message);
}

char * getInput() {
  char buffer[100]="";
  int len = Serial.available();
  if (len > 0){
    Serial.readBytes(buffer, len);
    buffer[len] = '\0';
    return buffer;
  }
  return testMsg;
}

void send(char * message) {
  enframe(message);
}


void enframe(char *input){
  size_t offset=0;
  size_t lenInput = strlen(input);
  // 3 extra characters for framing and end of string delimiter
  char txbuffer[lenPayload+3];

  while (offset < lenInput) {
    txbuffer[0] = frameStart;
    strncpy(txbuffer+1, input+offset, lenPayload);
    int end = min(lenPayload+1, strlen(txbuffer));
    txbuffer[end] = frameEnd;
    txbuffer[end + 1] = '\0';
    offset = offset + lenPayload;

    sync();
    for (int j = 0 ; j<lenPayload+2; j++){
      sendChar(txbuffer[j]);
    }

  }
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
