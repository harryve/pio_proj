#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "timesync.h"


#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long spm;
static void DisplayTime(unsigned long spm, bool synced)
{
  char buffer[64];

  int h = spm / (60 * 60);
  int m = (spm / 60) % 60;
  int s = spm % 60;

  display.setTextSize(4);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  snprintf(buffer, sizeof(buffer), "%2d:%02d", h, m);
  display.println(buffer);

  display.setTextSize(2);
  display.setCursor(0, 40);
  snprintf(buffer, sizeof(buffer), "%02d ", s);
  display.print(buffer);
  if (synced) {
    display.print(" S");
  }
  else {
    display.print(" ?");
  }
  display.display();
}

void setup()
{
  spm = 0;
  Serial.begin(115200);
  Serial.println("\n\nStart clock");

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.setContrast (0); // dim display
 
  display.display();

  TimeSyncInit();

  delay(1000);
  Serial.println("Setup complete");
}


void loop()
{
  bool synced;
  static unsigned long syncTimestamp = 0;
  static unsigned long offset = 0;
  static unsigned long lastTime = 0;

  display.clearDisplay();

  unsigned long newSpm;
  if (TimeSync(newSpm, synced)) {
    spm = newSpm;
    syncTimestamp = millis();
    offset = 0;
  }
  else {
    offset = ((millis() - syncTimestamp) / 1000);
  }

  if (lastTime != spm + offset) {
    lastTime = spm + offset;
    DisplayTime(spm + offset, synced);
    Serial.print("D");
  }

  delay(100);
}
