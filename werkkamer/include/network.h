#pragma once
#include <Arduino.h>

void NetworkInit();
void NetworkLoop();
int32_t NetworkSignalStrength();

void NetworkPublish(float temperature, float humidity, float pressure, uint32_t signalStrength);
