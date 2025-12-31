#include <Arduino.h>
#include <AM2315C.h>
#include <BH1750.h>
#include <RadioLib.h>
#include "hwdefs.h"
#include "bsp.h"
#include "loramsg.h"
#include "log.h"

#define FREQUENCY           868.0
#define BANDWIDTH           125.0
#define SPREADING_FACTOR    7
#define TRANSMIT_POWER      0

static RTC_DATA_ATTR int counter = 0;
static RTC_DATA_ATTR uint32_t runtime = 0;
static RTC_DATA_ATTR uint32_t errors = 0;

static SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa);
static AM2315C thSensor;
static BH1750 lightMeter;

static int lightMeterStart;

static bool isCharging()
{
    if (analogRead(CHARGE_ADC) > 500.0) {
        return true;
    }
    return false;
}

void setup()
{
    int r;

#ifdef SHOW_LOG
    Serial.begin(115200);
#endif
    LOG("\nInit balkon sensor %s %s at %ld\n", __DATE__, __TIME__, millis());

    bsp::SensorSupply(true);
    LOG("Sensor supply is now on %ld\n", millis());

    // Delay 200 ms, make sure the th sensor is powered up completely
    bsp::LowPowerDelayMs(200);

    LOG("Init I2C %ld\n", millis());
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE)) {
        LOG("Init light meter failed\n");
    }
    lightMeterStart = millis();
    // Delay 50 ms, light meter measurement is slow
    bsp::LowPowerDelayMs(50);

    if (!thSensor.begin()) {
        LOG("Init TH sensor failed\n");
        errors++;
        bsp::DeepSleep(60);     // Unrecoverable error
    }
    else {
        if ((r = thSensor.requestData()) != AM2315C_OK) {
            LOG("TH sensor request data failed (%d)", r);
        }
    }
    LOG("Sensors initialized %ld\n", millis());

    // Give sensors time to power up, init radio first
    if ((radio.begin() != RADIOLIB_ERR_NONE) ||
        (radio.setFrequency(FREQUENCY) != RADIOLIB_ERR_NONE) ||
        (radio.setBandwidth(BANDWIDTH) != RADIOLIB_ERR_NONE) ||
        (radio.setSpreadingFactor(SPREADING_FACTOR) != RADIOLIB_ERR_NONE) ||
        (radio.setOutputPower(TRANSMIT_POWER) != RADIOLIB_ERR_NONE) ||
        (radio.setCRC(2) != RADIOLIB_ERR_NONE)) {
        radio.sleep(false);
        LOG("radio init failed\n");
        bsp::DeepSleep(120);
    }
    LOG("radio init done %ld\n", millis());

    bsp::VbatMeasurementEnable(true);
}

void ChargeLoop()
{
    static uint32_t lastMillis = 0;
    static int state = 0;

    if (millis() - lastMillis > 300) {
        state = 1 - state;
        bsp::Led(state ? 20 : 0);
        lastMillis = millis();
    }
}

void AccuLoop()
{
    int status;
    float temp, hum;

    LOG("loop %ld\n", millis());
    while (thSensor.isMeasuring()) {
        delay(1);
    }
    LOG("thSensor ready %ld\n", millis());
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
    LOG("thSensor done %ld\n", millis());

    LOG("Wait for light sensor %ld\n", 120 - (millis() - lightMeterStart));
    while (millis() - lightMeterStart < 120) {
        delay(1);
    }
    float lux = lightMeter.readLightLevel();
    LOG("Lightmeter done %ld\n", millis());

    float vbat = bsp::MeasureVbat();
    bsp::VbatMeasurementEnable(false);
    LOG("vbat done %ld\n", millis());

    struct LoraMsg loraMsg;
    loraMsg.id = SENSOR2_ID;
    loraMsg.seq = counter;
    loraMsg.temperature = round(temp*10); // Tenths degeree Celcius
    loraMsg.humidity = round(hum);        // %
    loraMsg.illuminance = round(lux);
    loraMsg.vbat = round(vbat);   // mv
    loraMsg.runtime = (uint16_t)runtime;

    radio.transmit((uint8_t *)&loraMsg, sizeof(loraMsg));
    LOG("TX done (c=%d, t=%d, h=%d, l=%d) %d, errors=%d\n", counter, loraMsg.temperature, loraMsg.humidity, loraMsg.illuminance, millis(), errors);
    radio.sleep(false);         // Discard configuration

    counter += 1;
    runtime = millis();

    bsp::DeepSleep(5 * 60);
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
