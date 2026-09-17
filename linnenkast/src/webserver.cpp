#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// ---------- Servers ----------
static ESP8266WebServer webServer(80);
static WebSocketsServer webSocket(81);

// ---------- Web page ----------
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Linnenkast</title>

  <style>
    body {
      font-family: Arial;
      text-align: center;
      margin-top: 50px;
    }

    #status {
      font-weight: bold;
    }
  </style>
</head>

<body>

  <h2>Linnenkast</h2>

  WebSocket:
  <span id="status">Disconnected</span><br>
  Licht:
  <span id="light">?</span><br>
  Deur:
  <span id="door">?</span><br>
  Signaal sterkte:
  <span id="signal">?</span><br>

<script>

let socket;

function connectWebSocket()
{
  socket = new WebSocket("ws://" + window.location.hostname + ":81/");

  socket.onopen = function() {document.getElementById("status").innerHTML = "Connected";
    socket.send("GET_STATE");
  };

  socket.onmessage = function(event) {

    let data = JSON.parse(event.data);

    document.getElementById("light").innerHTML = data.light;
    document.getElementById("door").innerHTML =  data.door;
    document.getElementById("signal").innerHTML = data.signal + " dBm";
  };

  socket.onclose = function()
  {
    document.getElementById("status").innerHTML = "Disconnected";

    // reconnect after 2 seconds
    setTimeout(connectWebSocket, 2000);
  };
}

connectWebSocket();

</script>

</body>
</html>
)rawliteral";



// ---------- WebSocket events ----------
void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t* payload, size_t length)
{
    switch (type) {
        case WStype_CONNECTED:
            Serial.printf("WebSocket client %u connected\n", clientNum);
            break;


        case WStype_DISCONNECTED:
            Serial.printf("WebSocket client %u disconnected\n", clientNum);
            break;

        case WStype_TEXT: {
            String message = (char*)payload;
            Serial.printf("Client %u: %s\n", clientNum, message.c_str());

            if (message == "GET_STATE") {
                Serial.printf("WStype_TEXT GET_STATE\n");
            }
            break;
        }

        default:
            break;
    }
}

void WebServerInit()
{
    // HTTP server
    webServer.on("/", []() {
        webServer.send_P(200, "text/html", webpage);
    });

    webServer.begin();

    // WebSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    Serial.println("HTTP server: port 80");
    Serial.println("WebSocket: port 81");
}

void WebServerLoop()
{
    webServer.handleClient();
    webSocket.loop();
}

void WebServerPublish(int door, int light, int32_t signalStrength)
{
    JsonDocument json;
    json["door"] = door ? "Open" : "Dicht";
    json["light"] = light ? "Aan" : "Uit";
    json["signal"] = signalStrength;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    webSocket.broadcastTXT(jsonBuffer);
}
