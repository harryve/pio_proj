#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "network.h"
#include "ble_scan.h"

#define COMPANY_ID 0x4845       // 16-bit company ID = "HE"
#define SCAN_TIME  1               // active scan time in seconds

static uint32_t msgCount;
static BLEScan *pBLEScan;

static uint16_t msg[7];

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) override
    {
        std::string mfg = advertisedDevice.getManufacturerData();

        if (mfg.size() != sizeof(msg)) {
            // Not our beacon, or malformed
            return;
        }

        const uint16_t *data = (const uint16_t *)mfg.data();
        if (data[0] != COMPANY_ID) {
            return;
        }

        memcpy(msg, data, sizeof(msg));
        msgCount++;
    }
};

void BleScanSetup()
{
    Serial.println("Starting ESP32 BLE client/scanner...");

    BLEDevice::init("Blerk");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(100);  // scan interval
    pBLEScan->setWindow(80);     // scan window (<= interval)
    pBLEScan->setActiveScan(true); // active scan to get more info (RSSI, name, etc.)
}

void BleScanLoop()
{
    static uint32_t lastSeqNr = 0;

    // Perform scan
    msgCount = 0;
    BLEScanResults results = pBLEScan->start(SCAN_TIME, false);
    if (msgCount > 0) {
        //Serial.printf("Msg: %x", msg[0]);
        float t = (msg[1] / 10.0) - 273.15;
        float h = (msg[2] / 10.0);
        float p = (msg[3] / 10.0);
        float v = (msg[4] / 10.0);
        //Serial.printf(" t=%.1f, h=%.1f, p=%.1f, v=%.1f", t, h, p, v);
        //Serial.printf(" runtime=%d, boot count=%d\n", msg[5], msg[6]);
        uint32_t seqNr = msg[6];
        if (lastSeqNr != seqNr) {
            NetworkPublishBadkamer(t, h, p, v, msg[5], seqNr);
            lastSeqNr = seqNr;
        }
    }
    // Clear results to free memory and immediately re-scan
    pBLEScan->clearResults();
}
