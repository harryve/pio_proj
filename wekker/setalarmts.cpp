#include <Arduino.h>
#include "network.h"
#include "setalarmts.h"

#include "display.h"

SetAlarmTs::SetAlarmTs()
{
  drawRequest = false;
  hours = 99;
  minutes = 99;
}

void SetAlarmTs::Tick()
{
  if (!drawRequest) {
    return;
  }
  drawRequest = false;

  int x = 3;
  CRGB c = CRGB::Green; 

  if (hours / 10 != 0) {
    DrawDigit(x + 0, 0, hours / 10, c);
  }
  DrawDigit(x + 7, 0, hours % 10, c);
  SetLed(x + 13, 2, c);
  SetLed(x + 13, 5, c);
  DrawDigit(x + 15, 0, minutes / 10, c);
  DrawDigit(x + 22, 0, minutes % 10, c);

  FastLED.show();
}

void SetAlarmTs::ButtonHandler(Button::Id id, Button::Event event)
{
  
}

void SetAlarmTs::SetWakeupTime(int h, int m)
{
  hours = h;
  minutes = m;
  drawRequest = true;
}