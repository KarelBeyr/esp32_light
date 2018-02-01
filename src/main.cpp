#include <Arduino.h>
#include "passwords.h"
#include "tools.h"
#include "led.h"
#include "thingspeak.h"
#include "wifimodule.h"

State state = State(3, 50, 200, 0, true, 4, 10);

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  blink(3, 500, 500);
  delay(10);
  setupWifiServer(false);
  blink(5, 300, 300);
  changeLedPwm(&state);
}

void loop()
{
  delay(50); //otestovat zda se neusmazi
  maybeServeClient(false, &state);
  maybeInquireThingspeak(&state);
}
