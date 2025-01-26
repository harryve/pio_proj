#include <Arduino.h>

#include "alarmclock.h"
#include "setalarmts.h"
#include "hwdefs.h"
#include "display.h"
#include "alarm.h"
#include "button.h"
#include "ldr.h"
#include "network.h"
#include "webserver.h"

const char* ntpServer = "ntp.harry.thuis";
//const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

static CRGB color = CRGB::Red;
static int red = 255;
static int green = 0;
static int blue = 0;
static int brightness = 1;

AlarmClock alarmClock = AlarmClock();
SetAlarmTs setAlarmTs = SetAlarmTs();
Display *handlers[] = {&alarmClock, &setAlarmTs};
int mode = 0;

Alarm alarmBuzzer = Alarm(BUZZER_PIN);

void ButtonHandler(Button::Id id, Button::Event event)
{
  switch (event) {
    case Button::Event::SHORT_PRESS:    Serial.printf("%d: SP\n", (int)id); break;
    case Button::Event::LONG_PRESS:     Serial.printf("%d: LP\n", (int)id); break;
    case Button::Event::LONG_PRESS_END: Serial.printf("%d: LE\n", (int)id); break;
    default: Serial.printf("%d: ???\n", (int)id); break;
  }

  if (event == Button::Event::SHORT_PRESS && id == Button::Id::LEFT) {
    mode = 1 - mode;
    handlers[mode]->Redraw();
  }
  else {
    handlers[mode]->ButtonHandler(id, event);
  }
}

Button button1 = Button(BUTTON_LEFT_PIN, Button::Id::LEFT, ButtonHandler);
Button button2 = Button(BUTTON_SELECT_PIN, Button::Id::MID, ButtonHandler);
Button button3 = Button(BUTTON_RIGHT_PIN, Button::Id::RIGHT, ButtonHandler);

void setup()
{
  Serial.begin(115200);

  LdrInit();

  alarmClock.init(brightness, color);

  NetworkInit();
  WebserverInit();

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Wait for time sync
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    delay(100);
  }
}

#define PUBLISH_TIME 60000   // ms
#define SAMPLE_TIME  1000    // ms
void ReadLdr()
{
  static int count = 0;
  int val;

  if (LdrRead(&val, SAMPLE_TIME)) {

    //Serial.printf("LDR = %d\n", val);

    brightness = val / 50;
    if (brightness > 35) {
      brightness = 35;
    }
    if (brightness < 1) {
      brightness = 1;
    }
    Serial.printf("LDR brightness = %d\n", brightness);
    handlers[mode]->SetBrightness(brightness);

    int notRed = (val * 250) / 3000;
    if (notRed > 250) {
      notRed = 250;
    }
    color = CRGB(255, notRed, notRed);
    alarmClock.SetColor(color);

    if (count++ >= PUBLISH_TIME / SAMPLE_TIME) {
      count = 0;
      NetworkPublishLdr(val);
      NetworkPublishBrightness(brightness);
    }
  }
}

static void NewColor()
{
  Serial.printf("R=%d, G=%d, B=%d\n", red, green, blue);
  color = CRGB(red, green, blue);
  alarmClock.SetColor(color);
}

static void Monitor()
{
  char c = Serial.read();
  if (c > 0x20 && c < 0x80) {
    //Serial.printf("Received [%c]\n", c);
    if (c == 'u') {
      if (brightness < 100) {
        brightness++;
        alarmClock.SetBrightness(brightness);
        //DisplayRedrawTime();
      }
      Serial.printf("Brightness = %d %d\n", brightness, digitalRead(BUTTON_SELECT_PIN));
    }
    if (c == 'd') {
      if (brightness > 0) {
        brightness--;
        alarmClock.SetBrightness(brightness);
        //DisplayRedrawTime();
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
}

void loop()
{
  static int dispTime = -1;
  struct tm timeinfo;
  bool invert;

  NetworkTick();
  WebserverTick();

  if (alarmBuzzer.tick(invert)) {
    alarmClock.Invert(invert);
  }
  button1.Tick();
  button2.Tick();
  button3.Tick();
  ReadLdr();

  if (getLocalTime(&timeinfo)) {
    //if (dispTime != timeinfo.tm_sec) { //((timeinfo.tm_hour * 100) + timeinfo.tm_min)) {
    //  dispTime = timeinfo.tm_sec; //(timeinfo.tm_hour * 100) + timeinfo.tm_min;
    if (dispTime != ((timeinfo.tm_hour * 100) + timeinfo.tm_min)) {
      dispTime = (timeinfo.tm_hour * 100) + timeinfo.tm_min;

      if ((dispTime == 600) && alarmClock.AlarmActive()) {
        alarmBuzzer.trigger();
      }
      Serial.printf("Time = %d\n", dispTime);
      alarmClock.SetTime(timeinfo.tm_hour, timeinfo.tm_min);
    }
  }

  //alarmClock.Tick();
  handlers[mode]->Tick();
  Monitor();

}
