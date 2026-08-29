
#include <Arduino.h>
#include <FastLED.h>
#include "display.h"
#include "tester.h"

#define BOARD_LED   48
#define LED_ON      HIGH
#define LED_OFF     LOW

#define NUM_LEDS 1
#define DATA_PIN 48

// Define the array of leds
static CRGB leds[NUM_LEDS];

void setup()
{
    Serial.begin(115200);
    for (int i = 0; i < 5; i++) {
        if (Serial) {
            break;
        }
        delay(950);
    }
    delay(5000);

    Serial.printf("\n\nStarting dasboard " __DATE__ ", " __TIME__ "\n");
    Serial.flush();

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    DisplayInit();

    DisplayTerminal("Hallo " __DATE__ ", " __TIME__);
    delay(1000);
    TesterInit();
}

static CRGB colors [] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Pink, CRGB::Black };

void loop()
{
    static int colorIdx = 0;
    static uint32_t lastLed = 0;

    TesterTick();

    if (millis() - lastLed > 500) {
        lastLed = millis();
        leds[0] = colors[colorIdx];
        FastLED.setBrightness(10);
        FastLED.show();

        colorIdx++;
        if (colorIdx > sizeof(colors)/ sizeof(colors[0])) {
            colorIdx = 0;
        }
    }
}
