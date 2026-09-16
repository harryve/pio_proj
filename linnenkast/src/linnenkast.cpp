#include <Arduino.h>

#include "light.h"
#include "webserver.h"
#include "network.h"

#define LED_ON_BOARD D4
#define LED_OFF HIGH
#define LED_ON  LOW

static Light light;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nStart linnenkast verlichting " __DATE__ ", " __TIME__);

    pinMode(LED_ON_BOARD, OUTPUT);
    digitalWrite(LED_ON_BOARD, LED_ON);
    light.Setup();

    NetworkInit();
    WebServerInit();

    Serial.println("Setup complete");
    digitalWrite(LED_ON_BOARD, LED_OFF);
}

void loop()
{
    static unsigned long previousMillis = 0xf0000000;
    unsigned long currentMillis = millis();

    light.Tick();
    NetworkLoop();
    WebServerLoop();

    // Update values every 5 minutes or when changed
    if (light.Changed() ||
        (currentMillis - previousMillis >= 1000 * 6)) { //0 * 5) {
        previousMillis = currentMillis;

        digitalWrite(LED_ON_BOARD, light.On() ? LED_ON : LED_OFF);

        NetworkPublish(light.Door(), light.On(), NetworkSignalStrength());
    }
}
