#include <Arduino.h>

#include "webserver.h"
#include "network.h"
#include "sensor.h"
#define LED_BUILTINN D4
#define LED_OFF HIGH
#define LED_ON  LOW

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStart werkkamer sensor");

    pinMode(LED_BUILTINN, OUTPUT);
    digitalWrite(LED_BUILTINN, LED_ON);

    NetworkInit();
    WebServerInit();
    SensorInit();

    delay(1000);
    Serial.println("Setup complete");
    digitalWrite(LED_BUILTINN, LED_OFF);
}


void loop()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    NetworkLoop();
    WebServerLoop();

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