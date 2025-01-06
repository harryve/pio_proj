#include <Arduino.h>
#include "alarm.h"

#define IDLE  0
#define START 1
#define ON    2
#define OFF   3

#define BEEP_COUNT  4
#define FREQUENCY 800
#define DUTY_TIME 500

Alarm::Alarm(int pin)
{
  buzzerPin = pin;
  state = IDLE;
  count = 0;
}
    
void Alarm::trigger()
{
  state = START;
}

void Alarm::tick()
{
  switch (state) {
    case IDLE:
      return;

    case START:
      count = BEEP_COUNT;
      startTime = millis();
      tone(buzzerPin, FREQUENCY);
      state = ON;
      break;

    case ON:
      if (millis() - startTime < DUTY_TIME) {
        return;
      }
      noTone(buzzerPin);
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
        return;
      }
      tone(buzzerPin, FREQUENCY);
      state = ON;
      startTime = millis();
      break;

    default:
      noTone(buzzerPin);
      break;
  }
}
