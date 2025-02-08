#pragma once

void SettingsInit();
void SettingsSetChangeCb(void (*p)());
void SettingsToggleAlarmActive();

bool SettingsGetAlarmActive();
uint32_t SettingsGetRebootCounter();

uint16_t SettingsGetWakeupTime();
void SettingsSetWakeupTime(uint16_t val);

void SettingsSetUptime(uint32_t val);
uint32_t SettingsGetUptime();
