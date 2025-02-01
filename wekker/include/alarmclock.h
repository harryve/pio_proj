#pragma once
#include "display.h"
#include "button.h"

class AlarmClock : public Display {
public:
    AlarmClock();
    void init(int brightness, CRGB color);
    virtual void Tick();
    virtual void ButtonHandler(Button::Id id, Button::Event event);
    void SetColor(CRGB color);

    void Invert(bool invert);
    void SetTime(int hours, int minutes);

private:
    bool invert;
    int hours;
    int minutes;
    CRGB color;
};
