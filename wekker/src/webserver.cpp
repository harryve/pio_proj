#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "settings.h"
#include "webserver.h"

#define HTTP_PORT 80

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

static void ListDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                ListDir(fs, file.name(), levels - 1);
            }
        }
        else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

static void ReadFile(fs::FS &fs, const char * path)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while (file.available()){
        Serial.write(file.read());
    }
    file.close();
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

String processor(const String &var)
{
    Serial.println(var.c_str());
    if (var == "STATE") {
        return String(SettingsGetAlarmActive() ? "on" : "off");
    }

    if (var == "WAKEUPTIME") {
        return String("06:01");
    }

    if (var == "UPTIME") {
        return String("1:23");
    }

    if (var == "REBOOTCOUNT") {
        return String("1962");
    }
    return String("???");
}

void onRootRequest(AsyncWebServerRequest *request)
{
  Serial.printf("onRootRequest\n");
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients()
{
    Serial.printf("notifyClients\n");
    //const uint8_t size = JSON_OBJECT_SIZE(1);
    //StaticJsonDocument<size> json;
    JsonDocument json;
    json["status"] = SettingsGetAlarmActive() ? "on" : "off";

    char buffer[17];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    Serial.printf("handleWebSocketMessage\n");

    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        //const uint8_t size = JSON_OBJECT_SIZE(1);
        //StaticJsonDocument<size> json;
        JsonDocument json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        const char *action = json["action"];
        Serial.println(action);
        if (strcmp(action, "toggle") == 0) {
            SettingsToggleAlarmActive();
            //notifyClients();
        }
        if (strcmp(action, "submit") == 0) {
            SettingsToggleAlarmActive();
            //notifyClients();
        }
    }
}

static void OnEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len)
{

    Serial.printf("WebSocket onEvent\n");
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}




void WebserverInit()
{
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    ListDir(SPIFFS, "/", 0);
    ReadFile(SPIFFS, "/hallo.txt");

    // Init WebSocket
    ws.onEvent(OnEvent);
    server.addHandler(&ws);

    // Init webserver
    Serial.printf("initWebServer\n");
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();

    SettingsSetChangeCb(notifyClients);

}

void WebserverTick()
{
    ws.cleanupClients();
}