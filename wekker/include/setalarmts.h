#pragma once
#include "display.h"
#include "button.h"

class SetAlarmTs : public Display {
public:
    SetAlarmTs();
    virtual void Tick();
    virtual int ButtonHandler(Button::Id id, Button::Event event);

private:
    int wakeupTime;
};
