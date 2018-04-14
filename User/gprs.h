#ifndef __GPRS_H__
#define __GPRS_H__

#include "app_scheduler.h"
#include "loraSet.h"
#include "uart.h"
#include "debug.h"

void GPRS_RecvPacket(uint8_t *data,uint8_t len);
void GPRS_PacketParse(void *data,uint16_t len);





#endif
