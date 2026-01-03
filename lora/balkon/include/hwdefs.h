#pragma once

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
#define NSS       GPIO_NUM_8        // not Slave select (active low)
#define SCK       GPIO_NUM_9
#define MOSI      GPIO_NUM_10
#define MISO      GPIO_NUM_11

// Radio pins
#define LORA_RST  GPIO_NUM_12
#define LORA_BUSY GPIO_NUM_13
#define LORA_DIO1 GPIO_NUM_14

// Sensor connection
#define I2C_SDA                     33
#define I2C_SCL                     34

// ADC input for detection of connected charger
#define CHARGE_ADC          GPIO_NUM_3

