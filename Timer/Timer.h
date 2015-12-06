/*
  Timer.h
*/

#ifndef Timer_h
#define Timer_h

#include <Arduino.h> 


class Timer
{
  public:
    // static int *pin;
    static int pin;

    Timer(double frequency, void (*interruptHandler)(int), int handlerData );
    void startClock();
    void stopClock();
    void toSerial();

  private:
    double frequency;
    long clockSpeed = 16000000;
    long prescaler; // 1, 8, 32*, 64, 128*, 256, 1024 (* 16-bit clocks, only)
    long compareMatchRegister;

    void doSomethingSecret(void);
};
#endif
