#include <Arduino.h>
#include <USB.h>
#include <USBHIDMouse.h>

USBHIDMouse mouse;

#define BRIGHTNESS  20

static void blink()
{
    static uint8_t state = 1;


    int r, g, b;
    r = (state & 0x4) == 0x4 ? BRIGHTNESS : 0;
    g = (state & 0x2) == 0x2 ? BRIGHTNESS : 0;
    b = (state & 0x1) == 0x1 ? BRIGHTNESS : 0;
    neopixelWrite(RGB_BUILTIN, g, r, b);     // Green, Red, Blue

    state++;
    if (state > 7) {
        state = 1;
    }
}

void setup()
{
    USB.manufacturerName("Harry");
    USB.productName("Muisje");
    USB.serialNumber("19621001");

    USB.begin();
    mouse.begin();
}

void loop()
{
    mouse.move(0,1);
    delay(10);
    mouse.move(0,-1);
    blink();
    delay(100);
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);    // Off
    delay(4 * 60 * 1000);   // 4 minutes
}
