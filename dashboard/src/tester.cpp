#include <Arduino.h>
#include "tester.h"
#include "monutil.h"
#include "display.h"
#include "mandelbrot.h"

static void PanicTest(int argc, char *argv[])
{
    Serial.write("The quick brown fox jumps over the lazy dog\n");
}

static void AssertTest(int argc, char *argv[])
{
    for (int i = 0; i < 20; i++) {
        Serial.write("The quick brown fox jumps over the lazy dog\n");
    }
}

static void DoMandelBrot(int argc, char *argv[])
{
    int maxIter;

    if (argc == 1) {
        MandelBrot(128);
    }
    else {
        if (!xatoi(argv[1], &maxIter)) {
            Serial.printf("Invalid max iteration value\n");
            return;
        }
        MandelBrot(maxIter);
    }
}

static void DoDisplay(int argc, char *argv[])
{
    int x, y, font;

    if (!hatoi(argv[1], &x)) {
        Serial.printf("Invalid X\n");
        return;
    }

    if (!hatoi(argv[2], &y)) {
        Serial.printf("Invalid Y\n");
        return;
    }

    if (!xatoi(argv[3], &font)) {
        Serial.printf("Invalid font\n");
        return;
    }

    DisplayText(x, y, font, argv[4]);

}

Cmds cmdTable[] = {
        { "assert",     AssertTest,         0, 0, "(Test TlAssert functionality)" },
        { "display",    DoDisplay,          4, 4, "x y font text"},
        { "help",       DoHelp,             0, 0, "(This help menu)" },
        { "mandelbrot", DoMandelBrot,       0, 1, "My first Mandelbrot" },
        { "memory",     DoMemDump,          0, 2, "[addr] [length]" },
        { "panic",      PanicTest,          0, 1, "[message] (Panic function test)" },
        { "?",          DoHelp,             0, 0, "(Short help menu)" },
        { NULL,         NULL,               0, 0, NULL }       // Denotes end of list
};

static void CharOut(char c)
{
    Serial.print(c);
    //Serial.flush();
}

static char cmdBuf[256];
static bool GetCmd()
{
    char c;

    for (int i = 0; i < (int) (sizeof(cmdBuf) - 1);) {
        if ((c = Serial.read()) > 0 && c < 0x80) {
            if (c == '\r' || c == '\n') {
                CharOut('\n');
                return true;
            }
            else if (c == 0x08) {
                if (i > 0) {
                    cmdBuf[--i] = '\0';
                    CharOut(c);
                }
            }
            else {
                if (c >= 0x20 && c < 0x7f) {
                    cmdBuf[i++] = c;
                    cmdBuf[i] = '\0';
                    CharOut(c);
                }
            }
        }
    }
    return false;
}

void TesterInit()
{
    cmdBuf[0] = '\0';
    Serial.print("> ");
}

void TesterTick()
{
    if (Serial.available()) {
        if (GetCmd()) {
            HdlCmd(cmdTable, cmdBuf);
        }
        cmdBuf[0] = '\0';
        Serial.print("> ");
    }
}

//void TesterStart()
//{
//    xTaskCreate(Tester, "tester", 4096, NULL, 2, NULL);
//}
