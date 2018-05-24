#ifndef __SYS_TICK_TIMER_H__
#define __SYS_TICK_TIMER_H__

#include "stm32f10x.h"
#include "core_cm3.h"   //uint32_t SysTick_Config(uint32_t ticks)
#include "misc.h"       //void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource)



/*----第一部分是---向滴答时钟寄存器写初值，也就是要根据系统频率来递减----然后为0 则产生中断*/

void SysTick_Init(uint32_t a);   //a-us

void PreciseDelay_us(__IO u32 nTime);

void PreciseDelay_ms(__IO u32 nTime);

void TimingDelay_Decrement(void);   //节拍递减---在中断中使用

#endif


/*
void PreciseDelay_us(__IO uint32_t us);
void PreciseDelay_ms(__IO uint32_t ms);
void PreciseDelay_s(__IO uint32_t s);
*/

