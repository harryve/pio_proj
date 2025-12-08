/*
 * ESP32 + BME280 BLE Advertising Beacon (Deep Sleep)
 * - No connection required (advertising-only)
 * - Very low power
 * - Encodes T/H/P in manufacturer data
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>
//#include <Adafruit_BME280.h>
#include <esp_sleep.h>

//Adafruit_BME280 bme;

#define SEALEVELPRESSURE_HPA (1013.25)

#define DEVICE_NAME         "BadkamerSensor"
#define COMPANY_ID          0x4845       // 16-bit company ID = "HE"
#define ADVERTISE_MS        1000         // Advertise perios in ms
#define WAKE_INTERVAL_S     1 //30           // Wakeup time in s

#define BRIGHTNESS 16

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint32_t lastRuntime = 0;

BLEAdvertising *advertising;

// Prepare manufacturer data buffer
void fillManufacturerData(uint16_t *buf, float t, float h, float p, float v)
{
    buf[0] = COMPANY_ID;
    buf[1] = (uint32_t)round((t + 273.15) * 10.0);
    buf[2] = (uint32_t)round(h * 10.0);
    buf[3] = (uint32_t)round(p * 10.0);
    buf[4] = (uint32_t)round(v * 10.0);
    buf[5] = lastRuntime;
    buf[6] = bootCount;
}

void deepSleepNow(uint32_t seconds)
{
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    uint32_t runStart = millis();

    bootCount++;
    neopixelWrite(RGB_BUILTIN, 0, 0, BRIGHTNESS);
    Serial.begin(115200);
    //delay(1500);


    // ---- BLE Advertising-Only ----
    BLEDevice::init(DEVICE_NAME);

    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(false);

    uint16_t mfgData[7];
    fillManufacturerData(mfgData, 19.3, 63.2, 1010.3, 3.7);

    BLEAdvertisementData advData;
    advData.setManufacturerData(std::string((char*)mfgData, sizeof(mfgData)));
    advertising->setAdvertisementData(advData);

    BLEDevice::startAdvertising();
    Serial.print(bootCount);
    Serial.println(" Advertising...");

    delay(ADVERTISE_MS);      // Let beacon run for short window

    BLEDevice::stopAdvertising();
    BLEDevice::deinit(true);

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    Serial.println("Sleeping...");
    lastRuntime = millis() - runStart;
    //deepSleepNow(WAKE_INTERVAL_S);
}

void loop()
{
    for(int wait = 0; wait < 30; wait++) {
        neopixelWrite(RGB_BUILTIN, 10, 0, 0);
        delay(10);
        neopixelWrite(RGB_BUILTIN, 0, 0, 0);
        delay(1000);
    }
    deepSleepNow(WAKE_INTERVAL_S);
}
