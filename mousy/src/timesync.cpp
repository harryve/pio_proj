#include <Arduino.h>
#include <time.h>
//#include <coredecls.h>
#include "cred.h"
#include "display.h"
#include "timesync.h"

extern "C" {
    #include "lwip/apps/sntp.h"
}

#define SYNC_INTERVAL_SEC   3600
static const char* myTimezone = "CET-1CEST,M3.5.0/2,M10.5.0/3";

static volatile time_t lastSync = 0;

static void TimeSyncCallback()
{
    lastSync = time(nullptr);
}

void TimeSyncInit()
{
    // Init and get the time
    configTime(0, 0, TIME_SERVER_ADDR);
    setenv("TZ", myTimezone, 1);
    tzset();

    DisplayTerminal("Waiting for time sync");
    // Wait for time sync
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(100);
    }
    DisplayTerminal("Time synced!");
}

bool TimeIsSynced()
{
    if (lastSync == 0) {
        return false;
    }

    if (time(nullptr) - lastSync > SYNC_INTERVAL_SEC) {
        return false;
    }

    if (sntp_getreachability(0) == 0) {
        return false;
    }

    return true;
}
