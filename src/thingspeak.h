#ifndef Thingspeak_h
#define Thingspeak_h
#include "tools.h"
#include "Arduino.h"

void maybeInquireThingspeak(State *state);
void logMessageToThingspeak(String err);

#endif
