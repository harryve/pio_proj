#include <Arduino.h>

#include "network.h"
#include "sensor.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStart wemos2");

    NetworkInit();
    SensorInit();

    delay(1000);
    Serial.println("Setup complete");
    SensorTemperature();        // First measurement after power up fails
}

void loop()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    NetworkLoop();

    // Update values every 5 seconds
    if (currentMillis - previousMillis >= 1000 * 300) {
        previousMillis = currentMillis;
        float temperature = SensorTemperature();
        if (temperature < 50.0) {
            NetworkPublish(temperature, 0);
        }
    }
}
