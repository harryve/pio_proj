#pragma once
#if __has_include("cred_harry.h")
#   include "cred_harry.h"
#else
const char* WIFI_SSID = "wifi";
const char* WIFI_PASSWORD = "password";

const char* MQTT_BROKER = "mqtt";
const int MQTT_PORT = 1;

const char* TIME_SERVER_ADDR = "time";
const int TIME_SERVER_PORT = 1;


#endif
