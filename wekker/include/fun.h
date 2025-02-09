#pragma once
#include "display.h"
#include "button.h"

class Fun : public Display {
public:
    Fun();
    virtual void Start();
    virtual boolean Tick();
    virtual int ButtonHandler(Button::Id id, Button::Event event);

private:
    uint32_t startTime;
    uint32_t cycleTime;
    int cycle;
};
