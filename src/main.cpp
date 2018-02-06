#include <Arduino.h>
#include "passwords.h"
#include "tools.h"
#include "led.h"
#include "thingspeak.h"
#include "wifimodule.h"
#include "ota.h"

State state = State();

void setup()
{
  setupLed();
  readState(&state);
  Serial.begin(115200);
  setupWifiServer(false);
  setupOta();
  changeLedPwm(&state);
}

void loop()
{
  delay(50); //otestovat zda se neusmazi
  maybeServeClient(false, &state);
  maybeInquireThingspeak(&state);
  handleOta();
}
