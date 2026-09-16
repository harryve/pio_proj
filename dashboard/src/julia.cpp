// Based on sketch here:
// https://github.com/OpenHDZ/Arduino-experimentation
// Adapted for TFT_eSPI library

// Note: a high number of floating point calculations are needed
// for each pixel so rendering will be quite slow.
// For best performance use a Teensy 4.x (600MHz CPU clock).

#include <TFT_eSPI.h>       // Hardware-specific library
#include "julia.h"

extern TFT_eSPI tft;

const uint16_t MAX_ITERATION = 300; // Nombre de couleurs

#define SCREEN_WIDTH  tft.width()  //
#define SCREEN_HEIGHT tft.height() // Taille de l'écran

//static float zoom = 0.5;

// void setup() {
//   tft.begin();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);
//   tft.setFreeFont(&FreeMono9pt7b);
// }

static void draw_Julia(float c_r, float c_i, float zoom) {

  tft.setCursor(0,0);
  float new_r = 0.0, new_i = 0.0, old_r = 0.0, old_i = 0.0;

  for(int16_t x = SCREEN_WIDTH/2 - 1; x >= 0; x--) { // Rely on inverted symmetry
    for(uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
      old_r = 1.5 * (x - SCREEN_WIDTH / 2) / (0.5 * zoom * SCREEN_WIDTH);
      old_i = (y - SCREEN_HEIGHT / 2) / (0.5 * zoom * SCREEN_HEIGHT);
      uint16_t i = 0;

      while ((old_r * old_r + old_i * old_i) < 4.0 && i < MAX_ITERATION) {
        new_r = old_r * old_r - old_i * old_i ;
        new_i = 2.0 * old_r * old_i;

        old_r = new_r+c_r;
        old_i = new_i+c_i;

        i++;
      }
      /* Affiche le pixel */
      if (i < 100){
        tft.drawPixel(x,y,tft.color565(255,255,map(i,0,100,255,0)));
        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(255,255,map(i,0,100,255,0)));
      }if(i<200){
        tft.drawPixel(x,y,tft.color565(255,map(i,100,200,255,0),0));
        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(255,map(i,100,200,255,0),0));
      }else{
        tft.drawPixel(x,y,tft.color565(map(i,200,300,255,0),0,0));
        tft.drawPixel(SCREEN_WIDTH - x - 1,SCREEN_HEIGHT - y - 1,tft.color565(map(i,200,300,255,0),0,0));
      }
    }
  }
}

void julia(float zoom)
{
    draw_Julia(-0.8,+0.156,zoom);
    // tft.fillRect(0, 0, 150, 20, TFT_BLACK);
    // tft.setCursor(0,15);
    // tft.setTextColor(TFT_WHITE);
    // tft.print(" Zoom = ");
    // tft.println(zoom);
}

