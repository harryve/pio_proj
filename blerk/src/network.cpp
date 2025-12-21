#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include <time.h>
#include "cred.h"
#include "network.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

static int networkErrors = 0;

void NetworkInit()
{
    int timo = 20;

    networkErrors = 0;

    // Connect to Wi-Fi
    WiFi.setHostname("blerk");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (timo-- > 0 && WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n");
    if (WiFi.status() != WL_CONNECTED) {
        networkErrors |= NWK_NO_WIFI;
        Serial.println("Cannot connect to WiFi");
        return;
    }
    Serial.println("Connected to WiFi");

    mqttClient.setId("blerk");
    Serial.print("Attempting to connect to the MQTT broker: ");
    if (!mqttClient.connect(MQTT_BROKER, 1883)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
        networkErrors |= NWK_NO_MQTT;
    }
    else {
        Serial.println("Connected to the MQTT broker!");
        Serial.println();
    }
}

void NetworkTick()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            Serial.print("Reconnecting to WiFi...");
            networkErrors |= (NWK_NO_WIFI | NWK_NO_MQTT);
            mqttClient.stop();
            WiFi.disconnect();
            if (WiFi.reconnect()) {
                Serial.println("failed");
            }
            else {
                Serial.println("Ok!");
            }
        }
        else {
            networkErrors &= ~NWK_NO_WIFI;

            if (mqttClient.connected() == 0) {
                networkErrors |= NWK_NO_MQTT;
                if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
                    Serial.print("MQTT connection failed! Error code = ");
                    Serial.println(mqttClient.connectError());
                }
                else {
                    Serial.println("Reconnected to the MQTT broker!");
                }
            }
            else {
                networkErrors &= ~NWK_NO_MQTT;
            }
        }
    }
    mqttClient.poll();
}

void NetworkPublishPressed(int val)
{
    char buf[16];

    mqttClient.beginMessage("blerk/button");
    buf[snprintf(buf, sizeof(buf) - 1, "%d", val)] = '\0';
    mqttClient.print(buf);
    mqttClient.endMessage();
}

void NetworkPublishBadkamer(float temperature, float humidity, float pressure, float vbat, uint32_t runTime, uint32_t seqNr)
{
    JsonDocument json;
    json["Temperature"] = temperature;
    json["Humidity"] = humidity;
    json["Pressure"] = pressure;
    json["Vbat"] = vbat;
    json["Runtime"] = runTime;
    json["Seqnr"] = seqNr;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);

    unsigned long currentMillis = millis();
    //Serial.print(currentMillis);
    //Serial.println(jsonBuffer);

    mqttClient.beginMessage("blerk/sensor1");
    mqttClient.print(jsonBuffer);
    mqttClient.endMessage();
}

