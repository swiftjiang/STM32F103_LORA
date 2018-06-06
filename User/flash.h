#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"  
#include "stm32f10x_flash.h"
#include "uart.h"


/*
uint32_t ReadFlash(uint32_t offset);

void WriteFlash(uint32_t offset, uint32_t Data);
*/

/****************************************************************  
*Name:      ReadFlashNBtye  
*Function:  从内部Flash读取N字节数据  
*Input:     ReadAddress：数据地址（偏移地址）ReadBuf：数据指针  ReadNum：读取字节数  
*Output:    读取的字节数    
*Author:    ValerianFan  
*Date:      2014/04/09  
*E-Mail:    fanwenjingnihao@163.com  
*Other:       
****************************************************************/  
uint32_t ReadFlashNBtye(uint32_t Offset, uint8_t *ReadBuf, uint32_t len);

uint32_t WriteFlashNWord(uint32_t Offset,uint8_t *Data,uint32_t len); 

#endif
