#ifndef fullServo_h
#define fullServo_h

#include "Arduino.h"
#include <ESP32Servo.h>

class fullServo
{
  public:
    fullServo(byte s_pin, byte f_pin, int t);
    void setKp(float _Kp);
    void rotateTo(long degree);
    void setAccuracy(int _treshold);
    void feedback();
    int getfPin();
    void write(int val);
    int move();
    void getDiagnostics(char * deBuff);
    long getAngle();
    int getTurns();
    int getAngleSpeed();
    float logSpeed();

  private:
    byte fpin;
    void clearLog();
    void addLog();
    float calcAVGSpeed();
    Servo parallax;
    long angle, pAngle;
    float thetaPre;
    unsigned long firstRise, secondRise, fall;
    int turns;
    long target;
    int maxError;
    int treshold;
    float Kp;
    float _p, _i, _d, _b;
    const int unitsFC = 360;
    const float dcMin = 0.029;
    const float dcMax = 0.971;
    const int dutyScale = 1;
    const int q2min = unitsFC / 4;
    const int q3max = q2min * 3;
    int calcPid(int diff);
    int angleSpeed;
    int speedLog[6];
    float speedAVG;
    float errorsum;
    void updateAngle();
    void moveToTarget();

    int cycleState;

};

#endif
