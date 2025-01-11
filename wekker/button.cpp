#include <Arduino.h>
#include "button.h"


#define DEBOUNCE_DELAY	         100
#define	LONG_PRESSED_DELAY	1000

#define PRESSED     LOW
#define RELEASED    HIGH

Button::Button(int buttonPin, Button::EventCb eventCb)
{
  pin = buttonPin;

  pinMode(pin, INPUT_PULLUP);

  eventCb = eventCb;
  lastState = RELEASED;
  buttonState = RELEASED;
  pressedAt = 0;
  longPress = false;
}

void Button::Tick(void)
{
  unsigned long curTime = millis();
  int reading = digitalRead(pin);

  if (reading != lastState) {
    lastEdgeAt = curTime;  // Reset edge detection time
    lastState = reading;
    return;
  }

  if ((curTime - lastEdgeAt) > DEBOUNCE_DELAY) {
    if (reading != buttonState) { // Button state changed
      buttonState = reading;
      if (buttonState == PRESSED) {
        pressedAt = curTime;
      }

      if (buttonState == RELEASED) {     // Button released
        if (!longPress) {
          Serial.println("Short press");
          if (eventCb != NULL) {
              eventCb(Event::SHORT_PRESS);
          }
        }
        longPress = false;
      }
    }
    else {                        // Button still pressed
      if (buttonState == PRESSED) {
        if ((curTime - pressedAt) >= LONG_PRESSED_DELAY) {
          if (!longPress) {
            longPress = true;
            Serial.println("Long press");
            if (eventCb != NULL) {
              eventCb(Event::LONG_PRESS);
            }
          }
        }
      }
    }
  }
}
