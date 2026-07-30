#include <Arduino.h>
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI

#include "display.h"

#define TFT_LEDA_PIN   5

static TFT_eSPI tft = TFT_eSPI();

void DisplayInit()
{
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
    tft.setRotation(1);
    Serial.println("Fill screen");
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_LEDA_PIN, 1);  // Back light on

    //tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour

}

void DisplayTerminal(const char *pText)
{
    static int line = 0;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(pText, 0, line * 20, 2);
    line++;
}

