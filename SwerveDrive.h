#ifndef SwerveDrive_h
#define SwerveDrive_h

#include "Arduino.h"
#include "fullServo.h"

#include <ESP32Servo.h>

class SwerveDrive : public fullServo
{
  public:
  SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset, int m_pin);
  void input(float rot, float mag);
  void setAngleOffset(int _angleOffset);
  void setRotationCenter(int x, int y);
  void place(int x, int y);
  
  private:
  // variables
  int angleOffset;
  bool reversed;
  int cx, cy; //rotation point
  int px, py; //physical location
  // objects
  Servo motor;
  // functions
  int getRelativeAngle(int a, int b);
  void moveMotor(int spd);
};

#endif
