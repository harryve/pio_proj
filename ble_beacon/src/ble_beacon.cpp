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
#define WAKE_INTERVAL_S     30           // Wakeup time in s

#define BRIGHTNESS 16

RTC_DATA_ATTR uint16_t bootCount = 0;

BLEAdvertising *advertising;

// Prepare manufacturer data buffer: 6 bytes (T,H,P)
void fillManufacturerData(uint16_t *buf)
{
    buf[0] = 0xdead;
    buf[1] = 0xbeef;
    buf[2] = 0x55AC;
    buf[3] = bootCount;
}

void deepSleepNow(uint32_t seconds)
{
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    bootCount++;
    neopixelWrite(RGB_BUILTIN, 0, 0, BRIGHTNESS);
    Serial.begin(115200);
    //delay(1500);

  // ---- BME280 INIT ----
//  if (!bme.begin(0x76)) {
//    Serial.println("BME280 not found!");
//    deepSleepNow(WAKE_INTERVAL_S);
//  }

    // float t = bme.readTemperature();               // °C
    // float h = bme.readHumidity();                  // %
    // float p = bme.readPressure() / 100.0F;         // hPa

    // Serial.printf("T=%.2f H=%.2f P=%.2f\n", t, h, p);

    // ---- BLE Advertising-Only ----
    BLEDevice::init(DEVICE_NAME);

    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(false);

    // Manufacturer data buffer (company ID + 6 bytes payload)
    uint16_t mfgData[1 + 4];
    mfgData[0] = COMPANY_ID;

    fillManufacturerData(&mfgData[1]);

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
    deepSleepNow(WAKE_INTERVAL_S);
}

void loop()
{
    for(;;) {
        neopixelWrite(RGB_BUILTIN, 10, 0, 0);
        delay(10);
        neopixelWrite(RGB_BUILTIN, 0, 0, 0);
        delay(1000);
    }
}
