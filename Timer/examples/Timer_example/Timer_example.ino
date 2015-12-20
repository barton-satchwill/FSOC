#include <Timer.h>

//------------------------------------------------------------------
// Demostrates how to do something with the Timer library
//------------------------------------------------------------------

int pin = 13;
volatile boolean tic = false;
Timer myTimer = Timer(0.25, clock);

void setup() { 
  Serial.begin(9600); 
  pinMode(pin, OUTPUT);
}

void loop() {
  myTimer.toSerial();
  myTimer.startClock();
  delay(10000);
  myTimer.stopClock();
  delay(2000);
}



void do_something(int data){
    digitalWrite(data, digitalRead(data) ^ 1);
}



void clock() {
  tic = true;
  do_something(pin);
}

