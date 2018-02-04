#include "ota.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "thingspeak.h"

void setupOta()
{
  //ArduinoOTA.setHostname("esp_home");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    logMessageToThingspeak("1000");
  });
  ArduinoOTA
   .onEnd([]() {
     logMessageToThingspeak("1999");
  });
  //ArduinoOTA
  // .onProgress([](unsigned int progress, unsigned int total) {
  //   Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  // });
  ArduinoOTA
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) logMessageToThingspeak("1101");
    else if (error == OTA_BEGIN_ERROR) logMessageToThingspeak("1102");
    else if (error == OTA_CONNECT_ERROR) logMessageToThingspeak("1103");
    else if (error == OTA_RECEIVE_ERROR) logMessageToThingspeak("1104");
    else if (error == OTA_END_ERROR) logMessageToThingspeak("1105");
  });

  ArduinoOTA.begin();
}

void handleOta()
{
  ArduinoOTA.handle();
}
