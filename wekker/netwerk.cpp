#include <Arduino.h>
//#include <FastLED.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
//#include <Wire.h>

#include <time.h>
//#include "hwdefs.h"
//#include "display.h"
//#include "alarm.h"
//#include "button.h"
//#include "ldr.h"
#include "cred.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char* ssid = SSID;
const char* password = PASSWORD;
const char* ntpServer = "ntp.harry.thuis";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;