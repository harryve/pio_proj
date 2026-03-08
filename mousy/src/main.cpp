
#include <Arduino.h>
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI


#define LED_DI_PIN     40
#define LED_CI_PIN     39

#define TFT_CS_PIN     4
#define TFT_SDA_PIN    3
#define TFT_SCL_PIN    5
#define TFT_DC_PIN     2
#define TFT_RES_PIN    1
#define TFT_LEDA_PIN   38

#define SD_MMC_D0_PIN  14
#define SD_MMC_D1_PIN  17
#define SD_MMC_D2_PIN  21
#define SD_MMC_D3_PIN  18
#define SD_MMC_CLK_PIN 12
#define SD_MMC_CMD_PIN 16

TFT_eSPI tft = TFT_eSPI();

uint32_t targetTime = 0;       // for next 1 second timeout

byte omm = 99;
bool initial = 1;
byte xcolon = 0;
unsigned int colour = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello T-Dongle-S3");
    pinMode(TFT_LEDA_PIN, OUTPUT);
    // Initialise TFT
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(TFT_LEDA_PIN, 0);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour

    targetTime = millis() + 1000;
}



void loop()
{
    static uint8_t hh = 12;
    static uint8_t mm = 0;
    static uint8_t ss = 0;

    if (targetTime < millis()) {
        targetTime = millis()+1000;
        ss++;              // Advance second
        if (ss==60) {
            ss=0;
            omm = mm;
            mm++;            // Advance minute
            if(mm>59) {
                mm=0;
                hh++;          // Advance hour
                if (hh>23) {
                    hh=0;
                }
            }
        }

        if (ss==0 || initial) {
            initial = 0;
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            //tft.setCursor (8, 52);
            //tft.print(__DATE__); // This uses the standard ADAFruit small font
            tft.drawString(__DATE__, 8, 54, 4);
            //tft.setTextColor(TFT_BLUE, TFT_BLACK);
            //tft.drawCentreString("It is windy",120,48,2); // Next size up font 2

            //tft.setTextColor(0xF81F, TFT_BLACK); // Pink
            //tft.drawCentreString("12.34",80,100,6); // Large font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 . : a p m
        }

        // Update digital time
        byte xpos = 6;
        byte ypos = 0;
        if (omm != mm) { // Only redraw every minute to minimise flicker
            // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
            tft.setTextColor(0x39C4, TFT_BLACK);  // Leave a 7 segment ghost image, comment out next line!
            //tft.setTextColor(TFT_BLACK, TFT_BLACK); // Set font colour to black to wipe image
            // Font 7 is to show a pseudo 7 segment display.
            // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
            tft.drawString("88:88",xpos,ypos,7); // Overwrite the text to clear it
            tft.setTextColor(0xFBE0); // Orange
            omm = mm;

            if (hh<10) xpos+= tft.drawChar('0',xpos,ypos,7);
            xpos+= tft.drawNumber(hh,xpos,ypos,7);
            xcolon=xpos;
            xpos+= tft.drawChar(':',xpos,ypos,7);
            if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
            tft.drawNumber(mm,xpos,ypos,7);
        }

        if (ss%2) { // Flash the colon
            tft.setTextColor(0x39C4, TFT_BLACK);
            xpos+= tft.drawChar(':',xcolon,ypos,7);
            tft.setTextColor(0xFBE0, TFT_BLACK);
        }
        else {
            tft.drawChar(':',xcolon,ypos,7);
            // colour = random(0xFFFF);
            // // Erase the old text with a rectangle, the disadvantage of this method is increased display flicker
            // tft.fillRect (0, 64, 160, 20, TFT_BLACK);
            // tft.setTextColor(colour);
            // tft.drawRightString("Colour",75,64,4); // Right justified string drawing to x position 75
            // String scolour = String(colour,HEX);
            // scolour.toUpperCase();
            // char buffer[20];
            // scolour.toCharArray(buffer,20);
            // tft.drawString(buffer,82,64,4);
        }
    }
}

#ifdef JAJA
void loop()   // Put your main code here, to run repeatedly:
{
    static uint8_t i;
    switch (i++) {
    case 0:
        Serial.println("0");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawString("Hallo", 0, 0, 1);
        break;

    case 1:
        Serial.println("1");
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("Test", 0, 0, 2);
        break;

    case 2:
        Serial.printf("2 -> %d\n", tft.textsize);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        //tft.setTextSize(tft.textsize + 1);
        tft.drawString("Dat was het", 0, 0, 4);
        break;

    case 3:
        Serial.printf("2 -> %d\n", tft.textsize);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        //tft.setTextSize(tft.textsize + 1);
        tft.drawString("23:59", 0, 0, 6);
        break;

    case 4:
        Serial.printf("2 -> %d\n", tft.textsize);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        //tft.setTextSize(tft.textsize + 1);
        tft.drawString("08:36", 0, 0, 7); // Dit is de leukste!
        break;

    case 5:
        Serial.printf("2 -> %d\n", tft.textsize);
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        //tft.setTextSize(tft.textsize + 1);
        tft.drawString("12:34", 0, 0, 8);
        i = 0;
        break;

    default:
        break;
    }
    delay(2000);
}
#endif