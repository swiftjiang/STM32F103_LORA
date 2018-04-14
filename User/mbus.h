#ifndef __MBUS_H__
#define __MBUS_H__

#include "app_scheduler.h"
#include "loraSet.h"
#include "lora.h"
#include "uart.h"
#include "debug.h"

void MBUS_RecvPacket(uint8_t *data,uint8_t len);
void MBUS_PacketParse(void *data,uint16_t len);


//void MBUS_SendDATA();

#endif
