#include <Arduino.h>
#include "settings.h"

#include <Preferences.h>
static Preferences prefs;

// The entruies stored in NVS
static bool alarmActive;
static uint32_t rebootCounter;
static uint16_t wakeupTime;
// End of entries list

static uint32_t uptime;

static void (*changeCb)() = NULL;

void SettingsInit()
{
    prefs.begin("wekker");

    rebootCounter = prefs.getULong("rebootcount", 0);
    rebootCounter++;
    prefs.putULong("rebootcount", rebootCounter);

    alarmActive = prefs.getBool("alarmactive", false);
    wakeupTime = prefs.getShort("wakeuptime", 6 * 60);

//    prefs.end();

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
    prefs.putBool("alarmactive", alarmActive);
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
        prefs.putShort("wakeuptime", wakeupTime);
        if (changeCb != NULL) {
            changeCb();
        }
    }
}

void SettingsSetUptime(uint32_t val)
{
    uptime = val;
    if (changeCb != NULL) {
        changeCb();
    }
}

uint32_t SettingsGetUptime()
{
    return uptime;
}
