#pragma once

#if SHOW_LOG
#define LOG(...) Serial.printf(__VA_ARGS__)
#else
#define LOG(...)
#endif
