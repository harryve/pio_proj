#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define COMPANY_ID 0x4845       // 16-bit company ID = "HE"
#define SCAN_TIME  2               // active scan time in seconds

static uint32_t msgCount;
static BLEScan *pBLEScan;

static uint16_t msg[5];

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

    BLEDevice::init("Harry_client");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(100);  // scan interval
    pBLEScan->setWindow(80);     // scan window (<= interval)
    pBLEScan->setActiveScan(true); // active scan to get more info (RSSI, name, etc.)
}

void BleScanLoop()
{
    // Perform scan
    msgCount = 0;
    //Serial.println("Scanning...");
    BLEScanResults results = pBLEScan->start(SCAN_TIME, false);
    //Serial.print("Scan done. Found devices: ");
    //Serial.println(results.getCount());
    //Serial.printf("Received %u messages\n", msgCount);
    if (msgCount > 0) {
        Serial.printf("Msg: %x %x %x %x %x\n", msg[0], msg[1], msg[2], msg[3], msg[4]);
    }
    // Clear results to free memory and immediately re-scan
    pBLEScan->clearResults();

  //delay(1000); // short pause between scans
}
