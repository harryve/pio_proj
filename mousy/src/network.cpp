#include <Arduino.h>
#include <WiFi.h>

#include <time.h>
#include "cred.h"
#include "display.h"
#include "network.h"

static WiFiClient wifiClient;
static bool connected;

void NetworkInit()
{
    int timo = 20;

    // Connect to Wi-Fi
    WiFi.setHostname("mousy");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    DisplayTerminal("Connecting to WiFi");
    while (timo-- > 0 && WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    if (WiFi.status() != WL_CONNECTED) {
        connected = false;
        DisplayTerminal("Cannot connect to WiFi");
        return;
    }
    connected = true;
    DisplayTerminal("Connected to WiFi");
}

void NetworkTick()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 60000) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect();
            WiFi.reconnect();
            connected = (WiFi.status() == WL_CONNECTED);
        }
    }
}
