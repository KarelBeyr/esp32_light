#ifndef Wifimodule_h
#define Wifimodule_h
#include "tools.h"

void setupWifiServer(bool logToSerial);
void maybeServeClient(bool logToSerial, State *state);
#endif