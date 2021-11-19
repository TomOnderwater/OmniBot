#ifndef SwerveDrive_h
#define SwerveDrive_h

#include "Arduino.h"
#include "fullServo.h"

class SwerveDrive : public fullServo
{
  public:
  SwerveDrive(int s_pin, int f_pin, int t_ID, int _angleOffset);
  void input(float rot, float mag);
  void setAngleOffset(int _angleOffset);
  
  private:
  int angleOffset;
  bool reversed;

  int getRelativeAngle(int a, int b);
};

#endif
