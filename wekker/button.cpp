#include <Arduino.h>
#include "button.h"


#define DEBOUNCE_DELAY	         100
#define	LONG_PRESSED_DELAY	1000

Button::Button(int bottonPin/*, Button::EventCb eventCb*/)
{
  pin = pin;
  //eventCb = eventCb;
  lastState = LOW;
  buttonState = LOW;
  pressedAt = 0;
}


void Button::Tick(void)
{
  unsigned long curTime = millis();
  int reading = digitalRead(pin);

  if (reading != lastState) {
    lastEdgeAt = curTime;  // Reset edge detection time
    lastState = reading;
    Serial.println("Edge");
    return;
  }

  // Only if there is a debounced state available
  if ((curTime - lastEdgeAt) > DEBOUNCE_DELAY) {
    if (reading != buttonState) { // Button state changed
      buttonState = reading;
      if (buttonState == LOW) {
        pressedAt = curTime;
        Serial.println("Changed");
      }
    }

    if (buttonState == HIGH) {     // Button released
      if ((curTime - pressedAt) < LONG_PRESSED_DELAY) {
        // Short press
        Serial.println("Short press");
        //if (eventCb != NULL) {
        //    eventCb(Button::SHORT_PRESS);
        //}
      }
    }
    else {                        // Button still pressed
      if ((curTime - pressedAt) >= LONG_PRESSED_DELAY) {
        // Long press
        //Serial.println("Long press");
        //if (eventCb != NULL) {
        //    eventCb(Button::LONG_PRESS);
        //}
      }
    }
  }

}

