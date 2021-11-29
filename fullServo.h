#ifndef fullServo_h
#define fullServo_h

#include "Arduino.h"
#include <ESP32Servo.h>

class fullServo
{
  public:
  fullServo(byte s_pin, byte f_pin, int t);
  void setKp(float _Kp);
  void rotateTo(int degree);
  void setAccuracy(int _treshold);
  void feedback();
  int getfPin();
  void write(int val);
  int move();
  void getDiagnostics(char * deBuff);
  int getAngle();
  int getTurns();
  
  private:
  byte fpin;
  Servo parallax;
  volatile int angle;
  float thetaPre;
  unsigned long firstRise, secondRise, fall;
  int turns;
  int target;
  int treshold;
  float Kp;
  const int unitsFC = 360;
  const float dcMin = 0.029;
  const float dcMax = 0.971;
  const int dutyScale = 1;
  const int q2min = unitsFC / 4;
  const int q3max = q2min * 3;
  float errorsum;
  void updateAngle();
  void moveToTarget();
  
  int cycleState;
  
};

#endif
