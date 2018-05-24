#include "sys_tick_timer.h"



//----第一部分---向滴答时钟寄存器写初值，也就是要根据系统频率来递减----然后为0 则产生中断
uint32_t TimingDelay = 0;
void SysTick_Init(uint32_t a)   //a-us
{
/* SystemFrequency / 1000          1ms中断一次
* SystemFrequency / 100000       10us中断一次
* SystemFrequency / 1000000     1us中断一次
*/
	if (SysTick_Config(SystemCoreClock / 1000000)*a)  //写初值----ST3.5.0库版本
	{
		while (1);   //写初值成功---则进入while死循环-等待滴答时钟的中断到来
	}
  SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;  //如果向滴答时钟写初值失败---则关闭系统滴答时钟
}
//----第二部分---延时函数----
void PreciseDelay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;//多少个基础节拍
  SysTick_Init(1);  //1us
  SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;  //使能系统滴答时钟---开始计时
  while(TimingDelay != 0);  //定时节拍结束
}

void PreciseDelay_ms(__IO u32 nTime)
{
	TimingDelay = nTime;//多少个基础节拍
  SysTick_Init(1000);  //1ms
  SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;  //使能系统滴答时钟---开始计时
  while(TimingDelay != 0);  //定时节拍结束
}

//---第三部分---滴答时钟的中断函数---

void TimingDelay_Decrement(void)   //节拍递减---在中断中使用
{
	if (TimingDelay != 0x00)
		TimingDelay--;
}

void SysTick_Handler(void)  //---中断---
{
	TimingDelay_Decrement(); //进一次中断递减一次
}

//static uint8_t TIME_OUT = 0;
//static uint8_t SYS_TICK_DIV8 = 9;
/**
  * SysTick时钟源由HCLK/8提供，当系统频率为72MHz时，最小计数周期为1/9MHz,计满9次为1us，
  * tick_us以9为单位，在此为基础实现us延时。
  */
 /*
SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
tick_us = sysclk/8;    // sysyclk = 72          
tick_ms=(u16)tick_us*1000;
*/
/*
void PreciseDelay_us(__IO uint32_t us)
{
    if(us > 1864135)//24位计数器的最大装载值为 0XFFFFFF = 16777215，8分频的时候，9次震荡为1us，故参数最大值为 0XFFFFFF/9 = 1864135
        return;
		
    uint32_t tick  = us*SYS_TICK_DIV8;
    SysTick_Config(tick);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    while(!TIME_OUT)
    {
        TIME_OUT = 0;
        break;
    }
		
}

void PreciseDelay_ms(__IO uint32_t ms)
{
    uint32_t i;
    for(i=0;i<ms;i++)
    {
        PreciseDelay_us(1000);
    }
}

void PreciseDelay_s(__IO uint32_t s)
{
    uint32_t i;
    for(i=0;i<s;i++)
    {
        PreciseDelay_ms(1000);
    }
}
*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*
void SysTick_Handler(void)
{
    TIME_OUT = 1;
}
*/
