#include <Arduino.h>
#include <cc1101_drv.h>
#include "blink.h"
#include "remotecontrol.h"
#include "network.h"

static CC1101_drv cc1101;

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    delay(1000);
    Serial.println("Hello world!");
    NetworkInit();

    cc1101.setSpiPin(12, 13, 11, 2);
    if (cc1101.getCC1101()) {         // Check the CC1101 Spi connection.
        Serial.println("Found CC1101 module");
    } else {
        Serial.println("ERROR: CC1101 module not found");
        for(;;);
    }

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
    if (RemoteControlCheck()) {
        NetworkPublishPressed(1);
    }
}
