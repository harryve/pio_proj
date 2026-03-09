
#include <Arduino.h>
#include <USB.h>
#include <USBHIDMouse.h>
#include "display.h"
#include "network.h"
#include "timesync.h"
#include "led.h"

static USBHIDMouse mouse;

void setup()
{
    LedInit();
    LedSet(true, true, true);
    Serial.begin(115200);
    Serial.println("Hello T-Dongle-S3");

    DisplayInit();
    DisplayTerminal("Hello T-Dongle-S3");

    LedSet(true, true, false);
    NetworkInit();
    TimeSyncInit();

    USB.manufacturerName("Harry");
    USB.productName("Muisje");
    USB.serialNumber("19621001");

    USB.begin();
    mouse.begin();

    delay(5000);
    LedSet(false, false, false);
}

void loop()
{
    static int dispSec = -1;
    static int dispDay = -1;
    static uint32_t lastMove = 0;

    tm timeinfo;

    NetworkTick();

    if (getLocalTime(&timeinfo)) {
        if (dispDay != timeinfo.tm_mday) {
            DisplayDate(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
            dispDay = timeinfo.tm_mday;
        }
        if (dispSec != timeinfo.tm_sec) {
            DisplayTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            dispSec = timeinfo.tm_sec;
        }
    }

    if (millis() - lastMove > 4 * 60 * 1000) {   // 4 minutes
        LedSet(false, true, false);
        mouse.move(0, 1);
        delay(10);
        mouse.move(0, -1);
        lastMove = millis();
        LedSet(false, false, false);
    }

    delay(100);
}
