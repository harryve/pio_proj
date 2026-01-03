#include <Arduino.h>
#include "hwdefs.h"
#include "bsp.h"

using namespace bsp;

void bsp::Led(int percent)
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

void bsp::SensorSupply(bool enable)
{
    pinMode(VEXT, OUTPUT);
    if (enable) {
        //pinMode(VEXT, OUTPUT);
        digitalWrite(VEXT, LOW);
    }
    else {
        // pulled up, no need to drive it
        //pinMode(VEXT, INPUT);
        digitalWrite(VEXT, HIGH);
    }
}

void bsp::DeepSleep(int seconds)
{
    // Turn off external power
    SensorSupply(false);

    // Radio
    pinMode(LORA_DIO1, INPUT);
    pinMode(LORA_RST, INPUT);
    pinMode(LORA_BUSY, INPUT);

    // SPI
    pinMode(NSS, INPUT);
    pinMode(MISO, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(SCK, INPUT);

    esp_sleep_enable_timer_wakeup((int64_t)seconds * 1000000);
    esp_deep_sleep_start();
}

void bsp::VbatMeasurementEnable(bool enable)
{
    pinMode(VBAT_CTRL, OUTPUT);
    digitalWrite(VBAT_CTRL, enable ? LOW : HIGH);
}


float bsp::MeasureVbat()
{
    return analogReadMilliVolts(VBAT_ADC) * (390.0 + 100.0) / 100.0;
}

void bsp::LowPowerDelayMs(int ms)
{
    esp_sleep_enable_timer_wakeup((int64_t)ms * 1000);
    esp_light_sleep_start();
}
