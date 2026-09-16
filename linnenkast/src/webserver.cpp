#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

// ---------- Servers ----------
static ESP8266WebServer webServer(80);
static WebSocketsServer webSocket(81);

static bool ledOn;

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

    button {
      font-size: 24px;
      padding: 15px 30px;
      margin: 10px;
    }

    #status {
      font-weight: bold;
    }
  </style>
</head>

<body>

  <h2>Linnenkast</h2>

  WebSocket:
  <span id="status">Disconnected</span>

  <h3 id="led">LED: ?</h3>

  <button onclick="sendCommand('ON')">ON</button>
  <button onclick="sendCommand('OFF')">OFF</button>

<script>

let socket;

function connectWebSocket()
{
  socket = new WebSocket("ws://" + window.location.hostname + ":81/");

  socket.onopen = function() {document.getElementById("status").innerHTML = "Connected";
    socket.send("GET_STATE");
  };

  socket.onmessage = function(event)
  {
    console.log("Received:", event.data);
    if (event.data === "LED_ON") {
      document.getElementById("led").innerHTML = "LED: ON";
    }

    if (event.data === "LED_OFF") {
      document.getElementById("led").innerHTML = "LED: OFF";
    }
  };

  socket.onclose = function()
  {
    document.getElementById("status").innerHTML = "Disconnected";

    // reconnect after 2 seconds
    setTimeout(connectWebSocket, 2000);
  };
}

function sendCommand(command)
{
  if (socket.readyState === WebSocket.OPEN) {
    socket.send(command);
  }
}

connectWebSocket();

</script>

</body>
</html>
)rawliteral";


// ---------- Send LED state ----------
void sendLedState()
{
    if (ledOn) {
        webSocket.broadcastTXT("LED_ON");
    }
    else {
        webSocket.broadcastTXT("LED_OFF");
    }
}


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

            if (message == "ON") {
                Serial.printf("WStype_TEXT ON\n");
                ledOn = true;
                //digitalWrite(LED_PIN, LOW);
                sendLedState();
            }
            else if (message == "OFF") {
                Serial.printf("WStype_TEXT OFF\n");
                ledOn = false;
                //digitalWrite(LED_PIN, HIGH);
                sendLedState();
            }
            else if (message == "GET_STATE") {
                Serial.printf("WStype_TEXT GET_STATE\n");
                if (ledOn) {
                    webSocket.sendTXT(clientNum, "LED_ON");
                }
                else {
                    webSocket.sendTXT(clientNum, "LED_OFF");
                }
            }
            break;
        }

        default:
            break;
    }
}


// ---------- Setup ----------
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


// ---------- Main loop ----------
void WebServerLoop()
{
    webServer.handleClient();
    webSocket.loop();
}


#if 0
===v=v=v=v
String message =
  "{\"temperature\":23.4,\"humidity\":51.2}";

webSocket.broadcastTXT(message);

Xxxxxxxxxxxxxxxxxxxxxx
socket.onmessage = function(event) {

  let data = JSON.parse(event.data);

  document.getElementById("temp").innerHTML =
    data.temperature + " °C";

  document.getElementById("humidity").innerHTML =
    data.humidity + " %";
};
#endif











#if 0
#include <ESP8266WebServer.h>   // Include the WebServer library

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

static int doorState, lightState;
static int32_t signalStrength;

void WebServerPublish(int door, int light, int32_t sigStrength)
{
    doorState = door;
    lightState = light;
    signalStrength = sigStrength;
}

static void handleRoot()
{
    String content = "<!DOCTYPE html><html>";
    content += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    content += "<link rel=\"icon\" href=\"data:,\">";

    content += "<body><h1>Linnenkast Web Server</h1>";
    content += "<p>Deur: ";
    if (doorState == 1) {
        content += "open";
    }
    else {
        content += "dicht";
    }
    content += "</p>";

    content += "<p>Verlichting: ";
    if (lightState == 1) {
        content += "aan";
    }
    else {
        content += "uit";
    }
    content += "</p>";

    content += "<p>Signaal sterkte: ";
    content += signalStrength;
    content += "</p>";

    content += "<p>Uptime: ";
    content += millis() / 1000;
    content += "</p>";

    content += "</body></html>";

    server.send(200, "text/html", content);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found");
}

void WebServerInit()
{
    server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
    server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin();                           // Actually start the server
    Serial.println("HTTP server started");
}

void WebServerLoop()
{
    server.handleClient();                    // Listen for HTTP requests from clients
}
#endif