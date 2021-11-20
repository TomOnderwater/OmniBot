#include "ButtonMaker.h"
#include "SwerveDrive.h"
//#include "fullServo.h"
#include <Arduino.h>
#include <AsyncTimer.h>

//### SELECT YOUR INPUT METHOD
//#define PS4INPUT
#define APINPUT

#ifdef PS4INPUT
#include <PS4Controller.h>
//### BLUETOOTH MAC ADDRESS
char * mac = "14:C2:13:14:9C:7D";
#endif

#ifdef APINPUT
#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "web.h"
#endif

#define UPDATEFRAME 20    // 1000 / hz = UPDATEFRAME

//### DEBUG SETTINGS
#define DEBUG // comment out for no serial interface

#ifdef DEBUG
#define BUFFSIZE 256
char deBuff[BUFFSIZE];
#endif

//### DEFINE SWERVEDRIVES:

// swerveDrive(int servopin, int servofeedback,int timerID, int angleOffset, int motorpin)
SwerveDrive module = SwerveDrive(26, 35, 0, 0, 12);

struct controls
{
  float y, x, r;
};

struct controls input;

ButtonMaker cross; //toggle buttons

AsyncTimer t; //javascript style callback
unsigned long updatetimer;

void isr(void* arg) {
  fullServo* s = static_cast<fullServo*>(arg);
  s->feedback();
}

void setup() {
  t.setup(); // init async timer
  zeroInput(); //STOP EVERYTHING

  Serial.begin(115200);

#ifdef DEBUG
  emptyDeBuff();
#endif

#ifdef APINPUT
  configureAP();
#endif


#ifdef PS4INPUT
  PS4.begin(mac);
#endif

  //### SETUP OF MODULES BEGIN
  module.setAccuracy(2);  // fault tolerance

  //### attach interrupts to the swervedrive modules:
  attachInterruptArg(module.getfPin(), isr, &module, CHANGE);

  //### place the modules in space
  module.place(0, 0);

  //### set the rotation center
  module.setRotationCenter(0, 0);

  //### continuos counting of turns
 // module.setContinuous(false);

  //## SETUP OF MODULES END

  //### SETUP TIMERS
  updatetimer = millis();
#ifdef DEBUG
  t.setInterval([&]()
  {
    module.getDiagnostics(deBuff);
    printDebug();
  }, 100);
#endif

}   // END OF SETUP


void loop() {
  // check connection to controller
#ifdef PS4INPUT
  if (PS4.isConnected()) input = updateByPS4();
  else zeroInput();
#endif

#ifdef APINPUT
  auto client = server.accept();
  client.onMessage(handle_message);
  while (client.available()) {
    client.poll();
    updateBot();
  }
#elif
  updateBot();
#endif


}

void updateBot()
{
  if (millis() > updatetimer + UPDATEFRAME)
  {
    // convert controller input (vectors) to euclidians
    //float rotation = atan2(input.y, input.x);
    //float magnitude = sqrt(sq(input.x) + sq(input.y));

    // update the module
    //module.input(rotation, magnitude);
    module.input(input.x, input.y, input.r);

//    Serial.print("X: ");
//    Serial.print(input.x);
//    Serial.print(" Y: )");
//    Serial.println(input.y);

    updatetimer = millis();
  }
}
void zeroInput()
{
  input.x = 0;
  input.y = 0;
  input.r = 0;
}


#ifdef PS4INPUT
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
#endif
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
