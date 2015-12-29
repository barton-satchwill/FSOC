/*
  Timer.h
*/

#ifndef Timer_h
#define Timer_h

#include <Arduino.h> 


class Timer
{
  public:
    Timer(long frequency, void (*interruptHandler)() );
    void startClock();
    void stopClock();
    void toSerial();
    void resetClock();

  private:
    double frequency;
    long clockSpeed = 16000000;
    long prescaler; // 1, 8, 32*, 64, 128*, 256, 1024 (* 16-bit clocks, only)
    double compareMatchRegister;
};
#endif
