#include <FastLED.h>
#include "hwdefs.h"
#include "display.h"

static CRGB ledColor;
static CRGB leds[NUM_LEDS];
static bool alarmActive;
static int hours;
static int minutes;

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

static void setLED(int x, int y, CRGB color) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return;
  int index = (y % 2 == 0) ? y * MATRIX_WIDTH + x : (y + 1) * MATRIX_WIDTH - 1 - x;
  leds[index] = color;
}

static void drawDigit(int x, int y, int digit, CRGB color) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 5; col++) {
      if (digits[digit][row] & (1 << (4 - col))) {
        setLED(x + col, y + row, color);
      }
    }
  }
}

void DisplayInit()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  //FastLED.setBrightness(brightness);
}

void DisplaySetColor(CRGB color)
{
  ledColor = color;
}

void DisplaySetBrightness(int brightness)
{
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void DisplaySetAlarmActive(bool active)
{
  alarmActive = active;
  if (alarmActive) {
    setLED(0, 0, CRGB::Red);
  }
  else {
    setLED(0, 0, CRGB::Black);
  }
  FastLED.show();
}

void DisplayDrawTime(int h, int m) 
{
  hours = h;
  minutes = m;
  DisplayRedrawTime(); 
}

void DisplayRedrawTime(bool invert) 
{
  CRGB c;

  if (invert) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ledColor;
    }
  }
  else {
    FastLED.clear();
  }

  if (alarmActive) {
    c = invert ? CRGB::Black : CRGB::Red; 
    setLED(0, 0, c);
  }

  if (invert) {
    c = CRGB::Black;
  }
  else {
    c = ledColor;
  }
  // 5 1 5 3 5 1 5
  // 0   6   14  20
  int x = 3;
  drawDigit(x + 0, 0, hours / 10, c);
  drawDigit(x + 6, 0, hours % 10, c);
  setLED(x + 12, 2, c);
  setLED(x + 12, 5, c);
  drawDigit(x + 14, 0, minutes / 10, c);
  drawDigit(x + 20, 0, minutes % 10, c);

  FastLED.show();
}

