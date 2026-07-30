
#include <Arduino.h>
#include <FastLED.h>
#include "display.h"

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

    pinMode(BOARD_LED, OUTPUT);
    for (int i = 0; i < 5; i++) {
        if (Serial) {
            break;
        }
        digitalWrite(BOARD_LED, LED_ON);
        delay(50);
        digitalWrite(BOARD_LED, LED_OFF);
        delay(950);
    }
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    delay(250);
    Serial.printf("Starting dasboard " __DATE__ ", " __TIME__ "\n");
    delay(950);
    DisplayInit();
}

static CRGB colors [] = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Pink, CRGB::Black };

void loop()
{
    // digitalWrite(BOARD_LED, LED_ON);
    // delay(500);
    // digitalWrite(BOARD_LED, LED_OFF);
    // delay(500);

    static int colorIdx = 0;
    leds[0] = colors[colorIdx];
    FastLED.setBrightness(10);
    FastLED.show();
    delay(500);
    colorIdx++;
    Serial.printf("Color=%d\n", colorIdx);
    if (colorIdx > sizeof(colors)/ sizeof(colors[0])) {
        colorIdx = 0;
    }
}
