#include <Arduino.h>
//#include "fl/ui.h"
#include "fl/xymap.h"
#include "display.h"
#include "hwdefs.h"

using namespace fl;

// Add extra led position for XY errors
static CRGB leds[NUM_LEDS];
static XYMap xymap(MATRIX_WIDTH, MATRIX_HEIGHT, true);

// 5x8 (from HelvetiPixel.ttf) font for digits
static const uint8_t digits[10][8] = {
    {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
    {0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010, 0b00010}, // 1
    {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111}, // 2
    {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b00001, 0b10001, 0b01110}, // 3
    {0b00001, 0b00011, 0b00101, 0b01001, 0b10001, 0b11111, 0b00001, 0b00001}, // 4
    {0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}, // 5
    {0b01110, 0b10001, 0b10000, 0b11110, 0b10001, 0b10001, 0b10001, 0b01110}, // 6
    {0b11111, 0b00001, 0b00010, 0b00010, 0b00100, 0b00100, 0b01000, 0b01000}, // 7
    {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // 8
    {0b01110, 0b10001, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110}  // 9
};


Display::Display()
{
    minimumBrightness = 0;
}

void Display::InitLeds()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setScreenMap(xymap);
    FastLED.setBrightness(50);
    Rainbow(0, 7);
    FastLED.show();
}

void Display::Redraw()
{
    drawRequest = true;
}

void Display::SetBrightness(int brightness)
{
    if (brightness >= minimumBrightness) {
        FastLED.setBrightness(brightness);
        Redraw();
    }
}

void Display::SetMinimumBrightness(int minimum)
{
    minimumBrightness = minimum;
}

void Display::Fill(CRGB color)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
}

void Display::SetLed(int x, int y, CRGB color)
{
    leds[xymap(x, y)] = color;
}

void Display::SetLed(uint16_t idx, CRGB color)
{
    if (idx < NUM_LEDS) {
        leds[idx] = color;
    }
}

CRGB Display::GetLed(uint16_t idx)
{
    if (idx < NUM_LEDS) {
        return leds[idx];
    }
    return CRGB::Black;
}

void Display::Fade(uint8_t fade)
{
  fadeToBlackBy( leds, NUM_LEDS, fade);
}

void Display::ScrollDown()
{
	// scroll
    for (int row = MATRIX_HEIGHT - 1; row > 0; row--) {
        for (int col = 0; col < MATRIX_WIDTH; col++) {
            SetLed(xymap(col, row), GetLed(xymap(col, row - 1)));
        }
    }

	// Clear first line
    for (int col = 0; col < MATRIX_WIDTH; col++) {
        SetLed(col, 0, CRGB::Black);
    }
}

void Display::DrawDigit(int x, int y, int digit, CRGB color)
{
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 5; col++) {
            if (digits[digit][row] & (1 << (4 - col))) {
                SetLed(x + col, y + row, color);
            }
        }
    }
}

void Display::Rainbow(uint8_t initialhue, uint8_t deltahue )
{
    CHSV hsv;
    hsv.hue = initialhue;
    hsv.val = 255;
    hsv.sat = 240;
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            SetLed(x, y, hsv);
            hsv.hue += deltahue;
        }
    }
}
