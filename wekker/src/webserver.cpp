#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "settings.h"
#include "webserver.h"

#define HTTP_PORT 80

static Button::EventCb buttonCallback = NULL;
static AsyncWebServer server(HTTP_PORT);
static AsyncWebSocket ws("/ws");

static const char *GetAlarmToggleStr()
{
    // Text on button
    if (SettingsGetAlarmActive()) {
        return "Uit";
    }
    return "Aan";
}

static const char *GetAlarmActiveStr()
{
    if (SettingsGetAlarmActive()) {
        return "on";
    }
    return "off";
}

static char *GetWakupStr()
{
    static char wakeupTimeBuf[16];
    snprintf(wakeupTimeBuf, sizeof(wakeupTimeBuf), "%02d:%02d", SettingsGetWakeupTime() / 60, SettingsGetWakeupTime() % 60);
    return wakeupTimeBuf;
}

static char *GetTimeOfDayStr()
{
    static char timeBuf[16];
    uint32_t time = SettingsGetTimeOfDay();
    uint32_t hours, mins;

    hours = time / 60;
    time -= hours * 60;
    mins = time;

    snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", hours, mins);

    return timeBuf;
}

static char *GetUpTimeStr()
{
    static char upTimeBuf[32];
    uint32_t uptime = SettingsGetUptime();
    uint32_t days, hours, mins;

    days = uptime / (24 * 60);
    uptime -= days * 24 * 60;
    hours = uptime / 60;
    uptime -= hours * 60;
    mins = uptime;

    int l = 0;
    if (days > 0) {
        l = snprintf(upTimeBuf, sizeof(upTimeBuf), "%d days ", days);
    }
    snprintf(&upTimeBuf[l], sizeof(upTimeBuf) - l, "%d:%02d", hours, mins);

    return upTimeBuf;
}

static char *GetRebootStr()
{
    static char rebootCountBuf[16];
    snprintf(rebootCountBuf, sizeof(rebootCountBuf), "%d", SettingsGetRebootCounter());
    return rebootCountBuf;
}

String processor(const String &var)
{
    Serial.println(var.c_str());
    if (var == "TOGGLE") {
        return String(GetAlarmToggleStr());
    }
    if (var == "STATE") {
        return String(GetAlarmActiveStr());
    }

    if (var == "WAKEUPTIME") {
        return String(GetWakupStr());
    }

    if (var == "UPTIME") {
        return String(GetUpTimeStr());
    }

    if (var == "TIMEOFDAY") {
        return String(GetTimeOfDayStr());
    }

    if (var == "REBOOTCOUNT") {
        return String(GetRebootStr());
    }
    return String("???");
}

void onRootRequest(AsyncWebServerRequest *request)
{
  Serial.printf("onRootRequest\n");
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void notifyClients()
{
    if (ws.count() == 0) {
        return;
    }

    Serial.printf("notifyClients\n");
    JsonDocument json;
    json["toggle"] = GetAlarmToggleStr();
    json["status"] = GetAlarmActiveStr();
    json["wakeuptime"] = GetWakupStr();
    json["timeofday"] =  GetTimeOfDayStr();
    json["uptime"] =  GetUpTimeStr();
    json["reboot_count"] = GetRebootStr();
    char jsonBuffer[128];
    size_t len = serializeJson(json, jsonBuffer);
    ws.textAll(jsonBuffer, len);
}

static void SimButtonPress(Button::Id id)
{
    if (buttonCallback != NULL) {
        buttonCallback(id, Button::Event::SHORT_PRESS);
    }
}

static void HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        JsonDocument json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        if (json["action"].is<JsonVariant>()) {
            const char *action = json["action"];
            Serial.printf("Action = %s\n", action);
            if (strcmp(action, "toggle") == 0) {
                SettingsToggleAlarmActive();
            }
            else if (strcmp(action, "left") == 0) {
                SimButtonPress(Button::Id::LEFT);
            }
            else if (strcmp(action, "select") == 0) {
                SimButtonPress(Button::Id::MID);
            }
            else if (strcmp(action, "right") == 0) {
                SimButtonPress(Button::Id::RIGHT);
            }
        }

        if (json["submit"].is<JsonVariant>()) {
            const char *wakeupTime = json["submit"];
            if (strlen(wakeupTime) == 5) {
                int hour, min;
                if (sscanf(wakeupTime, "%d:%d", &hour, &min) == 2) {
                    SettingsSetWakeupTime((uint16_t)(hour * 60 + min));
                }
            }
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
            HandleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void WebserverInit(Button::EventCb eventCb)
{

    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // Init WebSocket
    ws.onEvent(OnEvent);
    server.addHandler(&ws);

    // Init webserver
    Serial.printf("initWebServer\n");
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();

    SettingsSetChangeCb(notifyClients);
    buttonCallback = eventCb;
}

void WebserverTick()
{
    ws.cleanupClients();
}
