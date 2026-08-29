#include <Arduino.h>
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI

#include "display.h"

#define TFT_LEDA_PIN   5

#define USE_DISPLAY

#ifdef USE_DISPLAY
TFT_eSPI tft = TFT_eSPI();
#endif

void DisplayInit()
{
#ifdef USE_DISPLAY
    Serial.println("Initialise TFT");

    // Initialise TFT
    pinMode(TFT_LEDA_PIN, OUTPUT);
    digitalWrite(TFT_LEDA_PIN, 0);

    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, 0);
    delay(10);
    digitalWrite(TFT_RST, 1);
    Serial.println("Init");
    tft.init();
    Serial.println("setRotation");
    tft.setRotation(3);
    Serial.println("Fill screen");
    tft.fillScreen(TFT_BLACK);

    digitalWrite(TFT_LEDA_PIN, 1);  // Back light on
#endif
}

void DisplayTerminal(const char *pText)
{
#ifdef USE_DISPLAY
    static int line = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(pText, 0, line * 20, 2);
    line++;
#endif
}

void DisplayText(int x, int y, int font, const char *pText)
{
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(pText, x, y, font);
}
