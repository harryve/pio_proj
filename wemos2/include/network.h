#pragma once
#include <Arduino.h>

void NetworkInit();
void NetworkLoop();
int32_t NetworkSignalStrength();

void NetworkPublish(float temperature, uint32_t signalStrength);
