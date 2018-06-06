//头文件调用
#include "uart.h"
#include "usually.h"
#include "app_scheduler.h"
#include "lora.h"
#include "timer.h"
#include "gprs.h"
#include "flash.h"

#include "test.h"

void init_system(void);
timer_t SYSLED;
timer_t timerLoRa;
void SYSTEM_LED(void* arg);//系统运行指示


#define MAIN_MENU "\r\n\
+--------------------+\r\n\
|                    |\r\n\
|    LoRa testing    |\r\n\
|                    |\r\n\
|--------------------|\r\n\
|--------------------|\r\n"
#define DEVICE_TYPE_MASTER "|*******MASTER*******|\r\n|--------------------|\r\n+--------------------+\r\n"
#define DEVICE_TYPE_SLAVER "|*******SLAVER*******|\r\n|--------------------|\r\n+--------------------+\r\n\r\n"



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
	/*
	SystemInit();					//系统时钟配置
	sys_timer_init();
	while(1)
	{
		sys_timer_handle();
	}
	*/
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
	
	
	
	
	GPRS_Power(POWER_ON);
	
	while(1)													
	{ 
		//任务调度
		app_sched_execute();
	}
	
}
uint32_t tedata[1024] = {0};

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
	Usart2_Configuration(9600);	//串口配置 设置波特率为2400
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
	timer_create(&SYSLED,200,OPT_TMR_PERIODIC,SYSTEM_LED,NULL);//200 modify
	timer_start(&SYSLED);
	#ifdef DEBUG_MAIN_FUN
	printf("|--init system LED success----|\r\n");
	#endif
	
	#ifdef DEBUG_MAIN_FUN
	printf("|-------**S*Y*S*T*E*M**-------|\r\n");
	printf("+-----------------------------+\r\n\r\n");
	#endif
	
	/*********************************************************************************************************************************************/
	
	uint8_t Temp_Data[1024] = {0};
	uint8_t d[1024] = {0};
	uint32_t data[5] = {0x12345678,0x12345678,0x12345678,0x12345678,0x12345678};
	
  uint32_t ReadNum = 0;
	uint32_t WriteNum = 0;
  int i = 0;  

	
	
	for(i=0;i<1024;i++)
	{
		tedata[i] = i;
		d[i] = i+1;
		//printf("%d,%d\r\n",i,tedata[i]);
	}
	
	
  WriteNum = WriteFlashNWord(0,d,1023);     //写入数据     
	printf("%d-------------------------------------------------------\r\n",WriteNum);                //发送读取的字节数
  ReadNum = ReadFlashNBtye(0, Temp_Data,1024); //读取数据
  printf("%d-------------------------------------------------------\r\n",ReadNum);                //发送读取的字节数  

  

  for(i = 0;i < ReadNum;i++)                
  {
		printf("%x\r\n",Temp_Data[i]);   //发送读取到的数据，可观测数据的存储格式  
  }
	printf("----------------------------------------------------------\r\n");

	return;
	/*********************************************************************************************************************************************/
	//GPRS_Init();
	//return;
	
	/**************************LoRa 初始化*********************************/
	printf("%s",MAIN_MENU);
	#ifdef MASTER
	printf("%s",DEVICE_TYPE_MASTER);
	#endif
	#ifdef SLAVER
	printf("%s",DEVICE_TYPE_SLAVER);
	#endif
	
	LoRa_Init();
	LoRa_Reset();
	printf("waiting lora for reseting . . . . . .\r\n");
	printf("init LoRa success \r\n\r\n");
	
	PreciseDelay_ms(1000);
	LoRa_GetParameter();
	PreciseDelay_ms(1000);
	LoRa_GetVersionInfo();
	PreciseDelay_ms(1000);
	LoRa_SetMode(MODE0);
	

	LoRaHeartbeat();

	
/*
	GPRS_Init();
*/




}

void SYSTEM_LED(void* arg)
{
	(void)arg;
	LED3=~LED3;
	LED_ADJ4=~LED_ADJ4;
	//USART3_Send_String((uint8_t*)"MBUS MASTER\r\n",sizeof("MBUS MASTER\r\n"));
}






/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

