#include "SwerveDrive.h"
#include "fullServo.h"
#include "Arduino.h"


SwerveDrive::SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset) :
  fullServo(s_pin, f_pin, t_ID)
{
  angleOffset = _angleOffset;
  //this->rotate(angleOffset);
}

void SwerveDrive::input(float rot, float mag)
{
  //call the fullServo functions here (rot)
  this->rotate(rot + angleOffset); //define the target
  int spdOffset = this->move(); //returns movement induced by rotating the axis along a fixed gear
  
  //measure actual speed of the wheel here

  // PID control here

  //apply the speed to the motor driver (mag)
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
