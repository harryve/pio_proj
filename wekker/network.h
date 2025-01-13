void NetworkInit();
void NetworkTick();
int NetworkGetStatus();
void NetworkPublishLdr(int val);
void NetworkPublishBrightness(int val);
int NetworkGetErrors();

#define NWK_NO_WIFI 0x01
#define NWK_NO_MQTT 0x02
#define NWK_NO_NTP  0x04
