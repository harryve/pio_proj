//#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <FastLED.h>
#include <time.h>
#include "hwdefs.h"
#include "alarm.h"
#include "button.h"
#include "cred.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char* ssid = SSID;
const char* password = PASSWORD;
const char* ntpServer = "ntp.harry.thuis";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

static int brightness = 1;

CRGB leds[NUM_LEDS];

Alarm alarmBuzzer = Alarm(BUZZER_PIN);

// 5x8 (from HelvetiPixel.ttf) font for digits
const uint8_t digits[10][8] = {
  {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110}, // 0
  {0b00010, 0b00110, 0b01010, 0b00010, 0b00010, 0b00010, 0b00010, 0b00010}, // 1
  {0b01110, 0b10001, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111}, // 2
  {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b00001, 0b10001, 0b01110}, // 3
  {0b00001, 0b00011, 0b00101, 0b01001, 0b10001, 0b11111, 0b00001, 0b00001}, // 4
  {0b11111, 0b10000, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110}, // 5
  {0b01110, 0b10001, 0b10000, 0b11110, 0b10001, 0b10001, 0b10001, 0b01110}, // 6
  {0b11111, 0b00001, 0b00010, 0b00010, 0b00100, 0b00100, 0b01000, 0b01000}, // 7
  {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110}, // 8
  {0b01110, 0b10001, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b01110}  // 9
};

void button2_event(Button::Event event)
{
  //using Button::Event;
  if (event == Button::Event::SHORT_PRESS) {
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
  pinMode(LDR_PIN, INPUT);

  //pinMode(27, INPUT_PULLUP);
  //pinMode(26, INPUT_PULLUP);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

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

void setLED(int x, int y, CRGB color) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return;
  int index = (y % 2 == 0) ? y * MATRIX_WIDTH + x : (y + 1) * MATRIX_WIDTH - 1 - x;
  leds[index] = color;
}

void drawDigit(int x, int y, int digit, CRGB color) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 5; col++) {
      if (digits[digit][row] & (1 << (4 - col))) {
        setLED(x + col, y + row, color);
      }
    }
  }
}

void drawTime(int hours, int minutes, CRGB color) 
{
  // 5 1 5 3 5 1 5
  // 0   6   14  20
  int x = 3;
  drawDigit(x + 0, 0, hours / 10, color);
  drawDigit(x + 6, 0, hours % 10, color);
  setLED(x + 12, 2, color);
  setLED(x + 12, 5, color);
  drawDigit(x + 14, 0, minutes / 10, color);
  drawDigit(x + 20, 0, minutes % 10, color);
}

void ReadLdr()
{
  static unsigned long prevMillis = 0;

  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis > 60000) {
    prevMillis = currentMillis;

    uint16_t val = analogRead(LDR_PIN);

    mqttClient.beginMessage("tele/wekker/sensor");
    char buf[16];
    buf[snprintf(buf, sizeof(buf) - 1, "%d", val)] = '\0';
    mqttClient.print(buf);
    mqttClient.endMessage();

    Serial.printf("LDR = %d\n", val);
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
        FastLED.setBrightness(brightness);
        FastLED.show();
      }
      Serial.printf("Brightness = %d %d\n", brightness, digitalRead(BUTTON_SELECT_PIN));
    }
    if (c == 'd') {
      if (brightness > 0) {
        brightness--;
        FastLED.setBrightness(brightness);
        FastLED.show();
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
    if (dispTime == 600) {
      alarmBuzzer.trigger();
    }
    Serial.printf("Time = %d\n", dispTime);
    FastLED.clear();
    drawTime(timeinfo.tm_hour, timeinfo.tm_min, CRGB::Red);
    FastLED.show();
  }
  //delay(100);
}
