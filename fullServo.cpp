#include "fullServo.h"
#include "Arduino.h"
#include <ESP32Servo.h>


//static void isr(void* arg) {
//  fullServo* s = static_cast<fullServo*>(arg);
//  s->feedback();
//}

fullServo::fullServo(byte s_pin, byte f_pin, int t)
{
  ESP32PWM::allocateTimer(t);
  parallax.setPeriodHertz(50);
  parallax.attach(s_pin, 1280, 1720);

  pinMode(f_pin, INPUT_PULLUP);

  fpin = f_pin;
  Kp = 1.0;
  target = 0;
  errorsum = 0;
  cycleState = 0;

  //attachInterruptArg(f_pin, isr, this, CHANGE); // this works only 50% of the time

}

int fullServo::getfPin()
{
  return fpin;
}

int fullServo::Angle()
{
  return angle;
}

void fullServo::setKp(float _Kp) {
  Kp = _Kp;
}

void fullServo::setAccuracy(int _treshold)
{
  treshold = _treshold;
}

void fullServo::updateAngle()
{
  if (cycleState == 3) //there is a completed full cycle
  {
    int tCycle = secondRise - firstRise;
    if ((tCycle < 1000) || (tCycle > 1200))
    {
      cycleState = 0;
      return;
    }
    unsigned int tHigh = fall - firstRise;
    unsigned int tLow = secondRise - fall;

    float dc = (dutyScale * tHigh) / (float)tCycle;
    float theta = ((dc - dcMin) * unitsFC) / (dcMax - dcMin);

    if (theta < 0.0)
      theta = 0.0;
    else if (theta > (unitsFC - 1.0))
      theta = unitsFC - 1.0;

    if ((theta < q2min) && (thetaPre > q3max))
      turns++;
    else if ((thetaPre < q2min) && (theta > q3max))
      turns--;

    if (turns >= 0)
      angle = (turns * unitsFC) + theta;
    else if (turns < 0)
      angle = ((turns + 1) * unitsFC) - (unitsFC - theta);

    thetaPre = theta;

    cycleState = 0; //reset measurement cycle
  }
}

int fullServo::move()
{
  updateAngle();
  moveToTarget();

  return 0;
}

void fullServo::moveToTarget()
{
  int diff = angle - target;

  if (abs(diff) < treshold)
  {
    parallax.write(90); // no movement
    errorsum = 0;
  }
  else if (diff < 0)
  {
    //turn left
    //errorsum += diff * 0.01;
    //if (diff < 45) errorsum += (float) diff * 0.01;
    diff = constrain(abs(diff), 0, 80);
    if (diff < 15) errorsum += (float)diff * 0.01;
    parallax.write(90 - (diff + round(errorsum)));
  }
  else
  {
    diff = constrain(abs(diff), 0, 80);
    if (diff < 15) errorsum += (float)diff * 0.01;
    parallax.write(90 + (diff + round(errorsum)));
  }
}

void fullServo::getDiagnostics(char * deBuff)
{
   strcat(deBuff, "target: ");
   itoa(target, (char *) &deBuff + strlen(deBuff), 10);
   strcat(deBuff, " angle: ");
   itoa(angle, (char *) &deBuff + strlen(deBuff), 10);
   strcat(deBuff, "\n");
}
void fullServo::write(int val)
{
  parallax.write(val);
}

void fullServo::rotateTo(int degree)
{
  target = degree;
}

void fullServo::feedback() {
  if (cycleState == 3)
    return;
  else if (cycleState == 0 && digitalRead(fpin)) //first high
  {
    firstRise = micros();
    cycleState ++;
  }
  else if (cycleState == 1 && !digitalRead(fpin)) //first low
  {
    fall = micros();
    cycleState ++;
  }
  else if (cycleState == 2 && digitalRead(fpin)) //second high
  {
    secondRise = micros();
    cycleState ++;
  }

}
