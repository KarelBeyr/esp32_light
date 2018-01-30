#include <WiFi.h>
#include <HTTPClient.h>
#include "passwords.h"

// const char* ssid     = "efractal";
const char* ssid     = "beyr-network";

const char* password = passwordHome;
String writeAPIKey = writeAPIKeyHome;

// ThingSpeak Settings
char thingSpeakServer[] = "api.thingspeak.com";

const unsigned long postingInterval = 15L * 1000L;
unsigned long lastConnectionTime = 0;

//HTTP server settings
//http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
const char* webPage = "<script>\n"
"function httpGet() {\n"
"  var freqRaw = document.getElementById(\"freqInputId\").value;\n"
"  var freq = Math.pow(10, Math.floor(freqRaw/2)) * ((freqRaw % 2) * 2 + 1);\n"
"  var duty = document.getElementById(\"dutyInputId\").value;\n"
"  var lux = document.getElementById(\"luxInputId\").value;\n"
"  var override = document.getElementById(\"overrideInputId\").checked;\n"
"  document.getElementById(\"dutySpanId\").innerHTML = duty;\n"
"  document.getElementById(\"freqSpanId\").innerHTML = freq.toPrecision(1);\n"
"  document.getElementById(\"luxSpanId\").innerHTML = lux;\n"
"  var overrideString = \"\";\n"
"  if (override == true) overrideString = \"automatic\"; else overrideString = \"manual\";\n"
"  document.getElementById(\"overrideSpanId\").innerHTML = overrideString;\n"
"  var url = '/' + duty + '/' + freq + '/' + lux + \"/\" + override;\n"
"  console.log(url);\n"
"  var xmlHttp = new XMLHttpRequest();\n"
"  xmlHttp.onreadystatechange = function() { \n"
"      if (xmlHttp.readyState == 4 && xmlHttp.status == 200)\n"
"          pending=false;\n"
"  }\n"
"  xmlHttp.open('GET', url, true); // true for asynchronous \n"
"  xmlHttp.send(null);\n"
"}\n"
"</script>\n"
"<input type='range' min='0' max='15' value='5' oninput=\"httpGet()\" onchange=\"httpGet()\" id=\"freqInputId\">Switching frequency: <span id=\"freqSpanId\">?</span> Hz<br /><br />\n"
"<input type='range' min='0' max='1000' value='200' oninput=\"httpGet()\" onchange=\"httpGet()\" id=\"luxInputId\">Automatic light intensity: <span id=\"luxSpanId\">?</span> Lux<br /><br />\n"
"<input type='range' min='0' max='100' value='50' oninput=\"httpGet()\" onchange=\"httpGet()\" id=\"dutyInputId\">Manual duty: <span id=\"dutySpanId\">?</span>%<br /><br />\n"
"<input type='checkBox' onchange=\"httpGet()\" id=\"overrideInputId\">Mode: <span id=\"overrideSpanId\">?</span>\n"
"\n";

WiFiServer httpServer(80);

// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0 0

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT 13

#define LED_PIN 2 

int duty = 50;
int freq = 3;
int desiredLux = 200;
float measuredLux = 0.0;
bool automaticMode;
const float UTLUM_STIN = 4;   //kolikrat je mene svetla ve stinu nez na okne?
const float DUTY_NEEDED_TO_MEASURE_100_LUX = 10;    //kolik musime mit duty, abychom namerili v danem miste 100 luxu (kdyz je jinak uplna tma)

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = (8191 / valueMax) * _min(value, valueMax);
  ledcWrite(channel, duty);
}

void changeLedPwm()
{
  ledcSetup(LEDC_CHANNEL_0, freq, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
  int currentDuty = 0;
  if (automaticMode == false) {
    currentDuty = duty;
  } else {
    float expectedLux = measuredLux / UTLUM_STIN;
    if (expectedLux > desiredLux) {
      currentDuty = 0;
    } else {
      float missingLux = (desiredLux - expectedLux);
      currentDuty = missingLux * DUTY_NEEDED_TO_MEASURE_100_LUX / 100;
    }
  }
  ledcAnalogWrite(LEDC_CHANNEL_0, currentDuty * 255 / 100);
}

void httpRequest() {
  WiFiClient client;
  if (!client.connect(thingSpeakServer, 80)) {
    //Serial.println("connection failed");
    lastConnectionTime = millis();
    client.stop();
    return;     
  } else {
    String data = "field1=" + String(freq) + "&field2=" + String(duty);
    if (client.connect(thingSpeakServer, 80)) {
      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println("User-Agent: ESP32WiFi/1.1");
      client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
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

void httpGet() {
  HTTPClient http;
  http.begin("http://api.thingspeak.com/channels/414735/fields/2/last.txt");
  int httpCode = http.GET();      
  if (httpCode == 200) { //Check for the returning code
    String payload = http.getString();
    measuredLux = payload.toFloat();
    // Serial.println(httpCode);
    // Serial.println(payload);
  } else {
   Serial.println("Error on HTTP request");
  }
  http.end(); //Free the resources
}

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  for (int i = 0; i < 3; i++ ) {
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
  }

  delay(10);
  // Serial.println("");
  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
 //     Serial.print(".");
  }

  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  for (int i = 0; i < 5; i++ ) {
    digitalWrite(2, HIGH);
    delay(300);
    digitalWrite(2, LOW);
    delay(300);
  }

  httpServer.begin();
  changeLedPwm();
}

void loop(){
  WiFiClient client = httpServer.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
  //  Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
         // Serial.println(currentLine);
          if (currentLine.startsWith("GET /")) {
            int startIdx = currentLine.indexOf(' ');
            if (startIdx > 0) {
              int midIdx1 = currentLine.indexOf('/', startIdx + 2); //zaciname totiz mezerou a lomitkem a to prvni lomitko nas netrapi
              int midIdx2 = currentLine.indexOf('/', midIdx1 + 1);
              int midIdx3 = currentLine.indexOf('/', midIdx2 + 1);
              int endIdx = currentLine.indexOf(' ', midIdx3 + 1);
              if (endIdx > 0) {
                String dutyString = currentLine.substring(startIdx + 2, midIdx1);
                duty = dutyString.toInt();
                String freqString = currentLine.substring(midIdx1 + 1, midIdx2);
                freq = freqString.toInt();
                String desiredLuxString = currentLine.substring(midIdx2 + 1, midIdx3);
                desiredLux = desiredLuxString.toInt();
                String overrideString = currentLine.substring(midIdx3 + 1, endIdx);
                if (overrideString == "true") {automaticMode = true;} else {automaticMode = false;}
                //Serial.println("duty: " + String(duty) + ", freq: " + String(freq) + ", desired lux: " + String(desiredLux) + ", mode: " + String(automaticMode));
                changeLedPwm();
              }
            }
          }

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(webPage);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
  //  Serial.println("Client Disconnected.");
  }
  //Serial.print(".");
  delay(10);
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
    httpGet();
    changeLedPwm();
  }
}
