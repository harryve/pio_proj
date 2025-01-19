#pragma once
#include "display.h"
#include "button.h"

class SetAlarmTs : public Display {
public:
  SetAlarmTs();
  virtual void Tick();
  virtual void ButtonHandler(Button::Id id, Button::Event event);
  void SetWakeupTime(int hours, int minutes);

private:
  bool drawRequest;
  int hours;
  int minutes;
};
