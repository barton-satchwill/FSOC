/*
  Timer.cpp 
*/


#include "Timer.h"

static void (*isrCallback)();
long prescalerArray[] = {1, 8, 64, 256, 1024};


Timer::Timer(long hertz, void (*interruptHandler)() ) {
  isrCallback = interruptHandler;
  frequency = hertz;
  prescaler = 1;
  int len = sizeof(prescalerArray) / sizeof(prescalerArray[0]);

  for (int i = 0; i<len; i++){
    prescaler = prescalerArray[i];
    compareMatchRegister = (clockSpeed / (prescaler * frequency)) - 1;
    if (1 <= compareMatchRegister && compareMatchRegister < 65536) {
      break;
    }
  }

}


void Timer::startClock() {

  // Timer1 in 'Clear Timer on Compare' mode
  TCCR1A = B00000000;  // 
  TIMSK1 = B00000010;  // enable timer compare interrupt
  TCNT1 = 0;           // initialise counter
  OCR1A = compareMatchRegister;

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
  Serial.println();
}


ISR(TIMER1_COMPA_vect){
  isrCallback();
}

