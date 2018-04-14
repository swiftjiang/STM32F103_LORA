#include "timer.h"

#define NULL (void*)0

#define TIME_PRECISION    10      //10ms one interrupt	 

/* timer list(arry) */
static timer_t		*timer_list[TIMER_LIST_MAX] = {NULL};
static uint8_t		timer_count = 0; 
static uint32_t     timer_tick=0;  

static void timer_cleanup(timer_t *ptimer);
static uint8_t timer_link(timer_t *ptimer);
static uint8_t timer_unlink(timer_t *ptimer);
void    timer_task(void);
void hard_tick_init(void);


void timer_init(void)
{
    for(uint8_t i=0;i<TIMER_LIST_MAX;i++){
        timer_list[i] = NULL;
    }
    
    timer_count = 0;
    timer_tick  = 0;
		
		hard_tick_init();
}

uint8_t timer_create(timer_t   *ptimer,	/* point to timer_t */
					           uint32_t  		period,		/* period			*/
					           uint32_t  		opt,
					           tmr_fnct_ptr	pcallback,
                     void* arg)
{
	if(!ptimer)
		return  0;
	
	ptimer->expire   = 0;
    ptimer->period   = period;
	ptimer->opt      = opt;
	ptimer->callback = pcallback;
	ptimer->state    = TMR_STATE_STOPPED;
	ptimer->arg      = arg;
	
	return 1;
}


uint8_t timer_del(timer_t *ptimer)
{
	if(!ptimer)
		return 0;
	timer_cleanup(ptimer);
	ptimer = NULL;
	
	return 1;
}

uint8_t timer_start(timer_t *ptimer)
{
    if(!ptimer)
        return 0;
    
    switch(ptimer->state)
    {
        case TMR_STATE_RUNNING:
                timer_unlink(ptimer);
                timer_link(ptimer);
                return 1;
        case TMR_STATE_STOPPED:
        case TMR_STATE_COMPLETED:
                timer_link(ptimer);
                return 1; 
        case TMR_STATE_UNUSED:
                return 0;
        default:
                return 0;
    }
}

uint8_t timer_stop(timer_t *ptimer)
{
    if(!ptimer)
        return 0;
    
   return timer_unlink(ptimer);
}

uint32_t get_timer_ms(void)
{
	return timer_tick;		
}

/***************************************************
****************************************************
****************************************************
****************************************************
****************************************************
****************************************************
***************************************************/

void timer_task(void)
{
	timer_t *ptmr = NULL;
	
    timer_tick=timer_tick+TIME_PRECISION;

   
    for(uint8_t i=0;i<TIMER_LIST_MAX;i++)
    {
        if(!timer_list[i])
            continue;
        ptmr = timer_list[i];
        if(timer_tick >= ptmr->expire)
        {
            if(ptmr->callback != NULL)
            {
                ptmr->callback(timer_list[i]->arg);
            }
            
            if(ptmr->opt == OPT_TMR_PERIODIC)
            {       
                ptmr->expire = timer_tick + ptmr->period;
            }else{
                ptmr->state = TMR_STATE_COMPLETED;
                timer_unlink(timer_list[i]);
            }
        }
    }
}




static void timer_cleanup(timer_t *ptimer)
{
	if(!ptimer)
		return;
	ptimer->state    = TMR_STATE_UNUSED;
	ptimer->callback = NULL;
	ptimer->expire   = 0;
	ptimer->opt      = 0;
	ptimer->arg      = NULL;
}

static uint8_t timer_link(timer_t *ptimer)
{
	uint8_t i;
	
	if(!ptimer)
		return 0;
	
	if(timer_count >= TIMER_LIST_MAX)
		return 0;
	
	for(i=0;i<TIMER_LIST_MAX;i++)
    {
		if(timer_list[i] != NULL)
           continue;
            
        ptimer->state  = TMR_STATE_RUNNING;
        ptimer->expire = ptimer->period + timer_tick;
        timer_list[i]  = ptimer;
        timer_count++;
        return 1;
	}
    
    return 0;
}

static uint8_t timer_unlink(timer_t *ptimer)
{
    uint8_t i;
    
    if(!ptimer)
        return 0;
    
    for(i=0;i<TIMER_LIST_MAX;i++)
    {
        if(timer_list[i] != ptimer)
            continue;
        timer_list[i] = NULL;
        ptimer->state = TMR_STATE_STOPPED;
        timer_count--;
        return 1;
    }
    return 0;
}

void hard_tick_init(void)
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
		
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  TIM_TimeBaseStructure.TIM_Period = 7200;
	TIM_TimeBaseStructure.TIM_Prescaler =100; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
 
	TIM_ITConfig( TIM2,TIM_IT_Update, ENABLE );
 
	TIM_Cmd(TIM2, ENABLE);

}


void TIM2_IRQHandler(void)
{ 	
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
        timer_task();
	} 
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
				    		  			    	    
}





