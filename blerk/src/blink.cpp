
#include <Arduino.h>
#include "blink.h"

#define BRIGHTNESS 5 // Change white brightness (max 255)


void Blink(bool error)
{
    static unsigned long prevMillis = 0;
    static int state = 0;

    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 500) {
        prevMillis = currentMillis;
        switch (state) {
            case 0:
                neopixelWrite(RGB_BUILTIN, 0, BRIGHTNESS, 0); // Red
                state = error ? 3 : 1;
                break;
            case 1:
                neopixelWrite(RGB_BUILTIN, 0, 0, BRIGHTNESS); // Blue
                state = 2;
                break;
            case 2:
                neopixelWrite(RGB_BUILTIN, BRIGHTNESS, 0, 0); // Green
                state = 3;
                break;
            default:
                neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Off
                state = 0;
                break;
        }
    }
}
