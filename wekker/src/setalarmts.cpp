#include <Arduino.h>
#include "network.h"
#include "mode.h"
#include "settings.h"
#include "setalarmts.h"

#include "display.h"

#define TIMEOUT (10 * 1000)       // ms

SetAlarmTs::SetAlarmTs()
{
    drawRequest = false;
    wakeupTime = 0;
}

void SetAlarmTs::Start()
{
    drawRequest = true;
    startTime = millis();
}

boolean SetAlarmTs::Tick()
{
    if (millis() - startTime > TIMEOUT) {
        return false;
    }

    int newTime = SettingsGetWakeupTime();
    if (wakeupTime != newTime) {
        wakeupTime = newTime;
    }
    else {
        if (!drawRequest) {
            return true;
        }
    }
    drawRequest = false;
    FastLED.clear();

    int x = 3;
    CRGB c = CRGB::Green;

    int hours = wakeupTime / 60;
    int minutes = wakeupTime % 60;

    if (hours / 10 != 0) {
        DrawDigit(x + 0, 0, hours / 10, c);
    }
    DrawDigit(x + 7, 0, hours % 10, c);
    SetLed(x + 13, 2, c);
    SetLed(x + 13, 5, c);
    DrawDigit(x + 15, 0, minutes / 10, c);
    DrawDigit(x + 22, 0, minutes % 10, c);

    FastLED.show();
    return true;
}

int SetAlarmTs::ButtonHandler(Button::Id id, Button::Event event)
{
    Start();        // Restart time-out
    return MODE_CLOCK;
}
