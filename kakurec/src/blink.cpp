
#include <Arduino.h>
#include "blink.h"

#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)


void Blink()
{
    static unsigned long prevMillis = 0;
    static int state = 0;

    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 500) {
        prevMillis = currentMillis;
        switch (state) {
            case 0:
                neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); // Green
                state = 1;
                break;
            case 1:
                neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); // Blue
                state = 2;
                break;
            case 2:
                neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); // Red
                state = 3;
                break;
            default:
                neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Off
                state = 0;
                break;
        }
    }
}
