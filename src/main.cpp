#include <Arduino.h>
#include "passwords.h"
#include "tools.h"
#include "led.h"
#include "thingspeak.h"
#include "wifimodule.h"

// int duty = 50;
// int freq = 3;
// int desiredLux = 200;
// float measuredLux = 0.0;
// bool automaticMode;
// float utlumStin = 4;   //kolikrat je mene svetla ve stinu nez na okne?
// float dutyFor100lux = 10;    //kolik musime mit duty, abychom namerili v danem miste 100 luxu (kdyz je jinak uplna tma)

State state = State(3, 50, 200, 0, true, 4, 10);

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  blink(3, 500, 500);
  delay(10);
  setupWifiServer(false);
  blink(5, 300, 300);
  changeLedPwm(state.duty, state.freq, state.automaticMode, state.measuredLux, state.desiredLux, state.utlumStin, state.dutyFor100lux);
}

void loop(){
 
  //Serial.print(".");
  delay(10);
  if (shouldInquireThingspeak()) {
    postTelemetryToThingspeak(state);
    thingSpeakGetWindowLux(state);
    changeLedPwm(state.duty, state.freq, state.automaticMode, state.measuredLux, state.desiredLux, state.utlumStin, state.dutyFor100lux);
  }
}
