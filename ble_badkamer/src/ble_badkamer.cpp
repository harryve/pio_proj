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

#define SEALEVELPRESSURE_HPA (1013.25)

#define DEVICE_NAME         "BadkamerSensor"
#define COMPANY_ID          0x4845       // 16-bit company ID = "HE"
#define ADVERTISE_MS        1000         // Advertise perios in ms
#define WAKE_INTERVAL_S     30           // Wakeup time in s

#define BRIGHTNESS 16

RTC_DATA_ATTR uint16_t bootCount = 0;
RTC_DATA_ATTR uint32_t lastRuntime = 0;

BLEAdvertising *advertising;

// Prepare manufacturer data buffer: 6 bytes (T,H,P)
void fillManufacturerData(uint16_t *buf, float t, float h, float p, float v)
{
    // "temp": float("%0.1f" % temp),
    // "hum": float("%.1f" % hum),
    // "pressure": float("%.1f" % pres),
    // "vbat": float("%0.2f" % vbat),
    // "rssi": wlan.status('rssi'),
    // "runtime": pers.get_prev_runtime(),
    // "counter": start_count }
    // Manufacturer data buffer (company ID + payload)
    buf[0] = COMPANY_ID;
    buf[1] = (uint32_t)round((t + 273.15) * 10.0);
    buf[2] = (uint32_t)round(h * 10.0);
    buf[3] = (uint32_t)round(p * 10.0);
    buf[4] = (uint32_t)round(v * 10.0);
    buf[5] = lastRuntime;
    buf[6] = bootCount;
}

float ReadVbat()
{
    //vbat_adc = machine.ADC(machine.Pin(35))
    //vbat_adc.atten(machine.ADC.ATTN_11DB)       #Full range: 3.3v
    //return (vbat_adc.read_uv() * 2) / 1000000.0
    analogSetAttenuation(ADC_11db);
    uint32_t mvbat = analogReadMilliVolts(35);
    return (mvbat * 2.0) / 1000.0;
}

void deepSleepNow(uint32_t seconds)
{
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    uint32_t runStart = millis();
    float t, h, p;

    bootCount++;

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LED_ON);

    Serial.begin(115200);
    //delay(1500);

    // ---- BME280 INIT ----
    if (!bme.begin(0x76)) {
        Serial.println("BME280 not found!");
        //deepSleepNow(WAKE_INTERVAL_S);
        t = h = p = 0.0;
    }
    else {
        t = bme.readTemperature();               // °C
        h = bme.readHumidity();                  // %
        p = bme.readPressure() / 100.0F;         // hPa

        Serial.printf("T=%.2f H=%.2f P=%.2f\n", t, h, p);
    }
    float vbat = ReadVbat();
    Serial.printf("Vbat=%.2f\n", vbat);

    // ---- BLE Advertising-Only ----
    BLEDevice::init(DEVICE_NAME);

    advertising = BLEDevice::getAdvertising();
    advertising->setScanResponse(false);

    uint16_t mfgData[7];
    fillManufacturerData(mfgData, t, h, p, vbat);

    BLEAdvertisementData advData;
    advData.setManufacturerData(std::string((char*)mfgData, sizeof(mfgData)));
    advertising->setAdvertisementData(advData);

    BLEDevice::startAdvertising();
    Serial.print(bootCount);
    Serial.println(" Advertising...");

    delay(ADVERTISE_MS);      // Let beacon run for short window

    BLEDevice::stopAdvertising();
    BLEDevice::deinit(true);

    digitalWrite(LED_BUILTIN, LED_OFF);
    Serial.println("Sleeping...");
    lastRuntime = millis() - runStart;
    deepSleepNow(WAKE_INTERVAL_S);
}

void loop()
{
    for(;;) {
        delay(10);
        delay(1000);
    }
}
