#include <Arduino.h>
#include "display.h"
#include "hwdefs.h"

static CRGB leds[NUM_LEDS];

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
}

void Display::InitLeds()
{
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  fill_rainbow( leds, NUM_LEDS, 0, 7);
  FastLED.show();  
}

//void Display::Tick() // Implemented in derived classes
//{
//  Serial.print(".");
//}

void Display::SetBrightness(int b)
{
  brightness = b;
}

void Display::Fill(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}

void Display::SetLed(int x, int y, CRGB color) 
{
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) {
    return;
  }
  int index = (y % 2 == 0) ? y * MATRIX_WIDTH + x : (y + 1) * MATRIX_WIDTH - 1 - x;
  leds[index] = color;
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
