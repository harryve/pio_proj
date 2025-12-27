//Board: TTGO LoRa32-OLED

//#include <math.h>
#include <Arduino.h>
#include <LoRa.h>

#include "network.h"
#include "hwdefs.h"
#include "loramsg.h"

#include "display.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("initBoard....");
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);

    pinMode(BOARD_LED, OUTPUT);
    digitalWrite(BOARD_LED, LED_ON);

    DisplayInit();

    NetworkInit();

    Serial.println("LoRa Bridge " __DATE__ ", " __TIME__);
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
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
            Serial.printf("Wrong sensor ID: %x\n", id);
            break;
    }
    return 0;
}

void loop()
{
    static unsigned long previousMillis = millis();
    struct LoraMsg loraMsg;
    static uint32_t dispOn;
    int sensor;
    unsigned long currentMillis = millis();

    // if WiFi is down, try reconnecting
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
            Serial.printf("Sensor      = %d\n", sensor);
            Serial.printf("Counter     = %d\n", loraMsg.seq);
            Serial.printf("Temperature = %d\n", loraMsg.temperature);
            Serial.printf("Humidity    = %d\n", loraMsg.humidity);        // %
            Serial.printf("Vbat        = %d\n", loraMsg.vbat);   // mv
            Serial.printf("runtime     = %d\n", loraMsg.runtime);   // ms
            Serial.printf("illuminance = %d\n", loraMsg.illuminance);

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
