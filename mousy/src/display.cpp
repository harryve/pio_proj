#include <Arduino.h>
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI

#include "display.h"

#define TFT_CS_PIN     4
#define TFT_SDA_PIN    3
#define TFT_SCL_PIN    5
#define TFT_DC_PIN     2
#define TFT_RES_PIN    1
#define TFT_LEDA_PIN   38

static TFT_eSPI tft = TFT_eSPI();

void DisplayInit()
{
    // Initialise TFT
    pinMode(TFT_LEDA_PIN, OUTPUT);
    digitalWrite(TFT_LEDA_PIN, 1);
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_LEDA_PIN, 0);  // Back light on

    tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour

}

void DisplayTerminal(const char *pText)
{
    static int line = 0;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(pText, 0, line * 20, 2);
    line++;
}

void DisplayDate(int year, int month, int day)
{
    int xpos = 8;
    int ypos = 54;

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    xpos += tft.drawNumber(year, xpos, ypos, 4);
    xpos += tft.drawChar('/', xpos, ypos, 4);
    xpos += tft.drawNumber(month, xpos, ypos, 4);
    xpos += tft.drawChar('/', xpos, ypos, 4);
    xpos += tft.drawNumber(day, xpos, ypos, 4);
}

void DisplayTime(int hh, int mm, int ss)
{
    static int omm = 99;
    static bool initial = true;
    static int xcolon = 0;

    // if (initial) {
    //     initial = false;
    //     tft.fillScreen(TFT_BLACK);
    //     tft.setTextColor(TFT_GREEN, TFT_BLACK);
    //     tft.drawString(__DATE__, 8, 54, 4);
    // }

    // Update digital time
    int xpos = 6;
    int ypos = 0;
    if (omm != mm) { // Only redraw every minute to minimise flicker
        tft.setTextColor(0x39C4, TFT_BLACK);  // Leave a 7 segment ghost image
        tft.drawString("88:88",xpos,ypos,7);  // Overwrite the text to clear it
        tft.setTextColor(0xFBE0); // Orange
        omm = mm;

        if (hh < 10) {
            xpos += tft.drawChar('0', xpos, ypos, 7);
        }
        xpos += tft.drawNumber(hh, xpos, ypos, 7);

        xcolon = xpos;
        xpos += tft.drawChar(':', xpos, ypos, 7);
        if (mm < 10) {
            xpos += tft.drawChar('0', xpos, ypos, 7);
        }
        tft.drawNumber(mm, xpos, ypos, 7);
    }

    if (ss % 2) { // Flash the colon
        tft.setTextColor(0x39C4, TFT_BLACK);
        tft.drawChar(':', xcolon, ypos, 7);
        tft.setTextColor(0xFBE0);
    }
    else {
        tft.drawChar(':', xcolon, ypos, 7);
    }
}
