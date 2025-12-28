#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "log.h"
#include "display.h"

static U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2 = nullptr;

void DisplayInit()
{
    Wire.beginTransmission(0x3C);
    if (Wire.endTransmission() == 0) {
        LOG("Started OLED\n");
        u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFlipMode(0);
        u8g2->setFontMode(1); // Transparent
        u8g2->setDrawColor(1);
        u8g2->setFontDirection(0);
        u8g2->firstPage();
        u8g2->setFont(u8g2_font_fur11_tf);
        do {
            u8g2->drawStr(0, 20, "Harry LoRa");
            u8g2->drawStr(0, 40, __DATE__);
            u8g2->drawStr(0, 60, __TIME__);
        } while ( u8g2->nextPage() );
        u8g2->sendBuffer();
        u8g2->setFont(u8g2_font_fur11_tf);
        delay(5000);
    }
}

void DisplaySleep()
{
    if (u8g2) {
        u8g2->sleepOn();
    }
}

void DisplayShowMeasurement(float temp, float hum)
{
    if (u8g2) {
        char buf[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Transmitting: OK!");
        snprintf(buf, sizeof(buf), "T=%.1f,  RV=%.0f", temp, hum);
        u8g2->drawStr(0, 30, buf);
        u8g2->sendBuffer();
        delay(5000);
    }
}
