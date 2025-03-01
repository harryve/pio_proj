#include <Arduino.h>
#include "network.h"
#include "mode.h"
#include "settings.h"
#include "fun.h"
#include "hwdefs.h"

#define TIMEOUT (300 * 1000)       // ms
//#define CYCLETIME 20
#define N_PATTERNS 6

Fun::Fun()
{
    patternNumber = 0;
}

void Fun::Start()
{
    startTime = millis();
    cycleTime = startTime;
    cycleTimeout = 20;
    hue = 0;
    SetMinimumBrightness(50);
    SetBrightness(50);
}

boolean Fun::Tick()
{
    if (millis() - startTime > TIMEOUT) {
        return false;
    }

    if (millis() - cycleTime < cycleTimeout) {
        return true;
    }
    cycleTime = millis();
    hue++;

    switch (patternNumber) {
        case 0:
            cycleTimeout = 20;
            Rainbow(hue, 7);
            break;
        case 1:
            cycleTimeout = 20;
            Rainbow(hue, 7);
            if (random8() < 80) {
                /*NUM_LEDS)*/
                int pos = random16(NUM_LEDS);
                SetLed(pos, GetLed(pos) + CRGB::White);
            }
            break;
        case 2:
            cycleTimeout = 20;
            Confetti();
            break;
        case 3:
            cycleTimeout = 20;
            Sinelon();
            break;
        case 4:
            cycleTimeout = 10;
            Cylon();
            break;
        case 5:
            cycleTimeout = 100;
            DigitalRain();
            break;
        default:
            Fill(CRGB::Blue);
            break;
    }
    FastLED.show();

    return true;
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

void Fun::Confetti()
{
    // random colored speckles that blink in and fade smoothly
    Fade(10);
    int pos = random16(NUM_LEDS);
    SetLed(pos, CHSV(hue + random8(64), 200, 255));
}

void Fun::Sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    Fade(20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );
    SetLed(pos, CHSV(hue, 255, 192));
}


#define FORWARD		1
#define BACKWARD 	2

void Fun::Cylon()
{
	static int state = FORWARD;
	static int led = 0;

	SetLed(led, CHSV(hue++, 255, 255));
	FastLED.show();
	Fade(20);

	switch (state) {
		case FORWARD:
			led++;
			if (led >= NUM_LEDS) {
				led = NUM_LEDS - 2;
				state = BACKWARD;
			}
			break;
		case BACKWARD:
			led--;
			if (led == 0) {
				state = FORWARD;
			}
			break;
		default:
			break;
	}
}

void Fun::DigitalRain()
{
	for (int i = 0; i < MATRIX_WIDTH; i++) {
		ledBuffer[i] = CRGB::Black;
	}

	if (random8() < 40) {
		int column = random8(MATRIX_WIDTH);
		for (int row = 0; row < RAIN_HEIGHT; row++) {
			int scale = RAIN_HEIGHT - row;
			ledBuffer[column + row * MATRIX_WIDTH] = CRGB(100 / scale, 127 + 128 / scale, 100 / scale);
		}
	}

	ScrollDown();

	for (int i = 0; i < MATRIX_WIDTH; i++) {
		SetLed(i, ledBuffer[i + MATRIX_WIDTH * (RAIN_HEIGHT - 1)]);
	}

	for (int y = RAIN_HEIGHT - 1; y > 0; y--) {
	    for (int x = 0; x < MATRIX_WIDTH; x++) {
		    ledBuffer[x + y * MATRIX_WIDTH] = ledBuffer[x + (y - 1) * MATRIX_WIDTH];
	    }
    }
}
