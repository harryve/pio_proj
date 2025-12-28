#include <Arduino.h>
#include <Wire.h>
#include <LoRa.h>
#include <AM2315C.h>
#include "hwdefs.h"
#include "display.h"
#include "loramsg.h"
#include "log.h"

static RTC_DATA_ATTR int counter = 0;
static RTC_DATA_ATTR uint32_t runtime = 0;

static AM2315C sensor;

void GoToSleep()
{
    Serial.println("Going to sleep");

    DisplaySleep();

    pinMode(RADIO_CS_PIN, INPUT);
    pinMode(RADIO_RST_PIN, INPUT);
    pinMode(RADIO_DIO0_PIN, INPUT);
    pinMode(RADIO_CS_PIN, INPUT);
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SDA, INPUT);
    pinMode(I2C_SCL, INPUT);
    pinMode(OLED_RST, INPUT);
    pinMode(RADIO_SCLK_PIN, INPUT);
    pinMode(RADIO_MISO_PIN, INPUT);
    pinMode(RADIO_MOSI_PIN, INPUT);
    pinMode(SDCARD_MOSI, INPUT);
    pinMode(SDCARD_MISO, INPUT);
    pinMode(SDCARD_SCLK, INPUT);
    pinMode(SDCARD_CS, INPUT);
    pinMode(BOARD_LED, INPUT);
    pinMode(ADC_PIN, INPUT);

    runtime = millis();
    esp_sleep_enable_timer_wakeup(5*60 * (1000*1000));
    esp_deep_sleep_start();
}

void setup()
{
#ifdef SHOW_LOG
    Serial.begin(115200);
#endif
    LOG("Start LoRa sensor " __DATE__ ", " __TIME__ "\n");
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
    Wire.begin(I2C_SDA, I2C_SCL);

    sensor.begin();

    if (counter == 0) {
        pinMode(BOARD_LED, OUTPUT);
        digitalWrite(BOARD_LED, LED_ON);
        DisplayInit();
    }

    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        LOG("Starting LoRa failed!\n");
        while (1);
    }
}

void loop()
{
    int status;
    float temp, hum, vbat;

    vbat = analogReadMilliVolts(ADC_PIN) * 2.0 / 1000.0;
    LOG("Voltage = %.3f\n", vbat);

    if ((status = sensor.read()) != AM2315C_OK) {
        temp = 0.0;
        hum = 0.0;
        LOG("AM2315C read error: %d\n", status);
    }
    else {
        temp = sensor.getTemperature();
        hum = sensor.getHumidity();

        LOG("Hum = %.0f%, temperature = %.1f C\n", hum, temp);

        LOG("Sending packet: %d\n", counter);
        struct LoraMsg loraMsg;
        loraMsg.id = 0x48764531;              // HvE1
        loraMsg.seq = counter;
        loraMsg.temperature = round(temp*10); // Tenths degeree Celcius
        loraMsg.humidity = round(hum);        // %
        loraMsg.vbat = round(vbat* 1000.0);   // mv
        loraMsg.runtime = (uint16_t)runtime;
        loraMsg.illuminance = 0;

        LoRa.enableCrc();
        LoRa.beginPacket();
        LoRa.write((const uint8_t *)&loraMsg, sizeof(loraMsg));
        LoRa.endPacket();

        DisplayShowMeasurement(temp, hum);
    }

    counter++;
    GoToSleep();
}
