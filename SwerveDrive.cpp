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
  reversed = false;
  reverseOffset = 0;

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
  //Serial.println(this->getAngle());
  Serial.println(this->logSpeed());
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
  mag = round(sqrt(sq(xmag) + sq(ymag)) * translationSpeed);

  // only if there is relevant input
  if (abs(mag) > 0)
  {
    // convert the vector to euclidians
    int controllerAngle = radToDeg(atan2(xmag, ymag));
    long targetangle = controllerAngle + angleOffset + reverseOffset; // target
    // left hand turn distance
    int dist = getRelativeAngle(this->getAngle(), targetangle);

    // magnitude flips
    // if further than a quarter turn, reversing would be faster
    if (dist > 90 && dist < 270)
  {
    //Serial.println("reversing");
    reversed = !reversed;
    if (reversed)
      {
        reverseOffset = 180;
        dist = getRelativeAngle(this->getAngle(), targetangle + reverseOffset);
      }
      else reverseOffset = 0;
    }

    if (dist < 180) rot = this->getAngle() + dist;
    else rot = this->getAngle() - (360 - dist);
    }
if (reversed) mag *= -1;
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
  return round(a * 57.2957f);
}
int SwerveDrive::getRelativeAngle(long a, long b)
{
  return (b - a) % 360;
}
// returns the (left hand) rotation distance between two angles
/*int SwerveDrive::getRelativeAngle(int a, int b)
{
  return ((720 - (a % 360)) + (b % 360)) % 360;
  //return (((180 - a) % 360) + ((180 + b) % 360)) % 360;
}*/
