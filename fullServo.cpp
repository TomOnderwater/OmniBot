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

  // PID settings
  _p = 1.3f;
  _i = 0.01f;
  _d = 3.0f;
  _b = 0.3f; //boost

  fpin = f_pin;
  angleSpeed = 0;
  angle = 0;
  Kp = 1.0;
  target = 0;
  errorsum = 0;
  maxError = 40 * round((1.0f / _i));
  cycleState = 0;

  clearLog();

  //attachInterruptArg(f_pin, isr, this, CHANGE); // don't use static interrupt in class, will bug out the memory. this works only 50% of the time

}

void fullServo::clearLog()
{
  for (int i = 0; i < 6; i++) {
    speedLog[i] = 0;
  }
}

void fullServo::addLog()
{
  for (int i = 0; i < 5; i++)
  {
    speedLog[i] = speedLog[i + 1];
  }
  speedLog[5] = angleSpeed;
}

float fullServo::calcAVGSpeed()
{
  float sum = 0;
  for (int i = 0; i < 6; i++)
  {
    sum += speedLog[i];
  }
  return sum / 6;
}

float fullServo::logSpeed()
{
  return speedAVG;
}

int fullServo::getAngleSpeed()
{
  return angleSpeed;
}
int fullServo::getTurns()
{
  return turns;
}

int fullServo::getfPin()
{
  return fpin;
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
    pAngle = angle;
    if (turns >= 0)
      angle = (turns * unitsFC) + theta;
    else if (turns < 0)
      angle = ((turns + 1) * unitsFC) - (unitsFC - theta);

    thetaPre = theta;
    angleSpeed = angle - pAngle;
    addLog();
    speedAVG = calcAVGSpeed();
    cycleState = 0; //reset measurement cycle
  }
}

long fullServo::getAngle()
{
  return angle;
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

  parallax.write(90 + calcPid(diff));
}

int fullServo::calcPid(int diff)
{
  if (abs(diff) < treshold * 3) //if nearly on target
  {
    if (abs(diff) < treshold) //if really on target
    {
      errorsum = 0; //stop _i from doing unnecessary stuff
      return constrain(round(_b * speedAVG * angleSpeed), -90, 90); // LESSER BOOST
    }
    return constrain(round(_b * speedAVG), -90, 90); //BOOST STOP
  }
  else
  {
    errorsum += diff;
    errorsum = constrain(errorsum, -maxError, maxError);
    int response = round(_p * diff);
    response += round(_i * errorsum);
    response += round(_d * angleSpeed);
    //response += round(_b * speedAVG);
    response = constrain(response, -90, 90);
    return response;
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

void fullServo::rotateTo(long degree)
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
