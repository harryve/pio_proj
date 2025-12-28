#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <AM2315C.h>
#include <BH1750.h>
#include <RadioLib.h>
#include "cred.h"
#include "hwdefs.h"
#include "loramsg.h"
#include "log.h"

#define FREQUENCY           868.0
#define BANDWIDTH           125.0
#define SPREADING_FACTOR    7
#define TRANSMIT_POWER      0

#define I2C_SDA                     33
#define I2C_SCL                     34
#define CHARGE_ADC          GPIO_NUM_3

static RTC_DATA_ATTR int counter = 0;
static RTC_DATA_ATTR uint32_t runtime = 0;

static SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa);
static AM2315C thSensor;
static BH1750 lightMeter;

static int lightMeterStart;

void heltec_ve(bool state)
{
    if (state) {
        pinMode(VEXT, OUTPUT);
        digitalWrite(VEXT, LOW);
    }
    else {
        // pulled up, no need to drive it
        pinMode(VEXT, INPUT);
    }
}

void heltec_led(int percent)
{
    if (percent > 0) {
        ledcSetup(LED_CHAN, LED_FREQ, LED_RES);
        ledcAttachPin(LED_PIN, LED_CHAN);
        ledcWrite(LED_CHAN, percent * 255 / 100);
    }
    else {
        ledcDetachPin(LED_PIN);
        pinMode(LED_PIN, INPUT);
    }
}

void enableVbatMeasurement()
{
    pinMode(VBAT_CTRL, OUTPUT);
    digitalWrite(VBAT_CTRL, LOW);
}

float measureVbat()
{
    return analogReadMilliVolts(VBAT_ADC) * (390.0 + 100.0) / 100.0;
}

void heltec_deep_sleep(int seconds)
{
    // It seems to make no sense to do a .begin() here, but in case the radio is
    // not interacted with at all before sleep, it will not respond to just
    // .sleep() and then consumes 800 µA more than it should in deep sleep.
    radio.begin();
    // 'false' here is to not have a warm start, we re-init the after sleep.
    // Turn off external power
    heltec_ve(false);
    // Turn off LED
    heltec_led(0);
    // Set all pins to input to save power
    pinMode(VBAT_CTRL, INPUT);
    pinMode(VBAT_ADC, INPUT);
    pinMode(DIO1, INPUT);
    pinMode(RST_LoRa, INPUT);
    pinMode(BUSY_LoRa, INPUT);
    pinMode(SS, INPUT);
    pinMode(MISO, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(SCK, INPUT);

    esp_sleep_enable_timer_wakeup((int64_t)seconds * 1000000);
    esp_deep_sleep_start();
}

static bool isCharging()
{
    if (analogRead(CHARGE_ADC) > 500.0) {
        heltec_led(20);
        return false;
    }
    return false;
}

void setup()
{
#ifdef SHOW_LOG
    Serial.begin(115200);
#endif
    LOG("\nInit balkon sensor %s %s at %ld\n", __DATE__, __TIME__, millis());

    heltec_ve(true);

    LOG("Init I2C %ld\n", millis());
    Wire.begin(I2C_SDA, I2C_SCL); //, 100000);
    thSensor.begin();
    thSensor.requestData();

    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
    lightMeterStart = millis();
    LOG("Sensors done %ld\n", millis());

    if ((radio.begin() != RADIOLIB_ERR_NONE) ||
        (radio.setFrequency(FREQUENCY) != RADIOLIB_ERR_NONE) ||
        (radio.setBandwidth(BANDWIDTH) != RADIOLIB_ERR_NONE) ||
        (radio.setSpreadingFactor(SPREADING_FACTOR) != RADIOLIB_ERR_NONE) ||
        (radio.setOutputPower(TRANSMIT_POWER) != RADIOLIB_ERR_NONE) ||
        (radio.setCRC(2) != RADIOLIB_ERR_NONE)) {
        radio.sleep(false);
        LOG("radio init failed\n");
        heltec_led(50);
        delay(500);
        heltec_deep_sleep(60);
    }

    LOG("radio init done %ld\n", millis());

    if (isCharging()) {
        LOG("Connect to WiFi");
        WiFi.setHostname("balkonsensor");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            LOG(".");
            delay(500);
        }
        LOG("\nConnected\n");
        heltec_led(50);
        ArduinoOTA.setHostname("balkonsensor");
        ArduinoOTA.onStart([]() {
            LOG("Start\n");
        });
        ArduinoOTA.onEnd([]() {
            LOG("\nEnd\n");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            LOG("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            LOG("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) LOG("Auth Failed\n");
            else if (error == OTA_BEGIN_ERROR) LOG("Begin Failed\n");
            else if (error == OTA_CONNECT_ERROR) LOG("Connect Failed\n");
            else if (error == OTA_RECEIVE_ERROR) LOG("Receive Failed\n");
            else if (error == OTA_END_ERROR) LOG("End Failed\n");
        });
        ArduinoOTA.begin();
    }
    enableVbatMeasurement();
}

void ChargeLoop()
{
    ArduinoOTA.handle();
}

void AccuLoop()
{
    int status;
    float temp, hum;

    LOG("loop %ld\n", millis());
    thSensor.readData();
    if ((status = thSensor.convert()) == AM2315C_OK) {
        temp = thSensor.getTemperature();
        hum = thSensor.getHumidity();
    }
    else {
        temp = 0.0;
        hum = 0.0;
        LOG("AM2315C read error: %d\n", status);
    }
    LOG("ht sensor done %ld\n", millis());

    while (millis() - lightMeterStart < 120) {
        delay(1);
    }
    LOG("Lightmeter done %ld\n", millis());
    float lux = lightMeter.readLightLevel();

    struct LoraMsg loraMsg;
    loraMsg.id = SENSOR2_ID;
    loraMsg.seq = counter;
    loraMsg.temperature = round(temp*10); // Tenths degeree Celcius
    loraMsg.humidity = round(hum);        // %
    loraMsg.illuminance = round(lux);
    loraMsg.vbat = round(measureVbat());   // mv
    loraMsg.runtime = (uint16_t)runtime;

    radio.transmit((uint8_t *)&loraMsg, sizeof(loraMsg));
    LOG("TX done (t=%d, h=%d, l=%d) %d\n", loraMsg.temperature, loraMsg.humidity, loraMsg.illuminance, millis());
    radio.sleep(false);

    counter += 1;
    runtime = millis();

    heltec_deep_sleep(5 * 60);
}

void loop()
{
    if (isCharging()) {
        ChargeLoop();
    }
    else {
        AccuLoop();
    }
}
