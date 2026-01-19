#pragma once

#if SHOW_LOG
#define LOG_BEGIN    Serial.begin(115200);
#define LOG(...) Serial.printf(__VA_ARGS__)
#else
#define LOG(...)
#define LOG_BEGIN
#endif
