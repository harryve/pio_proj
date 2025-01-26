#pragma once

#include <FastLED.h>
#include "button.h"

class Display {
public:
  Display();
  virtual void Tick() = 0;
  virtual void ButtonHandler(Button::Id id, Button::Event event) = 0;
  void SetBrightness(int brightness);

protected:
  void InitLeds();
  void Fill(CRGB color);
  void SetLed(int x, int y, CRGB color);
  void DrawDigit(int x, int y, int digit, CRGB color); 
  
//private:
};
