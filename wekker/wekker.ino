//#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <Wire.h>

#include <time.h>
#include "hwdefs.h"
#include "display.h"
#include "alarm.h"
#include "button.h"
#include "ldr.h"
#include "cred.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char* ssid = SSID;
const char* password = PASSWORD;
const char* ntpServer = "ntp.harry.thuis";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

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
  DisplaySetAlarmActive(alarmActive);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  mqttClient.setId("wekker");
  Serial.print("Attempting to connect to the MQTT broker: ");
  if (!mqttClient.connect("mqtt.harry.thuis", 1883)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
  }
  else {
    Serial.println("Connected to the MQTT broker!");
    Serial.println();
  }

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


void ReadLdr()
{
  int val;
  if (LdrRead(&val)) {
    mqttClient.beginMessage("tele/wekker/sensor");
    char buf[16];
    buf[snprintf(buf, sizeof(buf) - 1, "%d", val)] = '\0';
    mqttClient.print(buf);
    mqttClient.endMessage();

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
  }  
}

void loop() 
{
  static int dispTime = -1;
  struct tm timeinfo;

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
      }
      Serial.printf("Brightness = %d %d\n", brightness, digitalRead(BUTTON_SELECT_PIN));
    }
    if (c == 'd') {
      if (brightness > 0) {
        brightness--;
        DisplaySetBrightness(brightness);
      }
      Serial.printf("Brightness = %d\n", brightness);
    }
    if (c == 'a') {
      alarmBuzzer.trigger();
    }
  }

  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
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
