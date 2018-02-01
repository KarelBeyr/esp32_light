#include <WiFi.h>
#include <Arduino.h>
#include "wifimodule.h"
#include "passwords.h"
#include "led.h"

//http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
const char *webPages[] = {
    "",
    "<script>\n"
    "function sendData() {\n"
    "  var freqRaw = document.getElementById('freqInputId').value;\n"
    "  var freq = Math.pow(10, Math.floor(freqRaw/2)) * ((freqRaw % 2) * 2 + 1);\n"
    "  var duty = document.getElementById('dutyInputId').value;\n"
    "  var lux = document.getElementById('luxInputId').value;\n"
    "  var override = document.getElementById('overrideInputId').checked;\n"
    "  document.getElementById('dutySpanId').innerHTML = duty;\n"
    "  document.getElementById('freqSpanId').innerHTML = freq.toPrecision(1);\n"
    "  document.getElementById('luxSpanId').innerHTML = lux;\n"
    "  var overrideString = '';\n"
    "  if (override == true) overrideString = 'automatic'; else overrideString = 'manual';\n"
    "  document.getElementById(\"overrideSpanId\").innerHTML = overrideString;\n"
    "  var url = '/data/' + duty + '/' + freq + '/' + lux + '/' + override + '/';\n"
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
    "<input type='range' min='0' max='15' value='5' oninput='sendData()' onchange='sendData()' id='freqInputId'>Switching frequency: <span id='freqSpanId'>?</span> Hz<br /><br />\n"
    "<input type='range' min='0' max='1000' value='200' oninput='sendData()' onchange='sendData()' id='luxInputId'>Automatic light intensity: <span id='luxSpanId'>?</span> Lux<br /><br />\n"
    "<input type='range' min='0' max='100' value='50' oninput='sendData()' onchange='sendData()' id='dutyInputId'>Manual duty: <span id='dutySpanId'>?</span>%<br /><br />\n"
    "<input type='checkBox' onchange='sendData()' id='overrideInputId'>Mode: <span id='overrideSpanId'>?</span><br /><br />\n"
    "",
    "<script>\n"
    "function sendData() {\n"
    "  var ratio = document.getElementById('ratioInputId').value;\n"
    "  var coeff = document.getElementById('dutyCoeffInputId').value;\n"
    "  var url = '/config/' + ratio + '/' + coeff + '/';\n"
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
    "Kolikrat je nizsi intenzita na danem miste oproti oknu: <input type='text' value='1' id='ratioInputId'><br /><br />\n"
    "Kolik je traba svitit duty aby se zvysila intenzita o 100 lux: <input type='text' value='1' id='dutyCoeffInputId'><br /><br />\n"
    "<input type='submit' onclick='sendData()'>\n"
    "\n"
    ""};

WiFiServer httpServer(80);

void setupWifiServer(bool logToSerial)
{
    if (logToSerial)
    {
        Serial.println("");
        Serial.print("Connecting to ");
        Serial.println(Passwords().WifiSSID());
    }
    WiFi.begin(Passwords().WifiSSID(), Passwords().WifiPassword());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        if (logToSerial)
            Serial.print(".");
    }
    if (logToSerial)
    {
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
    httpServer.begin();
}

String getParameterAfterSlash(String line, int nr)
{
    int idx = line.indexOf(' ');
    for (int i = 1; i <= nr; i++)
    {
        idx = line.indexOf('/', idx + 1);
    }
    int endIdx = line.indexOf('/', idx + 1);
    return line.substring(idx + 1, endIdx);
}

void processDataLine(bool logToSerial, State *state, String line)
{
    state->duty = getParameterAfterSlash(line, 2).toInt();
    state->freq = getParameterAfterSlash(line, 3).toInt();
    state->desiredLux = getParameterAfterSlash(line, 4).toInt();
    String overrideString = getParameterAfterSlash(line, 5);
    if (overrideString == "true")
    {
        state->automaticMode = true;
    }
    else
    {
        state->automaticMode = false;
    }
    if (logToSerial)
    {
        Serial.println("duty: " + String(state->duty) + ", freq: " + String(state->freq) + ", desired lux: " + String(state->desiredLux) + ", mode: " + String(state->automaticMode));
    }
    changeLedPwm(state);
}

void processConfigLine(bool logToSerial, State *state, String line)
{
    state->utlumStin = getParameterAfterSlash(line, 2).toInt();
    state->dutyFor100lux = getParameterAfterSlash(line, 3).toInt();
    if (logToSerial)
    {
        Serial.println("utlum: " + String(state->utlumStin) + ", dutyFor100lux: " + String(state->dutyFor100lux));
    }
    changeLedPwm(state);
}

void maybeServeClient(bool logToSerial, State *state)
{
    bool res = false;
    WiFiClient client = httpServer.available(); // listen for incoming clients
    if (client)
    { // if you get a client,
        if (logToSerial)
        {
            Serial.println("New Client.");
        } // print a message out the serial port
        int pageType = 0;
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                if (c == '\n')
                { // if the byte is a newline character
                    if (logToSerial)
                        Serial.println(currentLine);
                    if (currentLine.startsWith("PUT /data/"))
                        processDataLine(logToSerial, state, currentLine);
                    if (currentLine.startsWith("PUT /config/"))
                        processConfigLine(logToSerial, state, currentLine);
                    if (currentLine.startsWith("GET "))
                        pageType = 1;
                    if (currentLine.startsWith("GET /config"))
                        pageType = 2;
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        client.println("HTTP/1.1 200 OK");        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        client.println("Content-type:text/html"); // and a content-type so the client knows what's coming, then a blank line:
                        client.println();
                        client.print(webPages[pageType]); // the content of the HTTP response follows the header:
                        client.println();                 // The HTTP response ends with another blank line:
                        break;                            // break out of the while loop:
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        client.stop(); // close the connection:
        if (logToSerial)
        {
            Serial.println("Client Disconnected.");
        }
    }
}