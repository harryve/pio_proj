//#include <Arduino.h>
#include <FastLED.h>
//#include <ArduinoMqttClient.h>
//#include <WiFi.h>
//#include <Wire.h>

//#include <time.h>
#include "hwdefs.h"
#include "display.h"
#include "alarm.h"
#include "button.h"
#include "ldr.h"
#include "network.h"

//WiFiClient wifiClient;
//MqttClient mqttClient(wifiClient);

//const char* ssid = SSID;
//const char* password = PASSWORD;
const char* ntpServer = "ntp.harry.thuis";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

static CRGB color = CRGB::Red;
static int red = 255;
static int green = 0;
static int blue = 0;
static int brightness = 1;
static bool alarmActive = false;


Alarm alarmBuzzer = Alarm(BUZZER_PIN);

void button2_event(Button::Event event)
{
  using enum Button::Event;
  if (event == SHORT_PRESS) {
    alarmActive = alarmActive ? false : true;
    DisplaySetAlarmActive(alarmActive);
    Serial.println("SP");
  }
  else {
    Serial.println("LP");
  }
}

Button button2 = Button(BUTTON_SELECT_PIN, button2_event);

void setup() 
{
  Serial.begin(115200);
  LdrInit();

  DisplayInit();
  DisplaySetBrightness(brightness);
  DisplaySetColor(color);
  DisplaySetAlarmActive(alarmActive);

  NetworkInit();

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org");//ntpServer);
}


void ReadLdr()
{
  int val;

  if (LdrRead(&val)) {
    NetworkPublishLdr(val);

    Serial.printf("LDR = %d\n", val);

    brightness = val / 50;
    if (brightness > 35) {
      brightness = 35;
    }
    if (brightness < 1) {
      brightness = 1;
    }
    Serial.printf("LDR brightness = %d\n", brightness);
    DisplaySetBrightness(brightness);

    int notRed = (val * 250) / 3000;
    if (notRed > 250) {
      notRed = 250;
    }
    color = CRGB(255, notRed, notRed);
    DisplaySetColor(color);

    DisplayRedrawTime();

    NetworkPublishBrightness(brightness);
  }  
}

static void NewColor()
{
  Serial.printf("R=%d, G=%d, B=%d\n", red, green, blue);
  color = CRGB(red, green, blue);
  DisplaySetColor(color);
}

void loop() 
{
  static int dispTime = -1;
  struct tm timeinfo;

  NetworkTick();
  alarmBuzzer.tick();
  button2.Tick();
  ReadLdr();

  char c = Serial.read();
  if (c != 0xff) {
    Serial.printf("Received [%c]\n", c);
    if (c == 'u') {
      if (brightness < 100) {
        brightness++;
        DisplaySetBrightness(brightness);
        DisplayRedrawTime();
      }
      Serial.printf("Brightness = %d %d\n", brightness, digitalRead(BUTTON_SELECT_PIN));
    }
    if (c == 'd') {
      if (brightness > 0) {
        brightness--;
        DisplaySetBrightness(brightness);
        DisplayRedrawTime();
      }
      Serial.printf("Brightness = %d\n", brightness);
    }
    if (c == 'r') { if (red   < 255) { red++;   NewColor(); }}
    if (c == 'R') { if (red   > 0)   { red--;   NewColor(); }}
    if (c == 'g') { if (green < 255) { green++; NewColor(); }}
    if (c == 'G') { if (green > 0)   { green--; NewColor(); }}
    if (c == 'b') { if (blue  < 255) { blue++;  NewColor(); }}
    if (c == 'B') { if (blue  > 0)   { blue--;  NewColor(); }}

    if (c == 'a') {
      alarmBuzzer.trigger();
    }
  }

  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    DisplayDrawTime(0, 0);
    return;
  }

  if (dispTime != ((timeinfo.tm_hour * 100) + timeinfo.tm_min)) {
    dispTime = (timeinfo.tm_hour * 100) + timeinfo.tm_min;
    if (alarmActive && (dispTime == 600)) {
      alarmBuzzer.trigger();
    }
    Serial.printf("Time = %d\n", dispTime);
    DisplayDrawTime(timeinfo.tm_hour, timeinfo.tm_min);
  }
  //delay(100);
}
