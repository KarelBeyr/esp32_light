#include <Arduino.h>
#include "tools.h"

State::State(int f, int d, int dl, int ml, bool a, float u, float df) {
  freq = f;
  duty = d;
  desiredLux = dl;
  measuredLux = ml;
  automaticMode = a;
  utlumStin = u;
  dutyFor100lux = df;
};

void blink(int times, int msHigh, int msLow) {
  for (int i = 0; i < times; i++ ) {
    digitalWrite(2, HIGH);
    delay(msHigh);
    digitalWrite(2, LOW);
    delay(msLow);
  }
}