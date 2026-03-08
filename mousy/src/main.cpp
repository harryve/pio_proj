
#include <Arduino.h>
#include "display.h"
#include "network.h"
#include "timesync.h"

#define LED_DI_PIN     40
#define LED_CI_PIN     39

static uint32_t targetTime = 0;       // for next 1 second timeout

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello T-Dongle-S3");

    DisplayInit();
    DisplayTerminal("Hello T-Dongle-S3");
    targetTime = millis() + 1000;

    NetworkInit();
    TimeSyncInit();
    delay(5000);
}

void loop()
{
    static int dispSec = -1;
    static int dispDay = -1;
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

    delay(100);
}
