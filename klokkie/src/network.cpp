#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>

#include <time.h>
#include "cred.h"
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
        Serial.print(".");
    }
    Serial.println("\n");
    if (WiFi.status() != WL_CONNECTED) {
        networkStatus |= NWK_NO_WIFI;
        Serial.println("Cannot connect to WiFi");
        return;
    }
    Serial.println("Connected to WiFi");

    mqttClient.setId("klokkie");
    Serial.print("Attempting to connect to the MQTT broker: ");
    if (!mqttClient.connect(MQTT_BROKER, 1883)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
        networkStatus |= NWK_NO_MQTT;
    }
    else {
        Serial.println("Connected to the MQTT broker!");
        Serial.println();
    }

    // Init and get the time

//    configTime(0, 0, TIME_SERVER_ADDR);
  //  setenv("TZ", myTimezone, 1);
    //tzset();
}

void NetworkTick()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to WiFi...");
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
                    Serial.print("MQTT connection failed! Error code = ");
                    Serial.println(mqttClient.connectError());
                }
                else {
                    Serial.println("Reconnected to the MQTT broker!");
                }
            }
            else {
                networkStatus &= ~NWK_NO_MQTT;
            }
        }
    }
    mqttClient.poll();
}

// static void Publish(const char *pTopic, int val)
// {
//     char buf[16];

//     mqttClient.beginMessage(pTopic);
//     buf[snprintf(buf, sizeof(buf) - 1, "%d", val)] = '\0';
//     mqttClient.print(buf);
//     mqttClient.endMessage();
// }

void NetworkPublishLdr(int val)
{
//    Publish("tele/wekker/sensor", val);
}

void NetworkPublishBrightness(int val)
{
//    Publish("tele/wekker/brightness", val);
}

int NetworkGetStatus()
{
    return networkStatus;
}
