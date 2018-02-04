#include <Arduino.h>
#include "thingspeak.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "passwords.h"
#include "tools.h"
#include "led.h"

unsigned long lastConnectionTime = 0;
const char *thingSpeakServer = "api.thingspeak.com";
const unsigned long postingInterval = 15L * 1000L;

void postDataToThingspeak(String data, String apiKey)
{
  WiFiClient client;
  if (!client.connect(thingSpeakServer, 80))
  {
      //Serial.println("connection failed");
      client.stop();
      return;
  }
  else
  {
      if (client.connect(thingSpeakServer, 80))
      {
          client.println("POST /update HTTP/1.1");
          client.println("Host: api.thingspeak.com");
          client.println("Connection: close");
          client.println("User-Agent: ESP32WiFi/1.1");
          client.println("X-THINGSPEAKAPIKEY: " + apiKey);
          client.println("Content-Type: application/x-www-form-urlencoded");
          client.print("Content-Length: ");
          client.print(data.length());
          client.print("\n\n");
          client.print(data);
      }
  }
  client.stop();
}

String iso8601date() {
    int month, day, year;
    char buff[16];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", buff, &day, &year);
    month = (strstr(month_names, buff)-month_names)/3+1;
    sprintf(buff, "%d%02d%02d.txt", year, month, day);
    String res(buff);
    return res;
}

String iso8601time() {
  String res = "";
  String input(__TIME__);
  res = res + input.substring(0, 2) + input.substring(3,5);
  return res;
}

String replaceForbiddenChars(String data)
{
  String copy = data;
  copy.replace(" ", "_");
  copy.replace(":", "_");
  return copy;
}

void logMessageToThingspeak(String message)
{
  String data = "field1=" + Passwords().CurrentLocationName()
     + "&field2=" + iso8601date()
     + "&field3=" + iso8601time()
    + "&field4=" + message;
  postDataToThingspeak(data, "NB4KBVX0CE6J6HUF");
}

void postTelemetryToThingspeak(State *state)
{
  String data = "field1=" + String(state->freq) + "&field2=" + String(state->duty) +
      "&field3=" + String(state->desiredLux) + "&field4=" + String(state->measuredLux) +
      "&field5=" + String(state->automaticMode) + "&field6=" + String(state->utlumStin) +
      "&field7=" + String(state->dutyFor100lux) + "&field8=" + String(state->currentDuty);
  postDataToThingspeak(data, Passwords().ApiKey());
  lastConnectionTime = millis();
}

void thingSpeakGetWindowLux(State *state)
{
    HTTPClient http;
    http.begin("http://api.thingspeak.com/channels/414735/fields/1/last.txt");
    int httpCode = http.GET();
    float res = 0;
    if (httpCode == 200)
    { //Check for the returning code
        String payload = http.getString();
        state->measuredLux = payload.toFloat();
    }
    else
    {
        Serial.println("Error on HTTP request");
    }
    http.end();
}

void maybeInquireThingspeak(State *state)
{
    if (millis() - lastConnectionTime > postingInterval)
    {
        postTelemetryToThingspeak(state);
        thingSpeakGetWindowLux(state);
        changeLedPwm(state);
    }
}
