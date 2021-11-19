#include <PS4Controller.h>
#include "ButtonMaker.h"
#include "SwerveDrive.h"
//#include "fullServo.h"
#include <Arduino.h>

#define DEBUG true

//### BLUETOOTH MAC ADDRESS
char * mac = "14:C2:13:14:9C:7D";


//### ATTACHED SWERVEDRIVES:

// swerveDrive(int servopin, int servofeedback,int timerID, int angleOffset)
SwerveDrive module = SwerveDrive(26, 35, 0, 0);

struct controls
{
  float y, x, r;
};

struct controls input;

ButtonMaker cross; //toggle buttons

unsigned long updatetimer;

void isr(void* arg) {
    fullServo* s = static_cast<fullServo*>(arg);
    s->feedback();
}

void setup() {
  zeroInput(); //STOP EVERYTHING
  Serial.begin(115200); 
  PS4.begin(mac);
  module.setKp(1.0); //this should be removed
  module.setAccuracy(2);

  //attach interrupts to the swervedrive modules:
  attachInterruptArg(module.getfPin(), isr, &module, CHANGE);

  //place the modules
  
  
  updatetimer = millis();


}   // END OF SETUP


void loop() {
  // check connection to controller
  if (PS4.isConnected()) input = updateByPS4();
  else zeroInput();

  if (millis() > updatetimer + 20)
  {
    float rotation = atan2(input.y, input.x);
    float mag = sqrt(sq(input.x) + sq(input.y));
    module.input(rotation, mag);
    
    updatetimer = millis();
  }  
}

void zeroInput()
{
  input.x = 0;
  input.y = 0;
  input.r = 0;
}

struct controls updateByPS4() {
    struct controls res;

    res.x = 0;
    res.y = 0;
    res.r = 0;


    // update here

    // translation
    if (abs(PS4.LStickX()) > 10) res.x = PS4.LStickX();
    if (abs(PS4.LStickY()) > 10) res.y = PS4.LStickY();

    // rotation
    if (abs(PS4.RStickX()) > 10) res.r = PS4.RStickX();
    
    return res;
  }
  /*
if (PS4.L2()) speedBackward = PS4.L2Value();
  else speedBackward = 0;

  if (PS4.R2())speedForward = PS4.R2Value();
  else speedForward = 0;

  if (cross.isPress(PS4.Cross())) lowspeed = !lowspeed;

  maxpower = lowspeed ? LOWSPEED : HIGHSPEED;
  
  if (speedForward > 0 && speedBackward > 0) power = nopower;
  else if (speedForward > 0) power = map(speedForward, 0, 255, nopower, nopower + maxpower);
  else power = map(speedBackward, 0, 255, nopower, nopower - maxpower);
 
 if (PS4.RStickX()) turning = map(PS4.RStickX(), -128, 127, neutral + 90, neutral - 90); 
  */
