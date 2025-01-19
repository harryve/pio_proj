#include <Arduino.h>
#include <FastLED.h>
//#include "display.h"
#include "alarm.h"

#define IDLE  0
#define START 1
#define ON    2
#define OFF   3

#define BEEP_COUNT  5
#define ALARM_COUNT  30
#define FREQUENCY 800
#define DUTY_TIME 500

Alarm::Alarm(int pin)
{
  buzzerPin = pin;
  state = IDLE;
  count = 0;
  pinMode(buzzerPin, INPUT_PULLDOWN);
}
    
void Alarm::trigger()
{
  state = START;
}

bool Alarm::tick(bool &invert)
{
  bool redraw = false;

  switch (state) {
    case IDLE:
      break;

    case START:
      count = ALARM_COUNT;
      tones = BEEP_COUNT - 1;
      startTime = millis();
      //tone(buzzerPin, FREQUENCY);
      Serial.println("BEEP 1st");
      redraw = true;
      invert = true;
      //DisplayRedrawTime(true);
      state = ON;
      break;

    case ON:
      if (millis() - startTime < DUTY_TIME) {
        break;
      }
      //noTone(buzzerPin);
      //DisplayRedrawTime(false);
      redraw = true;
      invert = false;
      if (--count > 0) {
        state = OFF;
        startTime = millis();
      }
      else {
        state = IDLE;
      }
      break;

    case OFF:
      if (millis() - startTime < DUTY_TIME) {
        break;
      }
      if (tones > 0) {
        //tone(buzzerPin, FREQUENCY);
        tones--;
        Serial.println("BEEP");
      }
      redraw = true;
      invert = true;
      //DisplayRedrawTime(true);
      state = ON;
      startTime = millis();
      break;

    default:
      //noTone(buzzerPin);
      break;
  }
  return redraw;
}
