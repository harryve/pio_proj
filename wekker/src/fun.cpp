#include <Arduino.h>
#include "network.h"
#include "mode.h"
#include "settings.h"
#include "fun.h"

#include "display.h"

#define TIMEOUT (30 * 1000)       // ms
#define CYCLETIME 150

Fun::Fun()
{
    drawRequest = false;
}

void Fun::Start()
{
    startTime = millis();
    cycleTime = startTime;
    cycle = 0;
}

boolean Fun::Tick()
{
    if (millis() - startTime > TIMEOUT) {
        return false;
    }

    if (millis() - cycleTime > CYCLETIME) {
        cycleTime = millis();
        switch(cycle) {
            case 0: Fill(CRGB::Blue);   break;
            case 1: Fill(CRGB::Yellow);   break;
            case 2: Fill(CRGB::Red);   break;
            case 3: Fill(CRGB::Green);   break;
            case 4: Fill(CRGB::Purple);   break;
            case 5: Fill(CRGB::Pink);   break;
            default: FastLED.clear();   break;
        }
        cycle++;
        if (cycle > 6) {
            cycle = 0;
        }
        FastLED.show();
    }
    return true;
}

int Fun::ButtonHandler(Button::Id id, Button::Event event)
{
    Start();        // Restart time-out
    return MODE_CLOCK;
}
