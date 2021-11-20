#include "SwerveDrive.h"
#include "fullServo.h"
#include "Arduino.h"
#include <ESP32Servo.h>

SwerveDrive::SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset, int m_pin) :
  fullServo(s_pin, f_pin, t_ID)
{
  angleOffset = _angleOffset;
  translationSpeed = 0.1;
  rotationSpeed = 0.05;

  ESP32PWM::allocateTimer(t_ID + 1);
  motor.setPeriodHertz(50);
  motor.attach(m_pin, 1000, 3000);
  motor.write(motorNeutral);
}

void SwerveDrive::input(int x, int y, int r)
{
  calcResponse(x, y, r);  //update rot and mag
  
  //call the fullServo functions here (rot)
  this->rotateTo(rot); // set the target
  int spdOffset = this->move(); // returns movement induced by rotating the axis along a fixed gear
  
  //measure actual speed of the wheel here

  // PID control here

  //apply the speed to the motor driver (mag)
  //Serial.println(mag);
  //motor.write(100);
  motor.write(motorNeutral + mag);
  //Serial.println(mag);
}

void SwerveDrive::calcResponse(int x, int y, int r)
{
  // combine rotation and translation in one vector
  int xmag = x + round(r * xRotationMultiplier);
  int ymag = y + round(r * yRotationMultiplier);

  // convert the vector to euclidians
  int targetangle = radToDeg(atan2(xmag, ymag)) + angleOffset; // target
  int dist = getRelativeAngle(getAngle(), targetangle);

  if (dist < 180) rot = getAngle() + dist;
  else rot = getAngle() - (360 - dist);
  
//  Serial.print("angle: ");
//  Serial.print(getAngle());
//  Serial.print(" target: ");
//  Serial.print(targetangle);
//  Serial.print("relative angle: ");
//  Serial.print(dist);
//  Serial.print(" rot: ");
//  Serial.println(rot);
//  Serial.println();
  
  mag = round(sqrt(sq(xmag) + sq(ymag)) * translationSpeed);
}

void SwerveDrive::moveMotor(int spd)
{
  motor.write(spd);
}

void SwerveDrive::setSpeed(float _spd)
{
  translationSpeed = _spd;
}

void SwerveDrive::setRotationSpeed(float _spd)
{
  rotationSpeed = _spd;
}
void SwerveDrive::setRotationCenter(int x, int y)
{
  cx = x;
  cy = y;
  // recalculate the orthogonal response for rotation
  xRotationMultiplier = -(py - cy) * rotationSpeed;
  yRotationMultiplier = (px - cx) * rotationSpeed;
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

int SwerveDrive::radToDeg(float a)
{
  return round(a * 57.2957795131);
}
// returns the (left hand) rotation distance between two angles
int SwerveDrive::getRelativeAngle(int a, int b)
{
  return ((180 - a) + (180 + b)) % 360;
}
