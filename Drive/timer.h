#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f10x.h"

typedef void   (*tmr_fnct_ptr) (void* arg);
typedef uint8_t TMR_STATE;

typedef struct{
    uint32_t		  expire;				
    uint32_t          period;
    tmr_fnct_ptr	  callback;	
    TMR_STATE		  state;	
    uint32_t		  opt;
    void* arg;
}timer_t;

#define   TIMER_LIST_MAX		5U		
 

/* timer state */
#define  TMR_STATE_UNUSED                    (TMR_STATE)(0u)
#define  TMR_STATE_STOPPED                   (TMR_STATE)(1u)
#define  TMR_STATE_RUNNING                   (TMR_STATE)(2u)
#define  TMR_STATE_COMPLETED                 (TMR_STATE)(3u)

#define  OPT_TMR_NONE                        (0u)  /* No option selected                                 */
#define  OPT_TMR_ONE_SHOT                    (1u)  /* Timer will not auto restart when it expires        */
#define  OPT_TMR_PERIODIC                    (2u)  /* Timer will     auto restart when it expires        */

//初始化timer，包括硬件（TIM2）和时钟滴答
void timer_init(void);

//创建一个定时器
//参数ptimer：定时器结构体指针
//参数period：定时周期
//参数opt：选项，包括无操作，关闭自动重启，开启自动重启
//参数pcallback：定时回调函数指针
//参数arg：可传递给定是回调函数的参数
uint8_t timer_create(timer_t   		*ptimer,	/* point to timer_t */
					           uint32_t  		period,		/* period			*/
					           uint32_t  		opt,
					           tmr_fnct_ptr	pcallback,
										 void* arg );

//删除定时器
uint8_t timer_del(timer_t *ptimer);
//打开定时器
uint8_t timer_start(timer_t *ptimer);
//关闭定时器
uint8_t timer_stop(timer_t *ptimer);
//获取当前定时器ms滴答次数
uint32_t get_timer_ms(void);

#endif

