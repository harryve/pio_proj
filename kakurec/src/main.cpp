#include <Arduino.h>
#include <HARRY_CC1101_SRC_DRV.h>

#define RGB_BRIGHTNESS 64 // Change white brightness (max 255)
#define SERIAL_DATA_PIN 4

static unsigned long interruptCount = 0;
typedef struct {
//    int state;
    unsigned long micros;
} TintAnalyze;

#define TRACE_LEN   4096
static TintAnalyze intAnalyze[TRACE_LEN];

static void DataHandler()
{
    static unsigned long prevMicros;

    if (interruptCount < TRACE_LEN) {
        // int state = digitalRead(SERIAL_DATA_PIN);
        // if (state == 1) {
        //     intAnalyze[interruptCount].state = state;
            if (interruptCount == 0) {
                intAnalyze[interruptCount].micros = 0;
            }
            else {
                intAnalyze[interruptCount].micros = micros() - prevMicros;
            }
            interruptCount++;
//        }
    }
    prevMicros = micros();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    delay(1000);
    Serial.println("Hello world!");

    HARRY_cc1101.setSpiPin(12, 13, 11, 2);
    if (HARRY_cc1101.getCC1101()) {         // Check the CC1101 Spi connection.
        Serial.println("Connection OK");
    } else {
        Serial.println("Connection Error");
    }

    HARRY_cc1101.Init();              // must be set to initialize the cc1101!
    //HARRY_cc1101.setGDO0(2);
    HARRY_cc1101.setDRate(4.0);
    HARRY_cc1101.setRxBW(16);
    HARRY_cc1101.setCCMode(0);
    HARRY_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    HARRY_cc1101.setMHZ(433.92);   // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    HARRY_cc1101.SetRx();
//    HARRY_cc1101.setSyncMode(0);  // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
//    HARRY_cc1101.setCrc(0);      // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
//    HARRY_cc1101.setDRate(8);

#ifdef JAJA
    HARRY_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    HARRY_cc1101.setMHZ(433.92); // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    HARRY_cc1101.SetRx();       // set Receive on
#endif

    pinMode(SERIAL_DATA_PIN, INPUT);
    Serial.println("Rx Mode");

    attachInterrupt(SERIAL_DATA_PIN, DataHandler, RISING); //CHANGE);
}

static void Blink()
{
    static unsigned long prevMillis = 0;
    static int state = 0;

    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 500) {
        prevMillis = currentMillis;
        switch (state) {
            case 0:
                neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); // Green
                //Serial.println("Test");
                state = 1;
                break;
            case 1:
                neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); // Blue
                state = 2;
                break;
            case 2:
                neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); // Red
                state = 3;
                break;
            default:
                neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Off
                state = 0;
                break;
        }
    }
}

#define IDLE    0
//#define PATTERN 1
#define START_OF_BIT    1
#define HANDLE_0_BIT    2
#define HANDLE_1_BIT    3

/*
 * Protocol:
 *          _   _
 *    '0': | |_| |_____  T,T,T,5T
 *          _       _
 *    '1': | |_____| |_  T,5T,T,T
 *          _
 *  Start: | |__________ T,10T
 *          _
 *  Stop:  | |______//__ T,40T
 *
 * T = 250 us. Measure time between two rising edges.
 */

static void Decode()
{
    int state = IDLE;
    int i = 0;
    int bit;
    //uint32_t pattern[2];
    uint32_t pattern;

    while (i < interruptCount) {
        switch(state) {
            case IDLE:
                if (intAnalyze[i].micros > 2400+250 && intAnalyze[i].micros < 2600+250) {
                    //Serial.printf("Start at %d %u\n", i, intAnalyze[i].micros);
                    //Serial.println();
                    state = START_OF_BIT;
                    pattern = 0;
                    bit = 0;
                }
                i++;
                break;

            case START_OF_BIT:
                //Serial.printf("Start of bit at %d %u\n", i, intAnalyze[i].micros);
                if (intAnalyze[i].micros > 200+250 && intAnalyze[i].micros < 300+250) {
                    state = HANDLE_0_BIT;
                }
                else if (intAnalyze[i].micros > 1200+250 && intAnalyze[i].micros < 1400+250) {
                    state = HANDLE_1_BIT;
                }
                else {
                    Serial.printf("Invalid bit length of first part of bit = %d, length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                break;

            case HANDLE_0_BIT:
                //Serial.printf("Handle bit 0 at %d %u\n", i, intAnalyze[i].micros);
                if (intAnalyze[i].micros > 1200+250 && intAnalyze[i].micros < 1400+250) {
                    // Ok
                    state = START_OF_BIT;
                }
                else {
                    Serial.printf("Invalid bit length of second part of 0 bit (%d), length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                bit++;
                break;

            case HANDLE_1_BIT:
                //Serial.printf("Handle bit 1 at %d %u\n", i, intAnalyze[i].micros);
                if (intAnalyze[i].micros > 200+250 && intAnalyze[i].micros < 300+250) {
                    pattern |= (1 << bit);
                    state = START_OF_BIT;
                }
                else {
                    Serial.printf("Invalid bit length of second part of 1 bit (%d), length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                bit++;
                break;
        }
        if (bit == 32) {
            Serial.printf("%08x\n", pattern);
            state = IDLE;
        }

            //     if (intAnalyze[i].micros > 9000 && intAnalyze[i + 1].micros > 2400 && intAnalyze[i + 1].micros < 2600) {
            //         //Serial.printf("Start at %d\n", i);
            //         //Serial.println();
            //         state = PATTERN;
            //         i += 2;
            //         bit = 0;
            //         memset(pattern, 0, sizeof(pattern));
            //     }
            //     else {
            //         i++;
            //     }
            //     break;

            // case PATTERN:
            //     if (intAnalyze[i].micros > 200 && intAnalyze[i].micros < 300) {
            //         // Ok
            //     }
            //     else if (intAnalyze[i].micros > 1200 && intAnalyze[i].micros < 1300) {
            //         pattern[bit / 32] |= (1 << (bit % 32));
            //     }
            //     else {
            //         Serial.printf("Invalid bit length (bit = %d, length = %u\n", bit, intAnalyze[i].micros);
            //         state = IDLE;
            //         break;
            //     }
            //     i++;
            //     bit++;
            //     if (bit == sizeof(pattern) * 8) {
            //         Serial.printf("%08x %08x\n", pattern[0], pattern[1]);
            //         state = IDLE;
            //     }
            //     break;
        //}
    }
}
static void DataChecker()
{
    static unsigned long prevMillis = 0;
    static unsigned long prevCount = 0;
    static int state = 0;

    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 500) {
        prevMillis = currentMillis;
        if (prevCount != interruptCount) {
            Serial.println(interruptCount);
            prevCount = interruptCount;
        }
        else {
            if (interruptCount > 0) {
                Decode();
                // for (int i = 0; i < interruptCount; i++) {
                //     Serial.printf("%2d %d %u\n", i, intAnalyze[i].state, intAnalyze[i].micros);
                // }
                interruptCount = 0;
                prevCount = 0;
            }
        }
    }
}

void loop()
{
    Blink();
    DataChecker();
}
