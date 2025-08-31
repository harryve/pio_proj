#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <esp_sntp.h>

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
    WiFi.setHostname("wekker");
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

    mqttClient.setId("wekker");
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
            Serial.println("Reconnecting to WiFi...");
            networkErrors |= (NWK_NO_WIFI | NWK_NO_MQTT);
            mqttClient.stop();
            WiFi.disconnect();
            WiFi.reconnect();
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

int NetworkGetErrors()
{
    unsigned long interval = sntp_get_sync_interval();

    if (interval > 3600*1000) {
        sntp_set_sync_interval(3600*1000);
        if (!sntp_restart()) {
            Serial.println("SNTP restart failed");
        }
        else {
            Serial.println("SNTP restarted");
        }
    }
    unsigned int reachability = sntp_getreachability(0);

    if ((reachability & 0xf) == 0) {
        networkErrors |= NWK_NO_NTP;
    }
    else {
        networkErrors &= ~NWK_NO_NTP;
    }

    return networkErrors;
}

static void Publish(const char *pTopic, int val)
{
    char buf[16];

    mqttClient.beginMessage(pTopic);
    buf[snprintf(buf, sizeof(buf) - 1, "%d", val)] = '\0';
    mqttClient.print(buf);
    mqttClient.endMessage();

}

void NetworkPublishLdr(int val)
{
    Publish("tele/wekker/sensor", val);
}

void NetworkPublishBrightness(int val)
{
    Publish("tele/wekker/brightness", val);
}
