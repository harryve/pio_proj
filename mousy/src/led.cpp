#include <Arduino.h>
#include <FastLED.h>
#include "led.h"

#define LED_DI_PIN     40
#define LED_CI_PIN     39

static CRGB leds[1];
static CRGB colors[4] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};

void LedInit()
{
    FastLED.addLeds<APA102, LED_DI_PIN, LED_CI_PIN, BGR>(leds, 1);  // BGR ordering
    FastLED.setBrightness(100);

    int i = 4;
    while (i > 0) {
        leds[0] = colors[4 - i];
        FastLED.show();
        delay(300);
        i--;
    }
}

void LedSet(bool r, bool g, bool b)
{
    leds[0] = CRGB(r ? 255 : 0, g ? 255 : 0, b ? 255 : 0);
    FastLED.show();
}
