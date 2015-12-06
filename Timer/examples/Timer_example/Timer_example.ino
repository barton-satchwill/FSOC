#include <Timer.h>

//------------------------------------------------------------------
// Demostrates how to do something with the Timer library
//------------------------------------------------------------------

int pin = 13;
volatile int counter = 0;
Timer myTimer = Timer(0.25, do_something, pin);

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
