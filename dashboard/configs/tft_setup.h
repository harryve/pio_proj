// Note: perform clean build after changing this file!
//
// Setup for the ESP32 S3 with ILI9341 display
// Note SPI DMA with ESP32 S3 is not currently supported
#define USER_SETUP_ID 70
// See SetupX_Template.h for all options available
#define ILI9341_DRIVER

//#define TFT_WIDTH   320
//#define TFT_HEIGHT  240

#define TFT_CS   10
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_MISO 13

// Use pins in range 0-31
#define TFT_DC    2
#define TFT_RST   8

//#define TOUCH_CS 16 // Optional for touch screen

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

// FSPI (or VSPI) port (SPI2) used unless following defined. HSPI port is (SPI3) on S3.
#define USE_HSPI_PORT

#define SPI_FREQUENCY    32000000

#define SPI_READ_FREQUENCY 32000000
