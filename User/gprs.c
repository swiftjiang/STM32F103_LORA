#include "gprs.h"

#define Buf2_Max 200 					  //串口1缓存长度
/*************  本地变量声明	**************/
char Uart1_Buf[Buf2_Max]; //串口2接收缓存
char *p1,*p2;             
u8 Times=0;
/*************	本地常量声明	**************/
const char *string = "AT+CIPSTART=\"TCP\",\"139.199.222.35\",2345";//IP登录服务器,需要根据自己的IP和端口做修改


void GPRS_RecvPacket(uint8_t *data,uint8_t len)
{
	if(data==NULL)
		 return;
	uint32_t temp = app_sched_event_put(data, len, GPRS_PacketParse);
	if(temp != 0)
	{
		printf("[error]%s,%d\r\n",__FILE__,__LINE__);
		return;
	}
}

void GPRS_PacketParse(void *data,uint16_t len)
{
	uint8_t i;
	uint8_t info[255] = {0};
	for(i=0;i<len;i++)
	{
		info[i] = ((uint8_t*)data)[i];
	}
	#ifdef DEBUG_GPRS
	
	printf("[GPRS_PacketParse]\r\n");
	
	for(i=0;i<len;i++)
	{
		printf("0x%02x,",((uint8_t*)data)[i]);
		//printf("%c,",((uint8_t*)data)[i]);
	}
	
	printf("----------------------------\r\n");
	#endif
	/*
	if(((uint8_t*)data)[2] == 0x02 && ((uint8_t*)data)[3] == 0x01)
	{
		uint8_t up2master[10] = {0x00,0x01,0x17,0xff,0xff};
		
		#ifdef DEBUG_GPRS
		printf("[MASTER GPRS get watermeter data]");
		#endif
		
		for(i=2;i<len;i++)
		{
			up2master[i+3] = ((uint8_t*)data)[i];
		}
		
		#ifdef DEBUG_GPRS
		printf("\r\n[MASTER GPRS to MASTER watermeter data]");
		printf("\r\n------------------------------------\r\n");
		#endif
		*/
		//下发命令
		if(strstr((char *)info,"CMD"))
		{
			//uint8_t MASTER_SENDTEST[8]={0x00,0x00,0x17,0xff,0xff,0x00,0x01,0x00};
			uint8_t cmd[8] = {0x00,0x00,0x17,0xff,0xff,0x01,0x01,0x00};
			LoRa_SendPacket(cmd,sizeof(cmd));
		}
		
}





void GPRS_Init(void)
{
	GPRS_Reset();
	
	GPRS_EN_Init();

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
	USART_DMACmd(USART3,USART_DMAReq_Rx,DISABLE);
	
	printf("GPRS模块GPRS测试程序\r\n");
	printf("GPRS模块在注册网络\r\n");
	Delay_Ms(1000); 
	USART3_Send_String((uint8_t *)"+++",3);//退出透传模式，避免模块还处于透传模式中
  Delay_Ms(2000);
	Wait_CREG();   //等待模块注册成功
	printf("GPRS模块注册成功\r\n");
	printf("GPRS模块开始连接服务器\r\n");
	Set_ATE0();    //取消回显
	Connect_Server(); //设置GPRS参数
	printf("连接成功\r\n");
	
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
}

void GPRS_EN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO结构体变量
		
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);		//使能各个端口时钟，重要！！！
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;								//配置EN到9PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   		//通用输出推挽
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   		//配置端口速度为50M
  GPIO_Init(GPIOB, &GPIO_InitStructure);				   				//根据参数初始化GPIOD寄存器
}

void GPRS_Power(GPRS_PowerTypdef status)
{
	if(status == POWER_ON)
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	if(status == POWER_OFF)
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	Delay_Ms(1000);
}

void GPRS_Reset(void)
{
	GPRS_Power(POWER_OFF);
	GPRS_Power(POWER_ON);
}


/*******************************************************************************
* 函数名  : Ram_Initialization
* 描述    : 变量初始化函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/

void Ram_Initialization(void)
{
	Times=0;
	p1=Uart1_Buf;
	p2=p1;

}

/*******************************************************************************
* 函数名  : USART2_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 
*******************************************************************************/

void USART1_IRQHandler(void)                	
{
	u8 Res=0;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res=USART_ReceiveData(USART1);
		*p1 = Res;  	  //将接收到的字符串存到缓存中
		p1++;                			//缓存指针向后移动
		if(p1>&Uart1_Buf[Buf2_Max])       		//如果缓存满,将缓存指针指向缓存的首地址
		{
			p1=Uart1_Buf;
		} 
	}			
} 	




/*******************************************************************************
* 函数名 : CLR_Buf1
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf1(void)
{
	u16 k;
	for(k=0;k<Buf2_Max;k++)      //将缓存内容清零
	{
		Uart1_Buf[k] = 0x00;
	}
  p1=Uart1_Buf;               //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/

u8 Find(char *a)
{ 
  if(strstr(Uart1_Buf,a)!=NULL)
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void AT_timerHandler(void *arg)
{
	if(Times>0)
	{
		Times--;
	}
}
void Second_AT_Command(char *b,char *a,u8 wait_time)         
{
	char *c;
	c = b;										//保存字符串地址到c
	CLR_Buf1(); 
	Times = wait_time;
	for (; *b!='\0';b++)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET);
		USART_SendData(USART3,*b);//UART2_SendData(*b);
	}
	printf("CMD = [%s]\r\n",c);
	USART3_Send_String((uint8_t *)"\r\n",2);

	uint8_t ret = Find(a);
	printf("Times = %d,ret = %d\r\n",Times,ret);
	                  
	while(Times && !ret)
	{
		Delay_Ms(1000);
		Times--;
		ret = Find(a);
		printf("Times = %d,ret = %d\r\n",Times,ret);
		if(ret) 
		{
			break;
    }
	}
	Times = 0;
		
	printf("Uart1_Buf = [%s]\r\n",Uart1_Buf);
	
	CLR_Buf1(); 
}

/*******************************************************************************
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Wait_CREG(void)
{
	u8 i;
	u8 k;
	i = 0;
  while(i == 0)        			
	{
		CLR_Buf1();        
		USART3_Send_String((uint8_t*)"AT+CREG?",strlen("AT+CREG?"));   //查询等待模块注册成功
		USART3_Send_String((uint8_t*)"\r\n",2);
		Delay_Ms(5000);  						
	    for(k=0;k<Buf2_Max;k++)      			
    	{
				if(Uart1_Buf[k] == ':')
				{
					if((Uart1_Buf[k+4] == '1')||(Uart1_Buf[k+4] == '5'))   //说明注册成功
					{
						i = 1;
						printf("\r\n");
						break;
					}
				}
			}
		printf("注册中.....");
	}
}

/*******************************************************************************
* 函数名 : Set_ATE0
* 描述   : 取消回显
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Set_ATE0(void)
{
	Second_AT_Command("ATE0","OK",3);								//取消回显	
	printf("取消回显\r\n");	
}
/*******************************************************************************
* 函数名 : Connect_Server
* 描述   : GPRS连接服务器函数
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Connect_Server(void)
{
	USART3_Send_String((uint8_t *)"AT+CIPCLOSE=1",strlen("AT+CIPCLOSE=1"));	//关闭连接
  Delay_Ms(100);
	Second_AT_Command("AT+CIPSHUT","SHUT OK",2);		//关闭移动场景
	Second_AT_Command("AT+CGCLASS=\"B\"","OK",2);//设置GPRS移动台类别为B,支持包交换和数据交换	
	Second_AT_Command("AT+CGDCONT=1,\"IP\",\"UNINET\"","OK",2);//设置PDP上下文,互联网接协议,接入点等信息
	Second_AT_Command("AT+CGATT=1","OK",2);//附着GPRS业务
	Second_AT_Command("AT+CIPCSGP=1,\"UNINET\"","OK",2);//设置为GPRS连接模式
  Second_AT_Command("AT+CIPMUX=0","OK",2);//设置为单路连接
	Second_AT_Command("AT+CIPHEAD=1","OK",2);//设置接收数据显示IP头(方便判断数据来源,仅在单路连接有效)
	Second_AT_Command("AT+CIPMODE=1","OK",2);//打开透传功能
	Second_AT_Command("AT+CIPCCFG=4,5,200,1","OK",2);//配置透传模式：单包重发次数:2,间隔1S发送一次,每次发送200的字节
  Second_AT_Command((char*)string,"OK",5);//建立连接
  Delay_Ms(100);                                //等待串口数据接收完毕
  CLR_Buf1();                                    //清串口接收缓存为透传模式准备
}

















/*******************************************************************************
* 函数名  : System_Initialization
* 描述    : STM32系统初始化函数(初始化STM32时钟及外设)
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
/*
void System_Initialization(void)
{
	RCC_Configuration();		//设置系统时钟为72MHZ(这个可以根据需要改)
  SysTick_Init_Config();  //初始化系统滴答时钟SysTick
  NVIC_Configuration();		//STM32中断向量表配配置
	Timer2_Init_Config();		//定时器2初始化配置
	USART1_Init_Config(115200);	//串口1初始化配置
	USART2_Init_Config(115200);	//串口2初始化配置	
	GPIO_Config();          //初始化GPIO
}
*/

/*******************************************************************************
* 函数名 : main 
* 描述   : 主函数
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 串口2负责与MG323模块通信，串口1用于串口调试，可以避免在下载程序时数据
					 还发送到模块
*******************************************************************************/
/*
int main(void)
{
	System_Initialization(); //系统初始化
	Ram_Initialization();    //变量初始化

	

	while(1)
	{
		Rec_Server_Data();//接收数据并原样返回	
	}
}
*/

/*******************************************************************************
* 函数名 : Rec_Server_Data
* 描述   : 接收服务器数据函数,并原样返回
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
/*
void Rec_Server_Data(void)
{
	if(p2!=p1)   		//说明还有数据未发出
	{	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
		USART_SendData(USART2,*p2);
		p2++;
    if(p2>&Uart1_Buf[Buf2_Max])
			p2=Uart1_Buf;
	}
}
*/

