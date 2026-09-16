#include <Arduino.h>
#include "light.h"

#define REED_RELAY_IN   D5
#define DOOR_CLOSED     LOW
#define DOOR_OPEN       HIGH
#define LIGHT_SWITCH    D7
#define LIGHT_ON        LOW
#define LIGHT_OFF       HIGH
#define MAX_ON_TIME     10000

// #define LED_ON_BOARD D4
// #define LED_OFF HIGH
// #define LED_ON  LOW

Light::Light()
{
    lastDoorState = DOOR_CLOSED;
    doorState = DOOR_CLOSED;
    lightState = LIGHT_OFF;
    changed = true;
}

void Light::Setup()
{
    pinMode(REED_RELAY_IN, INPUT_PULLUP);
    pinMode(LIGHT_SWITCH, OUTPUT);
    digitalWrite(LIGHT_SWITCH, LIGHT_OFF);
}

void Light::Tick()
{

    static u_int32_t lastDebounceTime;
    static u_int32_t lightOnTime;

    int currentState = digitalRead(REED_RELAY_IN);

    if (currentState != lastDoorState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > 100) {
        if (currentState != doorState) {
            doorState = currentState;
            if (doorState == DOOR_CLOSED) {
                Serial.println("Reed contact closed (door closed)");
                lightState = LIGHT_OFF;
                digitalWrite(LIGHT_SWITCH, lightState);
                //digitalWrite(LED_ON_BOARD, LED_OFF);
            }
            else {
                Serial.println("Reed contact open (door open)");
                lightState = LIGHT_ON;
                digitalWrite(LIGHT_SWITCH, lightState);
                lightOnTime = millis();
                //digitalWrite(LED_ON_BOARD, LED_ON);
            }
            changed = true;
        }
    }
    lastDoorState = currentState;

    if (lightState == LIGHT_ON) {
        if ((millis() - lightOnTime) > MAX_ON_TIME) {
            Serial.println("Time-out switch off light");
            lightState = LIGHT_OFF;
            digitalWrite(LIGHT_SWITCH, lightState);
            changed = true;
            //digitalWrite(LED_ON_BOARD, LED_OFF);
        }
    }
}

bool Light::Changed()
{
    if (changed) {
        changed = false;
        return true;
    }
    return false;
}

bool Light::Door()
{
    return doorState == DOOR_OPEN;
}

bool Light::On()
{
    return lightState == LIGHT_ON;
}
