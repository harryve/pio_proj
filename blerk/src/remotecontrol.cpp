#include <Arduino.h>
#include "remotecontrol.h"

#define SERIAL_DATA_PIN 4

#define IDLE    0
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

#define T_TIME          250
#define T_START_MIN     (10*T_TIME - 100 + T_TIME)
#define T_START_MAX     (10*T_TIME + 100 + T_TIME)
#define T_SHORT_MIN     (T_TIME - 50 + T_TIME)
#define T_SHORT_MAX     (T_TIME + 50 + T_TIME)
#define T_LONG_MIN      (5 * T_TIME - 150 + T_TIME)
#define T_LONG_MAX      (5 * T_TIME + 150 + T_TIME)
#define EXPECTED_CODE   0x0b1fb4b8
#define MIN_CODE_COUNT  10

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
        if (interruptCount == 0) {
            intAnalyze[interruptCount].micros = 0;
        }
        else {
            intAnalyze[interruptCount].micros = micros() - prevMicros;
        }
        interruptCount++;
    }
    prevMicros = micros();
}

void RemoteControlInit()
{
    pinMode(SERIAL_DATA_PIN, INPUT);
    Serial.println("Rx Mode");

    attachInterrupt(SERIAL_DATA_PIN, DataHandler, RISING);
}

static bool Decode()
{
    int state = IDLE;
    int i = 0;
    int bit;
    uint32_t pattern;
    int found_remote_control = 0;

    while (i < interruptCount) {
        switch(state) {
            case IDLE:
                if (intAnalyze[i].micros > T_START_MIN && intAnalyze[i].micros < T_START_MAX) {
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
                if (intAnalyze[i].micros > T_SHORT_MIN && intAnalyze[i].micros < T_SHORT_MAX) {
                    state = HANDLE_0_BIT;
                }
                else if (intAnalyze[i].micros > T_LONG_MIN && intAnalyze[i].micros < T_LONG_MAX) {
                    state = HANDLE_1_BIT;
                }
                else {
                    //Serial.printf("Invalid bit length of first part of bit = %d, length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                break;

            case HANDLE_0_BIT:
                //Serial.printf("Handle bit 0 at %d %u\n", i, intAnalyze[i].micros);
                if (intAnalyze[i].micros > T_LONG_MIN && intAnalyze[i].micros < T_LONG_MAX) {
                    // Ok
                    state = START_OF_BIT;
                }
                else {
                    //Serial.printf("Invalid bit length of second part of 0 bit (%d), length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                bit++;
                break;

            case HANDLE_1_BIT:
                //Serial.printf("Handle bit 1 at %d %u\n", i, intAnalyze[i].micros);
                if (intAnalyze[i].micros > T_SHORT_MIN && intAnalyze[i].micros < T_SHORT_MAX) {
                    pattern |= (1 << bit);
                    state = START_OF_BIT;
                }
                else {
                    //Serial.printf("Invalid bit length of second part of 1 bit (%d), length = %u\n", bit, intAnalyze[i].micros);
                    state = IDLE;
                    break;
                }
                i++;
                bit++;
                break;
        }
        if (bit == 32) {
            //Serial.printf("%08x\n", pattern);
            state = IDLE;
            // The remote sends the same code for about 20 times
            if (pattern == EXPECTED_CODE) {
                found_remote_control++;
                if (found_remote_control > MIN_CODE_COUNT) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool RemoteControlCheck()
{
    static unsigned long prevMillis = 0;
    static unsigned long prevCount = 0;
    static int state = 0;
    bool res = false;

    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis > 100) {
        prevMillis = currentMillis;
        if (prevCount != interruptCount) {
            //Serial.println(interruptCount);
            prevCount = interruptCount;
        }
        else {
            if (interruptCount > 0) {
                detachInterrupt(SERIAL_DATA_PIN);
                res = Decode();
                interruptCount = 0;
                prevCount = 0;
                attachInterrupt(SERIAL_DATA_PIN, DataHandler, RISING);
            }
        }
    }
    return res;
}
