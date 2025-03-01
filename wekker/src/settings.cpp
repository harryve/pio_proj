#include <Arduino.h>
#include "settings.h"

#define USE_PREFS

#ifdef USE_PREFS
#include <Preferences.h>

static Preferences prefs;
#endif

// The entruies stored in NVS
static bool alarmActive;
static uint32_t rebootCounter;
static uint16_t wakeupTime;
// End of entries list

static uint32_t uptime;
static uint32_t timeOfDay;

static void (*changeCb)() = NULL;

void SettingsInit()
{
#ifdef USE_PREFS
    prefs.begin("wekker");

    rebootCounter = prefs.getULong("rebootcount", 0);
    rebootCounter++;
    prefs.putULong("rebootcount", rebootCounter);

    alarmActive = prefs.getBool("alarmactive", false);
    wakeupTime = prefs.getShort("wakeuptime", 6 * 60);
#else
    rebootCounter = 0;
    alarmActive = false;
    wakeupTime = 6 * 60;
#endif

    Serial.printf("Reboot counter = %ld\n", rebootCounter);
    Serial.printf("Alarm active = %d\n", (int)alarmActive);
    Serial.printf("Wakeup time = %d\n", wakeupTime);
}

void SettingsSetChangeCb(void (*p)())
{
    changeCb = p;
}

void SettingsToggleAlarmActive()
{
    alarmActive = alarmActive ? false : true;
#ifdef USE_PREFS
    prefs.putBool("alarmactive", alarmActive);
#endif
    if (changeCb != NULL) {
        changeCb();
    }
}

bool SettingsGetAlarmActive()
{
    return alarmActive;
}

uint32_t SettingsGetRebootCounter()
{
    return rebootCounter;
}

uint16_t SettingsGetWakeupTime()
{
    return wakeupTime;
}

void SettingsSetWakeupTime(uint16_t val)
{
    if (val < 2400) {
        wakeupTime = val;
#ifdef USE_PREFS
        prefs.putShort("wakeuptime", wakeupTime);
#endif
        if (changeCb != NULL) {
            changeCb();
        }
    }
}

void SettingsSetTimes(uint32_t newTimeOfDay, uint32_t newUptime)
{
    timeOfDay = newTimeOfDay;
    uptime = newUptime;
    if (changeCb != NULL) {
        changeCb();
    }
}

uint32_t SettingsGetUptime()
{
    return uptime;
}

uint32_t SettingsGetTimeOfDay()
{
    return timeOfDay;
}
