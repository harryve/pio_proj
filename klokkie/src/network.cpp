#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include <time.h>
#include "cred.h"
#include "log.h"
#include "network.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

static int networkStatus = 0;

void NetworkInit()
{
    int timo = 20;

    networkStatus = 0;

    // Connect to Wi-Fi
    WiFi.setHostname("klokkie");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (timo-- > 0 && WiFi.status() != WL_CONNECTED) {
        delay(500);
        LOG(".");
    }
    LOG("\n");
    if (WiFi.status() != WL_CONNECTED) {
        networkStatus |= NWK_NO_WIFI;
        LOG("Cannot connect to WiFi\n");
        return;
    }
    LOG("Connected to WiFi\n");

    mqttClient.setId("klokkie");
    LOG("Attempting to connect to the MQTT broker: ");
    if (!mqttClient.connect(MQTT_BROKER, 1883)) {
        LOG("MQTT connection failed! Error code = %d\n", mqttClient.connectError());
        networkStatus |= NWK_NO_MQTT;
    }
    else {
        LOG("Connected to the MQTT broker!\n");
    }
}

void NetworkTick()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            LOG("Reconnecting to WiFi...\n");
            networkStatus |= (NWK_NO_WIFI | NWK_NO_MQTT);
            mqttClient.stop();
            WiFi.disconnect();
            WiFi.reconnect();
        }
        else {
            networkStatus &= ~NWK_NO_WIFI;

            if (mqttClient.connected() == 0) {
                networkStatus |= NWK_NO_MQTT;
                if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
                    LOG("MQTT connection failed! Error code = %d\n", mqttClient.connectError());
                }
                else {
                    LOG("Reconnected to the MQTT broker!\n");
                }
            }
            else {
                networkStatus &= ~NWK_NO_MQTT;
            }
        }
    }
    mqttClient.poll();
}

void PublishState(bool present, bool timeSynced)
{
    JsonDocument json;
    json["Present"] = present; // ? 1 : 0;
    json["TimeSynced"] = timeSynced; // ? 1 : 0;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    mqttClient.beginMessage("klokkie/state");
    mqttClient.print(jsonBuffer);
    mqttClient.endMessage();
}

void PublishSensor(float temperature, float humidity, float pressure)
{
    JsonDocument json;
    json["Temperature"] = temperature;
    json["Humidity"] = humidity;
    json["Pressure"] = pressure;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    mqttClient.beginMessage("klokkie/sensor");
    mqttClient.print(jsonBuffer);
    mqttClient.endMessage();
}

int NetworkGetStatus()
{
    return networkStatus;
}
