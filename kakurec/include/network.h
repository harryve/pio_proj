#pragma once

void NetworkInit();
void NetworkTick();
void NetworkPublishPressed(int val);

#define NWK_NO_WIFI 0x01
#define NWK_NO_MQTT 0x02
#define NWK_NO_NTP  0x04
