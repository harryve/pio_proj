#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

#include "cred.h"
#include "network.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

#define HOSTNAME    "wemos4"
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

        Serial.print(currentMillis);
        Serial.println(" Check network");
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

//tele/wemos1/SENSOR {"Time":"2025-05-09T11:41:50","BME280":{"Temperature":21.4,"Humidity":33.7,"Pressure":1021.9},"BH1750":{"Illuminance":0},"MHZ19B":{"Model":"B","CarbonDioxide":424,"Temperature":19.0},"PressureUnit":"hPa","TempUnit":"C"}
// "Signal":-54
void NetworkPublish(float temperature, float humidity, float pressure, uint32_t signalStrength)
{
    JsonDocument json;
    json["BME280"]["Temperature"] = temperature;
    json["BME280"]["Humidity"] = humidity;
    json["BME280"]["Pressure"] = pressure;
    char jsonBuffer[128];
    serializeJson(json, jsonBuffer);
    unsigned long currentMillis = millis();
    Serial.print(currentMillis);
    Serial.println(jsonBuffer);

    if (mqttClient.connected() == 0) {
        Serial.print(currentMillis);
        Serial.println("Connection with broker lost");
    }
    else {
        Serial.print(currentMillis);
        Serial.println("Send message to broker");
        mqttClient.beginMessage(TOPIC);
        mqttClient.print(jsonBuffer);
        mqttClient.endMessage();
    }
}
