#include <Arduino.h>
#include "tools.h"
#include <Preferences.h>

Preferences preferences;

State::State(int f, int d, int dl, int ml, bool a, float u, float df, int cd)
{
    freq = f;
    duty = d;
    desiredLux = dl;
    measuredLux = ml;
    automaticMode = a;
    utlumStin = u;
    dutyFor100lux = df;
    currentDuty = cd;
};

State::State()
{
};

void readState(State* state)
{
  preferences.begin("my-app", false);
  state->freq = preferences.getInt ("freq");
  state->duty = preferences.getInt ("duty");
  state->desiredLux = preferences.getInt ("desiredLux");
  state->automaticMode = preferences.getBool("automaticMode");
  state->utlumStin = preferences.getFloat("utlumStin");
  state->dutyFor100lux = preferences.getFloat("dutyFor100lux");
  preferences.end();
}

void saveState(State* state)
{
  preferences.begin("my-app", false);
  preferences.putInt("freq", state->freq);
  preferences.putInt("duty", state->duty);
  preferences.putInt("desiredLux", state->desiredLux);
  preferences.putBool("automaticMode", state->automaticMode);
  preferences.putFloat("utlumStin", state->utlumStin);
  preferences.putFloat("dutyFor100lux", state->dutyFor100lux);
  preferences.end();
}

void blink(int times, int msHigh, int msLow)
{
    for (int i = 0; i < times; i++)
    {
        digitalWrite(2, HIGH);
        delay(msHigh);
        digitalWrite(2, LOW);
        delay(msLow);
    }
}
