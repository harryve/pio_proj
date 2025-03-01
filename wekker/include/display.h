#pragma once

#include <FastLED.h>
#include "button.h"

class Display {
public:
    Display();
    virtual void Start() = 0;
    virtual boolean Tick() = 0;
    virtual int ButtonHandler(Button::Id id, Button::Event event) = 0;
    void SetBrightness(int brightness);
    void SetMinimumBrightness(int minimum);
    void Redraw();

protected:
    void InitLeds();
    void Fill(CRGB color);
    void SetLed(int x, int y, CRGB color);
    void SetLed(uint16_t idx, CRGB color);
    CRGB GetLed(uint16_t idx);
    void Fade(uint8_t fade);
    void ScrollDown();
    void DrawDigit(int x, int y, int digit, CRGB color);

    void Rainbow(uint8_t initialhue, uint8_t deltahue);

    bool drawRequest;
    int minimumBrightness;
};
