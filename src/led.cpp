#include <Arduino.h>
#include "led.h"

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = (8191 / valueMax) * _min(value, valueMax);
  ledcWrite(channel, duty);
}

void changeLedPwm(int duty, int freq, bool automaticMode, int measuredLux, int desiredLux, float utlumStin, float dutyFor100lux)
{
  ledcSetup(LEDC_CHANNEL_0, freq, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
  int currentDuty = 0;
  if (automaticMode == false) {
    currentDuty = duty;
  } else {
    float expectedLux = measuredLux / utlumStin;
    if (expectedLux > desiredLux) {
      currentDuty = 0;
    } else {
      float missingLux = (desiredLux - expectedLux);
      currentDuty = missingLux * dutyFor100lux / 100;
    }
  }
  ledcAnalogWrite(LEDC_CHANNEL_0, currentDuty * 255 / 100);
}
