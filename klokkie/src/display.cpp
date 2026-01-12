#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "display.h"

#define I2C_ADDRESS 0x3c  // SH1106 I2C address

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // QT-PY / XIAO

static Adafruit_SH1106G driver(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display()
{
}

void Display::Init()
{
    driver.begin(I2C_ADDRESS, true);    // Address 0x3C default
    driver.setContrast (0);             // dim display
    driver.clearDisplay();
    driver.setTextSize(3);
    driver.setTextColor(SH110X_WHITE);
    driver.setCursor(0, 0);
    driver.print("Klokkie");
    driver.setTextSize(1);
    driver.setCursor(0, 40);
    driver.print(__DATE__);
    driver.setCursor(0, 50);
    driver.print(__TIME__);
    driver.display();
}

void Display::Off()
{
    driver.clearDisplay();
    driver.display();
}

void Display::Show(int dispTimePercentage, bool synced)
{
    driver.clearDisplay();
    driver.setTextSize(4);
    driver.setTextColor(SH110X_WHITE);
    driver.setCursor(0, 0);
    driver.printf("%2d:%02d", hour, minute);

    driver.drawFastHLine(0, 37, (SCREEN_WIDTH * dispTimePercentage) / 100, SH110X_WHITE);

    driver.setTextSize(2);
    driver.setCursor(0, 43);
    if (synced) {
        driver.printf("%02d", second);
    }
    else {
        driver.print("??");
    }
    driver.printf("  %.1f", temperature);
    driver.display();
}

void Display::SetTime(int hour, int minute, int second)
{
    this->hour = hour;
    this->minute = minute;
    this->second = second;
}

void Display::SetTemperature(float temperature)
{
    this->temperature = temperature;
}
