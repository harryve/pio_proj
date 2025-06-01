#include <Arduino.h>

#include "webserver.h"
#include "network.h"
#include "sensor.h"
#define LED_BUILTINN D4

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStart werkkamer sensor");

    pinMode(LED_BUILTINN, OUTPUT);

    NetworkInit();
    WebServerInit();
    SensorInit();

    delay(1000);
    Serial.println("Setup complete");
}


void loop()
{
    static unsigned long previousMillis;
    static unsigned long ledMillis;
    static uint8_t ledState = LOW;
    unsigned long currentMillis = millis();

    NetworkLoop();
    WebServerLoop();

    if (currentMillis - ledMillis >= 1000) {
        ledMillis = currentMillis;

        ledState = ledState == LOW ? HIGH : LOW;
        digitalWrite(LED_BUILTINN, ledState);
    }

    // Update values every 5 minutes
    if (currentMillis - previousMillis >= 1000 * 60 * 5) {
        previousMillis = currentMillis;

        float temperature = SensorTemperature();
        float humidity = SensorHumidity();
        float pressure= SensorPressure();
        int32_t signalStrength = NetworkSignalStrength();

        NetworkPublish(temperature, humidity, pressure, signalStrength);
        WebServerPublish(temperature, humidity, pressure, signalStrength);
    }
}