#pragma once

void NetworkInit();
void NetworkTick();
void NetworkPublishPressed(int val);
void NetworkPublishBadkamer(float temperature, float humidity, float pressure, float vbat, uint32_t runTime, uint32_t seqNr);

#define NWK_NO_WIFI 0x01
#define NWK_NO_MQTT 0x02
#define NWK_NO_NTP  0x04
