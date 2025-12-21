/*
 * ESP32 + BME280 BLE Advertising Beacon (Deep Sleep)
 * - No connection required (advertising-only)
 * - Very low power
 * - Encodes T/H/P in manufacturer data
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <Adafruit_BME280.h>
#include <esp_sleep.h>

#define LED_ON  LOW
#define LED_OFF HIGH

static Adafruit_BME280 bme;

//#define SEALEVELPRESSURE_HPA (1013.25)

#define DEVICE_NAME         "BadkamerSensor"
#define COMPANY_ID          0x4845       // 16-bit company ID = "HE"
#define ADVERTISE_MS        500          // Advertise perios in ms
#define WAKE_INTERVAL_S     300          // Wakeup time in s

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint32_t lastRuntime = 0;

BLEAdvertising *advertising;

static void fillManufacturerData(uint16_t *buf, float t, float h, float p, float v)
{
    // Manufacturer data buffer (company ID + payload)
    buf[0] = COMPANY_ID;
    buf[1] = (uint32_t)round((t + 273) * 10.0);
    buf[2] = (uint32_t)round(h * 10.0);
    buf[3] = (uint32_t)round(p * 10.0);
    buf[4] = (uint32_t)round(v * 10.0);
    buf[5] = lastRuntime;
    buf[6] = bootCount;
}

static float ReadVbat()
{
//    analogSetAttenuation(ADC_11db);
    uint32_t mvbat = analogReadMilliVolts(35);
    return (mvbat * 2.0) / 1000.0;
}

static void deepSleepNow(uint32_t seconds)
{
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    uint32_t runStart = millis();
    float t, h, p;

    bootCount++;

    Serial.begin(115200);

    // ---- BME280 INIT ----
    if (!bme.begin(0x76)) {
        Serial.println("BME280 not found!");
        //deepSleepNow(WAKE_INTERVAL_S);
        t = h = p = 0.0;
    }
    else {
        t = bme.readTemperature();               // °C
        h = bme.readHumidity();                  // %
        p = bme.readPressure() / 100.0;          // hPa

        Serial.printf("T=%.1f H=%.1f P=%.1f\n", t, h, p);
    }
    //uint32_t t1 = millis() - runStart;
    float vbat = ReadVbat();
    Serial.printf("Vbat=%.2f\n", vbat);

    // ---- BLE Advertising-Only ----
    BLEDevice::init(DEVICE_NAME);
    //uint32_t t2 = millis() - runStart;

    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(false);

    uint16_t mfgData[7];
    fillManufacturerData(mfgData, t, h, p, vbat);

    BLEAdvertisementData advData;
    advData.setManufacturerData(std::string((char*)mfgData, sizeof(mfgData)));
    advertising->setAdvertisementData(advData);

    //uint32_t t3 = millis() - runStart;
    BLEDevice::startAdvertising();
    Serial.print(bootCount);
    Serial.println(" Advertising...");

    //uint32_t t4 = millis() - runStart;
    delay(ADVERTISE_MS);      // Let beacon run for short window
    //uint32_t t5 = millis() - runStart;

    BLEDevice::stopAdvertising();
    BLEDevice::deinit(true);
    //uint32_t t6 = millis() - runStart;

    //digitalWrite(LED_BUILTIN, LED_OFF);
    //Serial.printf("%d %d %d %d %d %d\n", t1, t2, t3, t4, t5, t6);
    Serial.println("Sleeping...");
    lastRuntime = millis() - runStart;
    deepSleepNow(WAKE_INTERVAL_S);
}

void loop()
{
}
