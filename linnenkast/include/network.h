#pragma once

void NetworkInit();
void NetworkLoop();
int32_t NetworkSignalStrength();

void NetworkPublish(int door, int light, int32_t signalStrength);
