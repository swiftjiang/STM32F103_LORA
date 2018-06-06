#ifndef __LORA_MASTER_SLAVER_H__
#define __LORA_MASTER_SLAVER_H__

#define MASTER 1
//#define SLAVER 1

#define LORA_STATUS 

#ifdef MASTER
#define GPRS  
#define ADDR  		0x0001
#define CHANNEL 	0x17
#endif

#ifdef SLAVER
#define MBUS  
#define ADDR  		0x0000
#define CHANNEL 	0x17
#endif

#endif
