#ifndef Wifimodule_h
#define Wifimodule_h
#include "tools.h"

void setupWifiServer(bool logToSerial);
bool maybeServeClient(bool logToSerial, State state);
#endif