#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include <time.h>
#include "cred.h"
#include "network.h"

static const char topic1[]  = "tele/lorasensor/sensor";
static const char topic2[]  = "tele/lorasensor2/sensor";

static WiFiClient wifiClient;
static MqttClient mqttClient(wifiClient);

void NetworkInit()
{
    int timo = 20;

    // Connect to Wi-Fi
    WiFi.setHostname("LoRaBridge");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (timo-- > 0 && WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n");
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Cannot connect to WiFi");
        return;
    }
    Serial.println("Connected to WiFi");

    mqttClient.setId("LoRaBridge");
    Serial.print("Attempting to connect to the MQTT broker: ");
    if (!mqttClient.connect(MQTT_BROKER, 1883)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
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

            if (mqttClient.connected() == 0) {
                if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
                    Serial.print("MQTT connection failed! Error code = ");
                    Serial.println(mqttClient.connectError());
                }
                else {
                    Serial.println("Reconnected to the MQTT broker!");
                }
            }
        }
    }
    mqttClient.poll();
}

void NetworkPublish(int sensor, uint16_t seq, int16_t temperature, int8_t humidity, int16_t vbat,
    int rssi, float snr, uint16_t runtime,  uint16_t illuminance)
{
    char tempBuf[16];
    char vbatBuf[16];
    char snrBuf[16];

    snprintf(tempBuf, sizeof(tempBuf), "%.1f", temperature / 10.0);
    snprintf(vbatBuf, sizeof(vbatBuf), "%.3f", vbat / 1000.0);
    snprintf(snrBuf, sizeof(snrBuf), "%.1f", snr);

    JsonDocument doc;
    doc["counter"] = seq;
    int frag = temperature % 10;
    doc["temp"] = tempBuf;
    doc["hum"] = humidity;
    doc["vbat"] = vbatBuf;
    doc["rssi"] = rssi;
    doc["snr"] = snrBuf;
    doc["runtime"] = runtime;
    doc["illuminance"] = illuminance;
    String msg;
    serializeJson(doc, msg);

    mqttClient.beginMessage(sensor == 1 ? topic1 : topic2);
    mqttClient.print(msg.c_str());
    mqttClient.endMessage();
}
