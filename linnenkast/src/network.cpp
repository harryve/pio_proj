#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

#include "cred.h"
#include "network.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

#define HOSTNAME    "linnenkast"
#define TOPIC "tele/" HOSTNAME "/SENSOR"

void NetworkInit()
{
    Serial.println("Start wifi for " HOSTNAME);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(HOSTNAME);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("Connected to wifi network");

    mqttClient.setId(HOSTNAME);
    Serial.print("Attempting to connect to the MQTT broker: ");
    if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
    }
    else {
        Serial.println("Connected to the MQTT broker");
    }
}

void NetworkLoop()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        Serial.println("Check network");
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to WiFi...");
            mqttClient.stop();
            WiFi.disconnect();
            WiFi.reconnect();
        }
        else {
            if (mqttClient.connected() == 0) {
                if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
                    Serial.print("MQTT connection failed! Error code = ");
                    Serial.println(mqttClient.connectError());
                }
            }
        }
    }
    mqttClient.poll();
}

int32_t NetworkSignalStrength()
{
    return WiFi.RSSI();
}

void NetworkPublish(int door, int light, int32_t signalStrength)
{
    JsonDocument json;
    json["door"] = door;
    json["light"] = light;
    json["signal"] = signalStrength;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    if (mqttClient.connected() == 0) {
        Serial.println("Connection with broker lost");
    }
    else {
        Serial.print("Send message to broker ");
        Serial.println(jsonBuffer);
        mqttClient.beginMessage(TOPIC);
        mqttClient.print(jsonBuffer);
        mqttClient.endMessage();
    }
}
