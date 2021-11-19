#include "SwerveDrive.h"
#include "fullServo.h"
#include "Arduino.h"
#include <ESP32Servo.h>


SwerveDrive::SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset, int m_pin) :
  fullServo(s_pin, f_pin, t_ID)
{
  angleOffset = _angleOffset;

  ESP32PWM::allocateTimer(t_ID + 1);
  motor.setPeriodHertz(50);
  motor.attach(m_pin, 1000, 2000);
}

void SwerveDrive::input(float rot, float mag)
{
  //call the fullServo functions here (rot)
  this->rotateTo(rot + angleOffset); // set the target
  int spdOffset = this->move(); // returns movement induced by rotating the axis along a fixed gear
  
  //measure actual speed of the wheel here

  // PID control here

  //apply the speed to the motor driver (mag)
}

void SwerveDrive::moveMotor(int spd)
{
  motor.write(spd);
}

void SwerveDrive::setRotationCenter(int x, int y)
{
  cx = x;
  cy = y;
}

void SwerveDrive::place(int x, int y)
{
  px = x;
  py = y;
}

void SwerveDrive::setAngleOffset(int _angleOffset)
{
  angleOffset = _angleOffset;
}
// returns the (left hand) rotation distance between two angles
int SwerveDrive::getRelativeAngle(int a, int b)
{
  return ((360 - a) + (360 + b)) % 720;
}
