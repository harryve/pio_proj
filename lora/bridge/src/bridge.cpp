//Board: TTGO LoRa32-OLED

#include <Arduino.h>
#include <LoRa.h>

#include "network.h"
#include "hwdefs.h"
#include "loramsg.h"
#include "log.h"

#include "display.h"

void setup()
{
    Serial.begin(115200);
    LOG("\n\n\nLoRa Bridge " __DATE__ ", " __TIME__ "\n");

    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    pinMode(BOARD_LED, OUTPUT);
    digitalWrite(BOARD_LED, LED_ON);
    DisplayInit();
    NetworkInit();

    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        LOG("Starting LoRa failed!\n");
        while (1);
    }
    LoRa.enableCrc();
}

int check_sensor(uint32_t id)
{
    switch(id) {
        case SENSOR1_ID:
            return 1;

        case SENSOR2_ID:
            return 2;

        default:
            LOG("Wrong sensor ID: %x\n", id);
            break;
    }
    return 0;
}

void loop()
{
    static unsigned long previousMillis = millis();
    static uint32_t dispOn;
    struct LoraMsg loraMsg;
    int sensor;
    unsigned long currentMillis = millis();

    NetworkTick();

    // Wait for packet
    int packetSize = LoRa.parsePacket();
    if (packetSize == sizeof(loraMsg)) {
        // received a packet
        uint8_t *p = (uint8_t *)&loraMsg;
        for (int i = 0; i < packetSize; i++) {
            p[i] = (uint8_t)LoRa.read();
        }
        int rssi = LoRa.packetRssi();
        float snr = LoRa.packetSnr();

        if ((sensor = check_sensor(loraMsg.id)) > 0) {
            LOG("Sensor      = %d\n", sensor);
            LOG("Counter     = %d\n", loraMsg.seq);
            LOG("Temperature = %d\n", loraMsg.temperature);
            LOG("Humidity    = %d\n", loraMsg.humidity);  // %
            LOG("Vbat        = %d\n", loraMsg.vbat);      // mv
            LOG("runtime     = %d\n", loraMsg.runtime);   // ms
            LOG("illuminance = %d\n", loraMsg.illuminance);

            DisplayLoraMsg(&loraMsg, rssi);
            digitalWrite(BOARD_LED, LED_ON);
            dispOn = currentMillis;

            NetworkPublish(sensor, loraMsg.seq, loraMsg.temperature, loraMsg.humidity, loraMsg.vbat,
                rssi, snr, loraMsg.runtime, loraMsg.illuminance);
        }
    }
    else {
        if (currentMillis - dispOn > 15000) {
            DisplayOff();
            digitalWrite(BOARD_LED, LED_OFF);
        }
    }
}
