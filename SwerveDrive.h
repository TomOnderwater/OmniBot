#ifndef SwerveDrive_h
#define SwerveDrive_h

#include "Arduino.h"
#include "fullServo.h"

#include <ESP32Servo.h>

class SwerveDrive : public fullServo
{
  public:
  SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset, int m_pin);
  void input(int x, int y, int r);
  void setAngleOffset(int _angleOffset);
  void setRotationCenter(int x, int y);
  void place(int x, int y);
  void setSpeed(float _spd);
  void setRotationSpeed(float _spd);
  
  private:
  // variables
  int angleOffset;
  bool reversed;
  int reverseOffset;
  //bool slowMode;
  const int motorNeutral = 58;
  int cx, cy; //rotation point
  int px, py; //physical location
  int rot, mag; // derived from input
  float xRotationMultiplier, yRotationMultiplier;
  float rotationSpeed, translationSpeed;
  //int reversed;

  // objects
  Servo motor;
  // functions
  int getRelativeAngle(int a, int b);
  void calcResponse(int x, int y, int r);
  void moveMotor(int spd);
  int radToDeg(float a);
};

#endif
