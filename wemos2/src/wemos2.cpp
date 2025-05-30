#include <Arduino.h>

#include "network.h"
#include "sensor.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStart wemos2 gevalletje");

    NetworkInit();
    SensorInit();

    delay(1000);
    Serial.println("Setup complete");
}


void loop()
{
    static unsigned long previousMillis;
    unsigned long currentMillis = millis();

    NetworkLoop();

    // Update values every 5 seconds
    if (currentMillis - previousMillis >= 1000 * 30) {
        previousMillis = currentMillis;
        NetworkPublish(SensorTemperature(), 0);
    }
}
