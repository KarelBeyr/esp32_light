#include <Arduino.h>
#include "thingspeak.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "passwords.h"
#include "tools.h"

unsigned long lastConnectionTime = 0;
const char * thingSpeakServer = "api.thingspeak.com";
const unsigned long postingInterval = 15L * 1000L;

//String writeAPIKey = writeAPIKeyWork;
String writeAPIKey = "writeAPIKeyHome";

bool shouldInquireThingspeak()
{
    return (millis() - lastConnectionTime > postingInterval);
}

void postTelemetryToThingspeak(State state) {
  WiFiClient client;
  if (!client.connect(thingSpeakServer, 80)) {
    //Serial.println("connection failed");
    lastConnectionTime = millis();
    client.stop();
    return;     
  } else {
    String data = "field1=" + String(state.freq) + "&field2=" + String(state.duty);
    if (client.connect(thingSpeakServer, 80)) {
      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println("User-Agent: ESP32WiFi/1.1");
      client.println("X-THINGSPEAKAPIKEY: "+ Passwords().ApiKey());
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.print(data.length());
      client.print("\n\n");
      client.print(data);
      
      //Serial.println("RSSI = " + String(field1Data));
      lastConnectionTime = millis();   
    }
  }
  client.stop();
}

void thingSpeakGetWindowLux(State state) {
  HTTPClient http;
  http.begin("http://api.thingspeak.com/channels/414735/fields/2/last.txt");
  int httpCode = http.GET();      
  float res = 0;
  if (httpCode == 200) { //Check for the returning code
    String payload = http.getString();
    state.measuredLux = payload.toFloat();
    // Serial.println(httpCode);
    // Serial.println(payload);
  } else {
   Serial.println("Error on HTTP request");
  }
  http.end(); //Free the resources
}
