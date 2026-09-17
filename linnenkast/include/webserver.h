#pragma once

void WebServerInit();
void WebServerLoop();

void WebServerPublish(int door, int light, int32_t sigStrength);
