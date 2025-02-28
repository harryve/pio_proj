#pragma once
#include "display.h"
#include "button.h"
#include "hwdefs.h"

#define RAIN_HEIGHT 6

class Fun : public Display {
public:
    Fun();
    virtual void Start();
    virtual boolean Tick();
    virtual int ButtonHandler(Button::Id id, Button::Event event);

private:
    void Confetti();
    void Sinelon();
    void Cylon();
    void DigitalRain();

    uint32_t startTime;
    uint32_t cycleTime;
    uint32_t cycleTimeout;
    int cycle;

    int patternNumber;
    uint8_t hue;
    CRGB ledBuffer[MATRIX_WIDTH * RAIN_HEIGHT];
};
