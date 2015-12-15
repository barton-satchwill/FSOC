/*
  Timer.cpp 
*/

#include "Timer.h"

static void (*isrCallback)(int);


int Timer::pin;

Timer::Timer(double f, void (*interruptHandler)(int), int handlerData ) {
  frequency = f;
  prescaler = 1024;
  compareMatchRegister = (clockSpeed / (prescaler * frequency)) - 1;
  isrCallback = interruptHandler;
  pin = handlerData;
}

void Timer::startClock() {
  TCCR1B = B00001100;
  TCCR1A = B00000000;
  TIMSK1 = B00000010;
  TCNT1 = 0;
  OCR1A = 6249; // 10 Hz
  // OCR1A = 2499; // 25 Hz
  // OCR1A = 1240; // 50 Hz
  // OCR1A = 624; // 100 Hz
  OCR1A = 124; // 500 Hz
  // OCR1A = 77; // 800 Hz
  // OCR1A = 62; // 1 kHz
}

void Timer::stopClock(){
  TCCR1B = 0;
  TCCR1A = 0;
  TIMSK1 = 0;
  OCR1A = 0;
  TCNT1 = 0;
}


void Timer::resetClock() {
  TCNT1 = 0;  
}


void Timer::toSerial(){
  Serial.print("frequency: ");              Serial.println(frequency);
  Serial.print("prescaler: ");              Serial.println(prescaler);
  Serial.print("compareMatchRegister: ");   Serial.println(compareMatchRegister);
  Serial.print("pin: ");                    Serial.println(pin);
}

void Timer::doSomethingSecret(void) {
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(800);
}

ISR(TIMER1_COMPA_vect){
  isrCallback(Timer::pin);
}

