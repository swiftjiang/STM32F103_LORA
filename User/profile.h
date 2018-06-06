#ifndef __PROFILE_H__
#define __PROFILE_H__


#include "lora.h"
#include "mbus.h"
#include "gprs.h"

#include "debug.h"
#include "loraSet.h"

#define LORA_MASTER_DATA        "MASTER"
#define LORA_SLAVER_DATA        "SLAVER"
#define GPRS_DATA               "GPRS"
#define MBUS_DATA               "MBUS"

#define HEARTBEAT_INTERVAL 5000



void Profie_get_data(char* tag,void* data,uint8_t len);

void LoRaHeartbeat(void);





#endif
