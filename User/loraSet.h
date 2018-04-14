#ifndef __LORA_MASTER_SLAVER_H__
#define __LORA_MASTER_SLAVER_H__

#define MASTER 1
//#define SLAVER 1

#ifdef MASTER
#define GPRS
#endif

#ifdef SLAVER
#define MBUS
#endif

#endif
