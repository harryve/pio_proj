#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <Adafruit_BME280.h>
#include <esp_sleep.h>
#include "log.h"

#define LED_ON  LOW
#define LED_OFF HIGH

#define SENSOR_VCC_PIN  32
#define SDA_PIN         21
#define SCL_PIN         22
#define ADC_PIN         35

#define DEVICE_NAME         "BadkamerSensor"
#define COMPANY_ID          0x4845       // 16-bit company ID = "HE"
#define ADVERTISE_MS        500          // Advertise perios in ms
#define WAKE_INTERVAL_S     300          // Wakeup time in s

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint32_t lastRuntime = 0;

static Adafruit_BME280 bme;
static BLEAdvertising *advertising;

static void FillManufacturerData(uint16_t *buf, float t, float h, float p, float v)
{
    // Manufacturer data buffer (company ID + payload)
    buf[0] = COMPANY_ID;
    buf[1] = (uint16_t)round((t + 100) * 10.0);
    buf[2] = (uint16_t)round(h * 10.0);
    buf[3] = (uint16_t)round(p * 10.0);
    buf[4] = (uint16_t)round(v * 1000.0);   // mv
    buf[5] = lastRuntime;
    buf[6] = bootCount;
}

static float ReadVbat()
{
    analogSetAttenuation(ADC_11db);
    uint32_t mvbat = analogReadMilliVolts(ADC_PIN);
    return (mvbat * 2.0) / 1000.0;
}

static void DeepSleepNow(uint32_t seconds)
{
    pinMode(ADC_PIN, INPUT);
    pinMode(SDA_PIN, INPUT);
    pinMode(SCL_PIN, INPUT);
    pinMode(SENSOR_VCC_PIN, INPUT);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    uint32_t runStart = millis();
    float t, h, p;

    bootCount++;

#ifdef SHOW_LOG
    Serial.begin(115200);
#endif

    LOG("Start badkamer sensor " __DATE__ ", " __TIME__ ", at %ld\n", millis());
    // Switch on sensor
    pinMode(SENSOR_VCC_PIN, OUTPUT);
    digitalWrite(SENSOR_VCC_PIN, HIGH);

    // Give sensor time to start, use BLE init as delay
    LOG("BLE Advertising init %ld\n", millis());
    BLEDevice::init("");

    // ---- BME280 INIT ----
    LOG("BME start %ld\n", millis());
    if (!bme.begin(0x76)) {
        LOG("BME280 not found!\n");
        t = h = p = 0.0;
    }
    else {
        t = bme.readTemperature();               // °C
        h = bme.readHumidity();                  // %
        p = bme.readPressure() / 100.0;          // hPa

        LOG("T=%.1f H=%.1f P=%.1f (%ld)\n", t, h, p, millis());
    }
    digitalWrite(SENSOR_VCC_PIN, LOW);

    float vbat = ReadVbat();
    LOG("Vbat=%.2f\n", vbat);

    LOG("Get advertising %ld\n", millis());
    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(false);

    uint16_t mfgData[7];
    FillManufacturerData(mfgData, t, h, p, vbat);

    BLEAdvertisementData advData;
    advData.setManufacturerData(std::string((char*)mfgData, sizeof(mfgData)));
    advertising->setAdvertisementData(advData);
    //advData.setName(DEVICE_NAME);
    advData.setShortName(DEVICE_NAME);

    LOG("Start advertising %ld\n", millis());
    BLEDevice::startAdvertising();

    delay(ADVERTISE_MS);      // Let beacon run for short window

    BLEDevice::stopAdvertising();
    BLEDevice::deinit(true);

    LOG("Stop advertising %ld\n", millis());

    LOG("Sleeping...\n\n");
    lastRuntime = millis() - runStart;
    DeepSleepNow(WAKE_INTERVAL_S);
}

void loop()
{
    // We slapen al...
}
