#ifndef Thingspeak_h
#define Thingspeak_h
#include "tools.h"

bool shouldInquireThingspeak();
void thingSpeakGetWindowLux(State state);
void postTelemetryToThingspeak(State state);
#endif