#pragma once

// 'PRG' Button
#ifndef BUTTON
#define BUTTON    GPIO_NUM_0
#endif
// LED pin & PWM parameters
#define LED_PIN   GPIO_NUM_35
#define LED_FREQ  5000
#define LED_CHAN  0
#define LED_RES   8
// External power control
#define VEXT      GPIO_NUM_36
// Battery voltage measurement
#define VBAT_CTRL GPIO_NUM_37
#define VBAT_ADC  GPIO_NUM_1
// SPI pins
#define SS        GPIO_NUM_8
#define MOSI      GPIO_NUM_10
#define MISO      GPIO_NUM_11
#define SCK       GPIO_NUM_9
// Radio pins
#define DIO1      GPIO_NUM_14
#define RST_LoRa  GPIO_NUM_12
#define BUSY_LoRa GPIO_NUM_13
