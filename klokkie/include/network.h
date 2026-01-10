#pragma once

void NetworkInit();
void NetworkTick();

void PublishState(bool present, bool timeSynced);
void PublishSensor(float temperature, float humidity, float pressure);

#define NWK_NO_WIFI 0x01
#define NWK_NO_MQTT 0x02
int NetworkGetStatus();
