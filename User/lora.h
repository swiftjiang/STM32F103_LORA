#ifndef __LORA_H__
#define __LORA_H__
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"

#include "hardware.h"
#include "app_scheduler.h"
#include "loraSet.h"
#include "uart.h"
#include "debug.h"
#include "profile.h"

#define MODE0 0x00
#define MODE1 0x01
#define MODE2 0x02
#define MODE3 0x03




//uint8_t LORA_RECV_DATA[64]={0};
/*
typedef struct WorkingParameter
{
	uint8_t header_B0;
	uint8_t addH_B1;
	uint8_t addL_B2;
	uint8_t opt1_B3;
	uint8_t channel_B4;
	uint8_t opt2_B5;
}WorkingParamTypeDef;

typedef struct LoRaPacket
{
	//uint16_t add;
	//uint8_t chanbel;
	uint8_t leadbyte;
	uint8_t *data;
}LoRaPacketTypeDef;
*/
//WorkingParamTypeDef GET_PARAM;
//WorkingParamTypeDef SEND_PARAM;

/*
typedef void (*callback)(uint8_t *data,uint8_t len);

typedef enum
{
	GPRS = 0x01,
	MBUS = 0x02
}LoRaRegisterTypedef;

typedef struct LoRa_callback
{
	callback GPRS_callback;
	callback MBUS_callback;
	
}LoRa_callbackTypedef;
*/


void LoRa_Init(void);
void LoRa_InitParameter(void);
void LoRa_SetMode(uint8_t mode);


void LoRa_GetParameter(void);//返回6字节数据  uint8_t *WorkParam
void LoRa_GetVersionInfo(void);//返回4字节数据  uint8_t *WorkParam
void LoRa_Reset(void);

void LoRa_SendCMD(uint8_t *data,uint8_t len);
void LoRa_SendPacket(uint8_t *data,uint8_t len);
void LoRa_RecvPacket(uint8_t *data,uint8_t len);
void LoRa_PacketParse(void *data,uint16_t len);

/*
void LoRa_register_callback(callback cb,LoRaRegisterTypedef type);
*/
#endif
