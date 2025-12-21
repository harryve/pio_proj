/*
  cc1101_drv.cpp - CC1101 module library
  Copyright (c) 2010 Michael.
    Author: Michael, <www.elechouse.com>
    Version: November 12, 2010

  This library is designed to use CC1101/CC1100 module on Arduino platform.
  CC1101/CC1100 module is an useful wireless module.Using the functions of the
  library, you can easily send and receive data by the CC1101/CC1100 module.
  Just have fun!
  For the details, please refer to the datasheet of CC1100/CC1101.
----------------------------------------------------------------------------------------------------------------
cc1101 Driver for RC Switch. Mod by Little Satan. With permission to modify and publish Wilson Shen (ELECHOUSE).
----------------------------------------------------------------------------------------------------------------

Aangepast door Harry, zodat ik er mijn KAKU bel drukker mee kan ontvangen.
Er kan nog een hoop verbeterd worden aan deze code, maar het werkt nu...
*/
#include <Arduino.h>
#include <SPI.h>
#include "cc1101_drv.h"

/****************************************************************/
#define   READ_SINGLE       0x80            //read single
#define   READ_BURST        0xC0            //read burst
#define   BYTES_IN_RXFIFO   0x7F            //byte number in RXfifo

// Deze variabelen moeten verdwijnen, zijn nergens goed voor.
byte modulation = 2;
byte frend0;
byte SCK_PIN;
byte MISO_PIN;
byte MOSI_PIN;
byte SS_PIN;
byte gdo_set=0;
bool spi = 0;
bool ccmode = 0;
float MHz = 433.92;
byte m4RxBw = 0;
byte m4DaRa;
byte m2DCOFF;
byte m2MODFM;
byte m2MANCH;
byte m2SYNCM;
byte m1FEC;
byte m1PRE;
byte m1CHSP;
byte pc1PQT;
byte pc1CRC_AF;
byte pc1APP_ST;
byte pc1ADRCHK;
byte pc0WDATA;
byte pc0PktForm;
byte pc0CRC_EN;
byte pc0LenConf;
byte clb1[2]= {24,28};
byte clb2[2]= {31,38};
byte clb3[2]= {65,76};
byte clb4[2]= {77,79};

/****************************************************************
*FUNCTION NAME:SpiStart
*FUNCTION     :spi communication start
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SpiStart(void)
{
    // initialize the SPI pins
    pinMode(SCK_PIN, OUTPUT);
    pinMode(MOSI_PIN, OUTPUT);
    pinMode(MISO_PIN, INPUT);
    pinMode(SS_PIN, OUTPUT);

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
}

/****************************************************************
*FUNCTION NAME:SpiEnd
*FUNCTION     :spi communication disable
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SpiEnd(void)
{
    // disable SPI
    SPI.endTransaction();
    SPI.end();
}

/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::Reset (void)
{
    digitalWrite(SS_PIN, LOW);
    delay(1);
    digitalWrite(SS_PIN, HIGH);
    delay(1);

    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));

    SPI.transfer(CC1101_SRES);
    while(digitalRead(MISO_PIN));

    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::Init(void)
{
    setSpi();
    SpiStart();                   //spi initialization
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(MOSI_PIN, LOW);
    Reset();                    //CC1101 reset
    RegConfigSettings();            //CC1101 register config
    SpiEnd();
}

/****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SpiWriteReg(byte addr, byte value)
{
    SpiStart();
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SPI.transfer(addr);
    SPI.transfer(value);
    digitalWrite(SS_PIN, HIGH);
    SpiEnd();
}

/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer define in CC1101.h//
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SpiStrobe(byte strobe)
{
    SpiStart();
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SPI.transfer(strobe);
    digitalWrite(SS_PIN, HIGH);
    SpiEnd();
}

/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte CC1101_drv::SpiReadReg(byte addr)
{
    byte temp, value;
    SpiStart();
    temp = addr| READ_SINGLE;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SPI.transfer(temp);
    value=SPI.transfer(0);
    digitalWrite(SS_PIN, HIGH);
    SpiEnd();
    return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
    byte i, temp;

    SpiStart();
    temp = addr | READ_BURST;
    digitalWrite(SS_PIN, LOW);

    while(digitalRead(MISO_PIN));

    SPI.transfer(temp);
    for(i = 0; i < num; i++) {
        buffer[i ]= SPI.transfer(0);
    }
    digitalWrite(SS_PIN, HIGH);
    SpiEnd();
}

/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte CC1101_drv::SpiReadStatus(byte addr)
{
    byte value,temp;

    SpiStart();
    temp = addr | READ_BURST;
    digitalWrite(SS_PIN, LOW);

    while(digitalRead(MISO_PIN));

    SPI.transfer(temp);
    value = SPI.transfer(0);
    digitalWrite(SS_PIN, HIGH);
    SpiEnd();
    return value;
}

/****************************************************************
*FUNCTION NAME:SPI pin Settings
*FUNCTION     :Set Spi pins
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setSpi(void)
{
    if (spi == 0){
         SCK_PIN = 18;
         MISO_PIN = 19;
         MOSI_PIN = 23;
         SS_PIN = 5;
    }
}

/****************************************************************
*FUNCTION NAME:COSTUM SPI
*FUNCTION     :set costum spi pins.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setSpiPin(byte sck, byte miso, byte mosi, byte ss)
{
    spi = 1;
    SCK_PIN = sck;
    MISO_PIN = miso;
    MOSI_PIN = mosi;
    SS_PIN = ss;
}

/****************************************************************
*FUNCTION NAME:CCMode
*FUNCTION     :Format of RX and TX data
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setCCMode(bool s)
{
    ccmode = s;
    SpiWriteReg(CC1101_IOCFG2,      0x0E);      // Carrier detect
    SpiWriteReg(CC1101_IOCFG0,      0x0D);      // Serial data out
    SpiWriteReg(CC1101_PKTCTRL0,    0x32);
    SpiWriteReg(CC1101_MDMCFG3,     0x93);
    SpiWriteReg(CC1101_MDMCFG4, 7+m4RxBw);
    setModulation(modulation);
}

/****************************************************************
*FUNCTION NAME:Modulation
*FUNCTION     :set CC1101 Modulation
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setModulation(byte m)
{
    if (m>4) {
        m=4;
    }
    modulation = m;
    Split_MDMCFG2();
    switch (m)
    {
        case 0: m2MODFM=0x00; frend0=0x10; break; // 2-FSK
        case 1: m2MODFM=0x10; frend0=0x10; break; // GFSK
        case 2: m2MODFM=0x30; frend0=0x11; break; // ASK
        case 3: m2MODFM=0x40; frend0=0x10; break; // 4-FSK
        case 4: m2MODFM=0x70; frend0=0x10; break; // MSK
    }
    SpiWriteReg(CC1101_MDMCFG2, m2DCOFF+m2MODFM+m2MANCH+m2SYNCM);
    SpiWriteReg(CC1101_FREND0,   frend0);
}

/****************************************************************
*FUNCTION NAME:Frequency Calculator
*FUNCTION     :Calculate the basic frequency.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setMHZ(float mhz)
{
    byte freq2 = 0;
    byte freq1 = 0;
    byte freq0 = 0;

    MHz = mhz;

    for (bool i = 0; i==0;) {
        if (mhz >= 26) {
            mhz-=26;
            freq2+=1;
        }
        else if (mhz >= 0.1015625) {
            mhz-=0.1015625;
            freq1+=1;
        }
        else if (mhz >= 0.00039675) {
            mhz-=0.00039675;
            freq0+=1;
        }
        else {
            i=1;
        }
    }
    if (freq0 > 255) {
        freq1+=1;freq0-=256;
    }

    SpiWriteReg(CC1101_FREQ2, freq2);
    SpiWriteReg(CC1101_FREQ1, freq1);
    SpiWriteReg(CC1101_FREQ0, freq0);

    //Calibrate();
}

/****************************************************************
*FUNCTION NAME:getCC1101
*FUNCTION     :Test Spi connection and return 1 when true.
*INPUT        :none
*OUTPUT       :none
****************************************************************/
bool CC1101_drv::getCC1101(void)
{
    setSpi();
    if (SpiReadStatus(0x31)>0) {
        return 1;
    }
    return 0;
}

/****************************************************************
*FUNCTION NAME:Set Receive bandwidth
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setRxBW(float f)
{
    Split_MDMCFG4();
    int s1 = 3;
    int s2 = 3;
    for (int i = 0; i<3; i++) {
        if (f > 101.5625) {
            f/=2;
            s1--;
        }
        else {
            i=3;
        }
    }
    for (int i = 0; i<3; i++) {
        if (f > 58.1) {
            f/=1.25;
            s2--;
        }
        else {
            i=3;
        }
    }
    s1 *= 64;
    s2 *= 16;
    m4RxBw = s1 + s2;
    SpiWriteReg(16,m4RxBw+m4DaRa);
}

/****************************************************************
*FUNCTION NAME:Set Data Rate
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::setDRate(float d)
{
    Split_MDMCFG4();
    float c = d;
    byte MDMCFG3 = 0;
    if (c > 1621.83) {
        c = 1621.83;
    }
    if (c < 0.0247955) {
        c = 0.0247955;
    }
    m4DaRa = 0;
    for (int i = 0; i<20; i++) {
        if (c <= 0.0494942) {
            c = c - 0.0247955;
            c = c / 0.00009685;
            MDMCFG3 = c;
            float s1 = (c - MDMCFG3) *10;
            if (s1 >= 5) {
                MDMCFG3++;
            }
            i = 20;
        }
        else {
            m4DaRa++;
            c = c/2;
        }
    }
    SpiWriteReg(16,  m4RxBw+m4DaRa);
    SpiWriteReg(17,  MDMCFG3);
}

/****************************************************************
*FUNCTION NAME:Split MDMCFG2
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::Split_MDMCFG2(void)
{
    int calc = SpiReadStatus(18);
    m2DCOFF = 0;
    m2MODFM = 0;
    m2MANCH = 0;
    m2SYNCM = 0;
    for (bool i = 0; i==0;) {
        if (calc >= 128) {
            calc-=128;
            m2DCOFF+=128;
        }
        else if (calc >= 16) {
            calc-=16;
            m2MODFM+=16;
        }
        else if (calc >= 8) {
            calc-=8;
            m2MANCH+=8;
        }
        else {
            m2SYNCM = calc;
            i=1;
        }
    }
}

/****************************************************************
*FUNCTION NAME:Split MDMCFG4
*FUNCTION     :none
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::Split_MDMCFG4(void)
{
    int calc = SpiReadStatus(16);
    m4RxBw = 0;
    m4DaRa = 0;
    for (bool i = 0; i==0;) {
        if (calc >= 64) {
            calc-=64;
            m4RxBw+=64;
        }
        else if (calc >= 16) {
            calc -= 16;
            m4RxBw+=16;
        }
        else {
            m4DaRa = calc;
            i=1;
        }
    }
}

/****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::RegConfigSettings(void)
{
    SpiWriteReg(CC1101_FSCTRL1,  0x06);

    setCCMode(ccmode);
    setMHZ(MHz);

    SpiWriteReg(CC1101_MDMCFG1,  0x02);
    SpiWriteReg(CC1101_MDMCFG0,  0xF8);
    SpiWriteReg(CC1101_CHANNR,   0);
    SpiWriteReg(CC1101_DEVIATN,  0x47);
    SpiWriteReg(CC1101_FREND1,   0x56);
    SpiWriteReg(CC1101_MCSM0 ,   0x18);
    SpiWriteReg(CC1101_FOCCFG,   0x16);
    SpiWriteReg(CC1101_BSCFG,    0x1C);
    SpiWriteReg(CC1101_AGCCTRL2, 0xC7);
    SpiWriteReg(CC1101_AGCCTRL1, 0x00);
    SpiWriteReg(CC1101_AGCCTRL0, 0xB2);
    SpiWriteReg(CC1101_FSCAL3,   0xE9);
    SpiWriteReg(CC1101_FSCAL2,   0x2A);
    SpiWriteReg(CC1101_FSCAL1,   0x00);
    SpiWriteReg(CC1101_FSCAL0,   0x1F);
    SpiWriteReg(CC1101_FSTEST,   0x59);
    SpiWriteReg(CC1101_TEST2,    0x81);
    SpiWriteReg(CC1101_TEST1,    0x35);
    SpiWriteReg(CC1101_TEST0,    0x09);
    SpiWriteReg(CC1101_PKTCTRL1, 0x04);
    SpiWriteReg(CC1101_ADDR,     0x00);
    SpiWriteReg(CC1101_PKTLEN,   0x00);
}

/****************************************************************
*FUNCTION NAME:SetRx
*FUNCTION     :set CC1101 to receive state
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void CC1101_drv::SetRx(void)
{
    SpiStrobe(CC1101_SIDLE);
    SpiStrobe(CC1101_SRX);        //start receive
}
