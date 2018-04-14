#ifndef __UART_H__
#define __UART_H__
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"



//void DEBUDINFO(char* str,char* info);


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART1
调试串口
RX-->PA9
TX-->PA10
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint8_t Data);
void USART1_Send_String(uint8_t* Data,uint8_t len);
void USART1_Receive_Byte(uint8_t len);
void Init_Usart1(void);
void Usart1_Configuration(uint32_t BaudRate); 
void Init_NVIC_USART1(void);
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART2
LoRa数据端口
RX-->PA3
TX-->PA2
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


void USART2_Send_Byte(uint8_t Data);
void USART2_Send_String(uint8_t* Data,uint8_t len);
void USART2_Receive_DMA(uint8_t* data,uint8_t len);
void Init_Usart2(void);
void Usart2_Configuration(uint32_t BaudRate); 
void Init_NVIC_USART2(void);
void Init_DMA_USART2(void);
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART3

RX-->PB11
TX-->PB10
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART3_Send_Byte(uint8_t Data);
void USART3_Send_String(uint8_t* Data,uint8_t len);
void USART3_Receive_DMA(uint8_t* data,uint8_t len);
void Init_Usart3(void);
void Usart3_Configuration(uint32_t BaudRate); 
void Init_NVIC_USART3(void);
void Init_DMA_USART3(void);
#endif
