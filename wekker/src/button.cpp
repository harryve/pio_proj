#include <Arduino.h>
#include "button.h"


#define DEBOUNCE_DELAY	     100
#define	LONG_PRESSED_DELAY	1000

#define PRESSED     LOW
#define RELEASED    HIGH

Button::Button(int buttonPin, Id id, EventCb cb)
{
    this->pin = buttonPin;
    this->id = id;

    pinMode(pin, INPUT_PULLUP);

    eventCb = cb;
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

    if ((curTime - lastEdgeAt) < DEBOUNCE_DELAY) {
        return;
    }
    if (reading != buttonState) { // Button state changed
        buttonState = reading;
        if (buttonState == PRESSED) {
            pressedAt = curTime;
        }

        if (buttonState == RELEASED) {     // Button released
            if (!longPress) {
                HandleEvent(Event::SHORT_PRESS);
            }
            else {
                HandleEvent(Event::LONG_PRESS_END);
            }
            longPress = false;
        }
    }
    else {                        // Button not changed
        if (buttonState == PRESSED) {
            if ((curTime - pressedAt) >= LONG_PRESSED_DELAY) {
                if (!longPress) {
                    longPress = true;
                    HandleEvent(Event::LONG_PRESS);
                }
            }
        }
    }
}

void Button::HandleEvent(Event event)
{
    if (eventCb != NULL) {
        eventCb(id, event);
    }
}
