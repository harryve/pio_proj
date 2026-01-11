#include <Arduino.h>
#include "display.h"

#define I2C_ADDRESS 0x3c  // SH1106 I2C address

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // QT-PY / XIAO

Display::Display()
{
    pDriver = new Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    pDriver->begin(I2C_ADDRESS, true);    // Address 0x3C default
    pDriver->setContrast (0);             // dim display
    pDriver->clearDisplay();
    pDriver->setTextSize(3);
    pDriver->setTextColor(SH110X_WHITE);
    pDriver->setCursor(0, 0);
    pDriver->print("Klokkie");
    pDriver->setTextSize(1);
    pDriver->setCursor(0, 40);
    pDriver->print(__DATE__);
    pDriver->setCursor(0, 50);
    pDriver->print(__TIME__);
    pDriver->display();
}

void Display::Off()
{
    pDriver->clearDisplay();
    pDriver->display();
}

void Display::Show(int dispTimePercentage, bool synced)
{
    pDriver->clearDisplay();
    pDriver->setTextSize(4);
    pDriver->setTextColor(SH110X_WHITE);
    pDriver->setCursor(0, 0);
    pDriver->printf("%2d:%02d", hour, minute);

    pDriver->drawFastHLine(0, 37, (SCREEN_WIDTH * dispTimePercentage) / 100, SH110X_WHITE);

    pDriver->setTextSize(2);
    pDriver->setCursor(0, 43);
    if (synced) {
        pDriver->printf("%02d", second);
    }
    else {
        pDriver->print("??");
    }
    pDriver->printf("  %.1f", temperature);
    pDriver->display();
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
