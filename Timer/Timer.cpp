/*
  Timer.cpp 
*/

#include "Timer.h"

static void (*isrCallback)();


Timer::Timer(double f, void (*interruptHandler)() ) {
  frequency = f;
  prescaler = 1024;
  compareMatchRegister = (clockSpeed / (prescaler * frequency)) - 1;
  isrCallback = interruptHandler;
}

void Timer::startClock() {

  // Timer1 in 'Clear Timer on Compare' mode
  TCCR1A = B00000000;  // 
  TIMSK1 = B00000010;  // enable timer compare interrupt
  TCNT1 = 0;           // initialise counter

  prescaler = 256;
  OCR1A = 62499; // 1 Hz

  // prescaler = 64;
  // OCR1A = 24999; // 10 Hz
  // OCR1A = 2499; // 100 Hz
  // OCR1A = 249; // 1,000 Hz
  // OCR1A = 49; // 5,000 Hz
  // OCR1A = 24; // 10,000 Hz
  

  // prescaler = 1;
  // OCR1A = 1066; // 15,000 Hz
  // OCR1A = 799; // 20,000 Hz
  // OCR1A = 639; // 25,000 Hz
  // OCR1A = 532; // 30,000 Hz
  // OCR1A = 456; // 35,000 Hz
  // OCR1A = 399; // 40,000 Hz

  switch (prescaler) {
    case 0:
      TCCR1B = B00001000; // stopped
      break;
    case 1:
      TCCR1B = B00001001;
      break;
    case 8:
      TCCR1B = B00001010;
      break;
    case 64:
      TCCR1B = B00001011;
      break;
    case 256:
      TCCR1B = B00001100;
      break;
    case 1024:
      TCCR1B = B00001101;
      break;
    default:
      TCCR1B = B00001001; // prescaler = 1
      break;    
  }
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
}


ISR(TIMER1_COMPA_vect){
  isrCallback();
}

