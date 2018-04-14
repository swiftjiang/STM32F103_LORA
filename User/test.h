#ifndef __TEST_H__
#define __TEST_H__
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

#include "usually.h"
#include "hardware.h"
#include "app_scheduler.h"
#include "timer.h"
#include "lora.h"

//usart2 DMA÷–∂œ≤‚ ‘
void test_get_data(uint8_t *data,uint8_t len);
void test_parse(void *data,uint16_t len);

//LoRa≤‚ ‘
#define MAIN_MENU "\r\n\
+--------------------+\r\n\
|                    |\r\n\
|    LoRa testing    |\r\n\
|                    |\r\n\
|--------------------|\r\n\
|--------------------|\r\n"
#define DEVICE_TYPE_MASTER "|*******MASTER*******|\r\n|--------------------|\r\n+--------------------+\r\n"
#define DEVICE_TYPE_SLAVER "|*******SLAVER*******|\r\n|--------------------|\r\n+--------------------+\r\n\r\n"
void init_LoRa(void);
void testLoRa(void);

//timer≤‚ ‘
void testTimer(void);
void timerHandler(void *arg);


//MBUS≤‚ ‘ 
void MBUS_Send5Bytes(void);
#endif
