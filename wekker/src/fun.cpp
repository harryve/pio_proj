#include <Arduino.h>
#include "network.h"
#include "mode.h"
#include "settings.h"
#include "fun.h"
#include "hwdefs.h"

#define TIMEOUT (300 * 1000)       // ms
#define CYCLETIME 20
#define N_PATTERNS 4

Fun::Fun()
{
    patternNumber = 0;
}

void Fun::Start()
{
    startTime = millis();
    cycleTime = startTime;
    cycle = 0;
    hue = 0;
}

boolean Fun::Tick()
{
    if (millis() - startTime > TIMEOUT) {
        return false;
    }

    if (millis() - cycleTime < CYCLETIME) {
        return true;
    }
    cycleTime = millis();
    cycle++;
    hue++;

    switch (patternNumber) {
        case 0:
            Rainbow(hue, 7);
            break;
        case 1:
            Rainbow(hue, 7);
            if (random8() < 80) {
                /*NUM_LEDS)*/
                int pos = random16(NUM_LEDS);
                SetLed(pos, GetLed(pos) + CRGB::White);
            }
            break;
        case 2:
            Confetti();
            break;
        case 3:
            Sinelon();
            break;
        default:
            Fill(CRGB::Blue);
            break;
    }
    FastLED.show();

    // if (millis() - cycleTime > CYCLETIME) {
    //     cycleTime = millis();
    //     switch(cycle) {
    //         case 0: Fill(CRGB::Blue);   break;
    //         case 1: Fill(CRGB::Yellow);   break;
    //         case 2: Fill(CRGB::Red);   break;
    //         case 3: Fill(CRGB::Green);   break;
    //         case 4: Fill(CRGB::Purple);   break;
    //         case 5: Fill(CRGB::Pink);   break;
    //         default: FastLED.clear();   break;
    //     }
    //     cycle++;
    //     if (cycle > 6) {
    //         cycle = 0;
    //     }
    //     FastLED.show();
    // }
    return true;
}

void Fun::Confetti()
{
  // random colored speckles that blink in and fade smoothly
  Fade(10);
  int pos = random16(NUM_LEDS);
  /*GetLed(pos) + */
  SetLed(pos, CHSV(hue + random8(64), 200, 255));
}

void Fun::Sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  Fade(20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  SetLed(pos, /*GetLed(pos) +*/ CHSV(hue, 255, 192));
}

int Fun::ButtonHandler(Button::Id id, Button::Event event)
{
    Start();        // Restart time-out
    if (id == Button::Id::LEFT && event ==Button::Event::SHORT_PRESS) {
        patternNumber += 1;
        if (patternNumber >= N_PATTERNS) {
            patternNumber = 0;
        }
        return MODE_FUN;
    }
    return MODE_CLOCK;
}
