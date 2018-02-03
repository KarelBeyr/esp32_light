#include <Arduino.h>
#include "led.h"

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
    uint32_t duty = (8191 / valueMax) * _min(value, valueMax);
    ledcWrite(channel, duty);
}

void changeLedPwm(State *state)
{
    ledcSetup(LEDC_CHANNEL_0, state->freq, LEDC_TIMER_13_BIT);
    ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
    if (state->automaticMode == false)
        state->currentDuty = state->duty;
    else
    {
        float expectedLux = state->measuredLux / state->utlumStin;
        if (expectedLux > state->desiredLux)
            state->currentDuty = 0;
        else
        {
            float missingLux = (state->desiredLux - expectedLux);
            state->currentDuty = missingLux * state->dutyFor100lux / 100;
        }
    }
    ledcAnalogWrite(LEDC_CHANNEL_0, state->currentDuty * 255 / 100);
}
