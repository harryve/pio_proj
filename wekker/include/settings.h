#pragma once

void SettingsInit();
void SettingsSetChangeCb(void (*p)());
void SettingsToggleAlarmActive();

bool SettingsGetAlarmActive();
uint32_t SettingsGetRebootCounter();

uint16_t SettingsGetWakeupTime();
void SettingsSetWakeupTime(uint16_t val);

void SettingsSetTimes(uint32_t timeOfDay, uint32_t uptime);
uint32_t SettingsGetTimeOfDay();
uint32_t SettingsGetUptime();
