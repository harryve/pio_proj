#pragma once

void NetworkInit();
void NetworkTick();

//void NetworkPublishLdr(int val);
//void NetworkPublishBrightness(int val);

#define NWK_NO_WIFI 0x01
#define NWK_NO_MQTT 0x02
int NetworkGetStatus();
