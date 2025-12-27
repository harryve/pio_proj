#pragma once

void NetworkInit();
void NetworkTick();
void NetworkPublish(int sensor, uint16_t seq, int16_t temperature, int8_t humidity, int16_t vbat,
    int rssi, float snr, uint16_t runtime,  uint16_t illuminance);
