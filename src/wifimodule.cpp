#include <WiFi.h>
#include <Arduino.h>
#include "wifimodule.h"
#include "passwords.h"

//http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
const char* webPage = "<script>\n"
"function sendData() {\n"
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
"  xmlHttp.open('PUT', url, true); // true for asynchronous \n"
"  xmlHttp.send(null);\n"
"}\n"
"</script>\n"
"<input type='range' min='0' max='15' value='5' oninput=\"sendData()\" onchange=\"sendData()\" id=\"freqInputId\">Switching frequency: <span id=\"freqSpanId\">?</span> Hz<br /><br />\n"
"<input type='range' min='0' max='1000' value='200' oninput=\"sendData()\" onchange=\"sendData()\" id=\"luxInputId\">Automatic light intensity: <span id=\"luxSpanId\">?</span> Lux<br /><br />\n"
"<input type='range' min='0' max='100' value='50' oninput=\"sendData()\" onchange=\"sendData()\" id=\"dutyInputId\">Manual duty: <span id=\"dutySpanId\">?</span>%<br /><br />\n"
"<input type='checkBox' onchange=\"sendData()\" id=\"overrideInputId\">Mode: <span id=\"overrideSpanId\">?</span><br /><br />\n"
"Kolikrat je nizsi intenzita na danem miste oproti oknu: <input type=\"text\" value='1' id=\"ratioInputId\"><br /><br />\n"
"Kolik je traba svitit duty aby se zvysila intenzita o 100 lux: <input type=\"text\" value='1' id=\"dutyCoeffInputId\">\n"
"\n";

WiFiServer httpServer(80);

void setupWifiServer(bool logToSerial) {
  if(logToSerial) {
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.println(Passwords().WifiSSID());
  }
  WiFi.begin(Passwords().WifiSSID(), Passwords().WifiPassword());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(logToSerial) {Serial.print(".");}
  }
  if(logToSerial) {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  httpServer.begin();
}

bool maybeServeClient(bool logToSerial, State state) {
  bool res = false;
  WiFiClient client = httpServer.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
  if(logToSerial) {Serial.println("New Client.");}           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character

         if(logToSerial) {Serial.println(currentLine);}
          if (currentLine.startsWith("PUT /")) {
            int startIdx = currentLine.indexOf(' ');
            if (startIdx > 0) {
              int midIdx1 = currentLine.indexOf('/', startIdx + 2); //zaciname totiz mezerou a lomitkem a to prvni lomitko nas netrapi
              int midIdx2 = currentLine.indexOf('/', midIdx1 + 1);
              int midIdx3 = currentLine.indexOf('/', midIdx2 + 1);
              int endIdx = currentLine.indexOf(' ', midIdx3 + 1);
              if (endIdx > 0) {
                String dutyString = currentLine.substring(startIdx + 2, midIdx1);
                state.duty = dutyString.toInt();
                String freqString = currentLine.substring(midIdx1 + 1, midIdx2);
                state.freq = freqString.toInt();
                String desiredLuxString = currentLine.substring(midIdx2 + 1, midIdx3);
                state.desiredLux = desiredLuxString.toInt();
                String overrideString = currentLine.substring(midIdx3 + 1, endIdx);
                if (overrideString == "true") {state.automaticMode = true;} else {state.automaticMode = false;}
                //if(logToSerial) {Serial.println("duty: " + String(duty) + ", freq: " + String(freq) + ", desired lux: " + String(desiredLux) + ", mode: " + String(automaticMode));}
                res = true;
              }
            }
          }

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");      // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            client.println("Content-type:text/html");       // and a content-type so the client knows what's coming, then a blank line:
            client.println();
            client.print(webPage);  // the content of the HTTP response follows the header:
            client.println();   // The HTTP response ends with another blank line:
            
            break;  // break out of the while loop:
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    client.stop();  // close the connection:
    if(logToSerial) {Serial.println("Client Disconnected.");}
  }
  return res;
}