/**
  * @file           rubik_bit1_config.c
  * @version        1.0
  * @date           2021-5-25
  */

#ifndef __RUBIK_BIT1_CONFIG_H_
#define __RUBIK_BIT1_CONFIG_H_

#include "driver/gpio.h"
#include "driver/i2c.h"

// I2C0
#define RUBIK_BIT1_I2C0_SDA_IO                    GPIO_NUM_21
#define RUBIK_BIT1_I2C0_SCL_IO                    GPIO_NUM_22
#define RUBIK_BIT1_I2C0_I2C_NUM                   I2C_NUM_0

// BEEP
#define RUBIK_BIT1_BUZZER_IO                      GPIO_NUM_25 

// SK6812
#define RUBIK_BIT1_SK6812_IO                      GPIO_NUM_5
#define RUBIK_BIT1_SK6812_LEN                     8

// LED
#define RUBIK_BIT1_LED_IO                         GPIO_NUM_19

// MPU9250
#define RUBIK_BIT1_MPU9250_SDA_IO                 RUBIK_BIT1_I2C0_SDA_IO
#define RUBIK_BIT1_MPU9250_SCL_IO                 RUBIK_BIT1_I2C0_SCL_IO
#define RUBIK_BIT1_MPU9250_INT_IO                 GPIO_NUM_39
#define RUBIK_BIT1_MPU9250_I2C_NUM                RUBIK_BIT1_I2C0_I2C_NUM
#define RUBIK_BIT1_MPU9250_I2C_ADDR               0X68

#endif /* __RUBIK_BIT1_CONFIG_H_ */