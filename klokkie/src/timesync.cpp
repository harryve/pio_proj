#include <Arduino.h>
#include <time.h>
#include <coredecls.h>
#include "cred.h"
#include "log.h"
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
    LOG("Time sync event\n");
}

// Set sync interval erkt niet zoals verwacht. Geen sync bij start.
//uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 ()
//{
//    return SYNC_INTERVAL_SEC * 1000UL;
//}

void TimeSyncInit()
{
    settimeofday_cb(TimeSyncCallback);
    configTime(myTimezone, TIME_SERVER_ADDR);

    LOG("Waiting for time sync");
    time_t now = time(nullptr);
    // while (now < 1767229200) {      // Epoch 1 jan 2026
    //     delay(500);
    //     now = time(nullptr);
    //     LOG(".");
    // }
    while(!TimeIsSynced()) {
        delay(500);
        LOG(".");
    }
    LOG("\nSynchronised\n");
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
