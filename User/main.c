//头文件调用
#include "uart.h"
#include "usually.h"
#include "app_scheduler.h"
#include "lora.h"
#include "timer.h"

#include "test.h"

void init_system(void);
timer_t SYSLED;
timer_t timerLoRa;
void SYSTEM_LED(void* arg);//系统运行指示

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: main
** 功能描述: 主函数入口
** 参数描述：这个程序演示了串口收发程序 ，接收是采用中断方式
				你可以输入键盘上的任意值 电脑超级终端将以十进制输出你输入的数值 
				比如：1对应十进制是49；2对应于50；a对应于97；这个你懂！！！
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


//extern void timerLoRaHandler(void* arg);
void MBUSTEST(void* arg)
{
	(void)arg;
	//MBUS_Send5Bytes();
	printf("############\n");
	//timerLoRaHandler(NULL);
}

int  main()
{
	//系统初始化
	init_system();
	
	//timer测试
	#ifdef TEST_TIMER
	printf("begin test timer. . . . . .\r\n");
	testTimer();
	#endif
	
	//timer_t mbusTest;
	//timer_create(&mbusTest,3000,OPT_TMR_PERIODIC,MBUSTEST,NULL);
	//timer_start(&mbusTest);
	
	#ifdef TEST_LORA
	//lora数据收发测试函数
	//testLoRa();
	#endif
	
	while(1)													
	{ 
		//任务调度
		app_sched_execute();
		//USART3_Send_String((uint8_t*)"CHECK WATERMETER CMD",sizeof("CHECK WATERMETER CMD"));
	}
}

void init_system(void)
{
	SystemInit();					//系统时钟配置

	Init_NVIC_USART1();					//中断向量表注册函数
	Init_Usart1();					//串口引脚配置
	Usart1_Configuration(9600);	//串口配置 设置波特率为115200
	#ifdef DEBUG_MAIN_FUN
	printf("\r\n\r\n\r\n");
	printf("+-----------------------------+\r\n");
	printf("|-------**S*Y*S*T*E*M**-------|\r\n");
	printf("|--init usart1 success--------| \r\n");	
	#endif

	Init_NVIC_USART2();					//中断向量表注册函数
	Init_Usart2();					//串口引脚配置
	Usart2_Configuration(9600);	//串口配置 设置波特率为115200
	#ifdef DEBUG_MAIN_FUN
	printf("|--init usart2 success--------|\r\n");
	#endif
	
	Init_NVIC_USART3();					//中断向量表注册函数
	Init_Usart3();					//串口引脚配置
	Usart3_Configuration(9600);	//串口配置 设置波特率为115200
	#ifdef DEBUG_MAIN_FUN
	printf("|--init usart3 success--------|\r\n");
	#endif
	
	scheduler_init();
	#ifdef DEBUG_MAIN_FUN
	printf("|--init app scheduler success-|\r\n");
	#endif
	
	timer_init();
	#ifdef DEBUG_MAIN_FUN
	printf("|--init timer success---------|\r\n");
	#endif
	
	Init_LED3();
	timer_create(&SYSLED,200,OPT_TMR_PERIODIC,SYSTEM_LED,NULL);
	timer_start(&SYSLED);
	#ifdef DEBUG_MAIN_FUN
	printf("|--init system LED success----|\r\n");
	#endif
	
	#ifdef DEBUG_MAIN_FUN
	printf("|-------**S*Y*S*T*E*M**-------|\r\n");
	printf("+-----------------------------+\r\n\r\n");
	#endif
	
	
	
	#ifdef TEST_LORA
	init_LoRa();
	//lora数据收发测试函数
	testLoRa();
	#endif
	
}

void SYSTEM_LED(void* arg)
{
	(void)arg;
	LED3=~LED3;
}






/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

