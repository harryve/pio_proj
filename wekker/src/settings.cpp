#include <Arduino.h>
#include "settings.h"

static bool alarmActive;
static void (*changeCb)() = NULL;

void SettingsSetChangeCb(void (*p)())
{
    changeCb = p;
}

void SettingsToggleAlarmActive()
{
    alarmActive = alarmActive ? false : true;
    if (changeCb != NULL) {
        changeCb();
    }
}

bool SettingsGetAlarmActive()
{
    return alarmActive;
}