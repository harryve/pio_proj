#include <Arduino.h>
#include <cc1101_drv.h>
#include "blink.h"
#include "remotecontrol.h"
#include "network.h"
#include "ble_scan.h"

static CC1101_drv cc1101;

void setup()
{
    Serial.begin(115200);
    for (int i = 0; i < 5; i++) {
        if (Serial) {
            Serial.printf("i = %d\n", i);
            break;
        }
        neopixelWrite(RGB_BUILTIN, 5, 5, 5);
        delay(50);
        neopixelWrite(RGB_BUILTIN, 0, 0, 0);
        delay(950);
    }

    Serial.println("Blerk started");
    NetworkInit();

    cc1101.setSpiPin(12, 13, 11, 2);
    if (cc1101.getCC1101()) {         // Check the CC1101 Spi connection.
        Serial.println("Found CC1101 module");
    } else {
        Serial.println("ERROR: CC1101 module not found");
        for(;;) {
            neopixelWrite(RGB_BUILTIN, 10, 0, 0);
            delay(50);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);
            delay(250);
        }
    }

    BleScanSetup();
    cc1101.Init();              // must be set to initialize the cc1101!
    cc1101.setDRate(4.0);
    cc1101.setRxBW(16);
    cc1101.setCCMode(0);
    cc1101.setModulation(2);  // set modulation mode, 2 = ASK/OOK
    cc1101.setMHZ(433.92);
    cc1101.SetRx();

    RemoteControlInit();
}

void loop()
{
    NetworkTick();
    Blink();
    BleScanLoop();
    if (RemoteControlCheck()) {
        Serial.println("Ruft");
        NetworkPublishPressed(1);
    }
}
