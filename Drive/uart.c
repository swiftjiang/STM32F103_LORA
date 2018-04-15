#include "uart.h"
#include "usually.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
	   
	return Data;										  //返回一个值
}
#endif 

/***************************************************************/


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART1**********************************************************
调试串口*********************************************************
RX-->PA9********************************************************
TX-->PA10*******************************************************
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint8_t Data)
{ 
	USART_SendData(USART1,Data);//发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){}//等待发送结
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_String
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_String(uint8_t* Data,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART1_Send_Byte(Data[i]);
		//while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Receive_Byte(uint8_t len)
{ 
	;
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Init_NVIC
** 功能描述: 系统中断配置
** 参数描述：无
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_NVIC_USART1(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	#ifdef  VECT_TAB_RAM  							//向量表基地址选择

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//将0x20000000地址作为向量表基地址(RAM)
	#else  

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)  
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断组 为2 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//配置串口1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口1中断
	NVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart_Init
** 功能描述: 串口引脚初始化
** 参数描述: 无
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_Usart1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	//使能各个端口时钟，重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 							//RX-->PA9
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 							//TX-->PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart1_Configuration
** 功能描述: 串口配置函数
** 参数描述: BaudRate设置波特率 
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Usart1_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    							//定义一个串口结构体

	USART_InitStructure.USART_BaudRate            =BaudRate ;	  					//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 		//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 		//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART1, &USART_InitStructure);															//根据参数初始化串口寄存器
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);													//使能串口中断接收
	USART_Cmd(USART1, ENABLE);     																				//使能串口外设
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_IRQHandler
** 功能描述: 串口中断函数
** 参数描述: 无 
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
//uint8_t USART1_ReceiveDate[64],USART1_LEN=0;;								//定义一个变量存放接收的数据
/*
void USART1_IRQHandler()
{
	uint8_t RX_dat;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//判断发生接收中断
	{
		RX_dat= USART_ReceiveData(USART1) ;      //接收数据，整理除去前两位
	}
	USART_ClearITPendingBit(USART1,  USART_IT_RXNE);          //清除中断标志
	//while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){}//等待接收结束
		
	USART_SendData(USART1 ,RX_dat);             //发送数据 
}

*/

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART2*********************************************************
LoRa数据端口****************************************************
RX-->PA3*******************************************************
TX-->PA2*******************************************************
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
#define MAX_UART_LEN  255
uint8_t UART2_RECV_BUF[MAX_UART_LEN]={0};
uint8_t UART2_RECV_LEN=0;
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART2_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART2_Send_Byte(uint8_t Data)
{ 
	USART_SendData(USART2,Data);//发送数据
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET){}//等待发送结
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_String
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART2_Send_String(uint8_t* Data,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART2_Send_Byte(Data[i]);
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART2_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*
#define MAX_UART_LEN  64
uint8_t UART2_RECV_BUF[MAX_UART_LEN]={0};
uint8_t UART2_RECV_LEN=0;
*/
void USART2_Receive_DMA(uint8_t* data,uint8_t len)
{
	//data = UART2_RECV_BUF
	;
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Init_NVIC_USART2
** 功能描述: 系统中断配置
** 参数描述：无
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_NVIC_USART2(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	#ifdef  VECT_TAB_RAM  							//向量表基地址选择

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//将0x20000000地址作为向量表基地址(RAM)
	#else  

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)  
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断组 为2

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//配置串口2为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口2中断
	NVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Init_DMA_USART2
** 功能描述: 串口2 DMA配置
** 参数描述: 无
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/


void Init_DMA_USART2(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&UART2_RECV_BUF[0]);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = MAX_UART_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel6,ENABLE);

	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart_Init
** 功能描述: 串口引脚初始化
** 参数描述: 无
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_Usart2(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	//使能各个端口时钟，重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 							//TX-->PA2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 							//RX-->PA3
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart2_Configuration
** 功能描述: 串口配置函数
** 参数描述: BaudRate设置波特率 
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Usart2_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    							//定义一个串口结构体

	USART_InitStructure.USART_BaudRate            =BaudRate ;	  					//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 		//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 		//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART2, &USART_InitStructure);															//根据参数初始化串口寄存器 
  
	//DMA传输开启以下语句
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE); 
  USART_Cmd(USART2, ENABLE);	// 使能USART2
	USART_ClearFlag(USART2, USART_FLAG_TC);     //清发送外城标志，Transmission Complete flag   CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	Init_DMA_USART2();
	
	//不使用DMA传输开启以下语句
	//USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);													//使能串口中断接收
	//USART_Cmd(USART2, ENABLE);     																				//使能串口外设
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART2_IRQHandler
** 功能描述: 串口中断函数
** 参数描述: 无 
** 作  　者: 
** 日　  期: 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

//#include "test.h"
#include"debug.h"
extern void LoRa_RecvPacket(uint8_t *data,uint8_t len);
void USART2_IRQHandler()
{
	//不使用DMA中断接收函数在文件末尾注释部分
	//以下是使用DMA中断接受函数
	uint32_t temp = 0;
	//printf("*******************************************\r\n");
  if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
  {
		USART_ClearFlag(USART2,USART_IT_IDLE);
    temp = USART2->SR;
    temp = USART2->DR; 
		temp=temp;
    DMA_Cmd(DMA1_Channel6,DISABLE);
    UART2_RECV_LEN = MAX_UART_LEN - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		#ifdef DEBUG_USART2_IRQHANDLER
		uint8_t i;
		#endif
		
    if(UART2_RECV_LEN>3)// && UART2_RECV_BUF[0] == LEADBYTE && UART2_RECV_BUF[1] == LEADBYTE  )
    {
			#ifdef DEBUG_USART2_IRQHANDLER
			printf("[USART2 DMA RECV data info:]");
			for(i=0;i<UART2_RECV_LEN;i++)
			{
				printf("%02x ",UART2_RECV_BUF[i]);
			}
			printf("____\r\n");
			#endif
			//LoRa数据传递
			LoRa_RecvPacket(UART2_RECV_BUF,UART2_RECV_LEN);
    }
		            
    DMA_SetCurrDataCounter(DMA1_Channel6, MAX_UART_LEN);
    DMA_Cmd(DMA1_Channel6,ENABLE);
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
USART3*********************************************************
***************************************************************
RX-->PB11******************************************************
TX-->PB10******************************************************
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

uint8_t UART3_RECV_BUF[MAX_UART_LEN]={0};
uint8_t UART3_RECV_LEN=0;

void USART3_Send_Byte(uint8_t Data)
{
	USART_SendData(USART3,Data);//发送数据
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){}//等待发送结
}
void USART3_Send_String(uint8_t* Data,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
		USART3_Send_Byte(Data[i]);
	}
}
void USART3_Receive_Byte(uint8_t len)
{
	
}
void USART3_Receive_DMA(uint8_t* data,uint8_t len)
{
	;
}
void Init_Usart3(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	//使能各个端口时钟，重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO ,ENABLE);
  //GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE); 
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
  //GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE); 
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 							//TX-->PB10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 							//RX-->PB11
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   			//根据参数初始化GPIOA寄存器	
}
void Usart3_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    							//定义一个串口结构体

	USART_InitStructure.USART_BaudRate            =BaudRate ;	  					//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 		//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 		//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART3, &USART_InitStructure);															//根据参数初始化串口寄存器
	
	//DMA传输开启以下语句
	USART_ITConfig(USART3,USART_IT_TC,DISABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
  USART_Cmd(USART3, ENABLE);	// 使能USART3
	USART_ClearFlag(USART3, USART_FLAG_TC);     //清发送外城标志，Transmission Complete flag   CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	Init_DMA_USART3();
	
	//不开启DMA传输以下语句
	//USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);													//使能串口中断接收
	//USART_Cmd(USART3, ENABLE);     																				//使能串口外设
}
void Init_NVIC_USART3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	#ifdef  VECT_TAB_RAM  							//向量表基地址选择

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//将0x20000000地址作为向量表基地址(RAM)
	#else  

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)  
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断组 为2

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//配置串口2为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口2中断
	NVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
}
void Init_DMA_USART3(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&UART3_RECV_BUF[0]);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = MAX_UART_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel3,ENABLE);

	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
}

#include "test.h"

extern void MBUS_RecvPacket(uint8_t *data,uint8_t len);
extern void GPRS_RecvPacket(uint8_t *data,uint8_t len);
void USART3_IRQHandler()
{
	//不使用DMA中断接收函数在文件末尾注释部分
	/*
	uint8_t RX_dat;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//判断发生接收中断
	{
		RX_dat=(USART_ReceiveData(USART3) & 0x7F);      //接收数据，整理除去前两位
	}
	RX_dat=(USART_ReceiveData(USART3) & 0x7F);
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);          //清除中断标志
	USART_SendData(USART3,RX_dat);             //发送数据
	*/
	
	//以下是使用DMA中断接受函数
	
	uint32_t temp = 0;
	//printf("*******************************************\r\n");
  if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
  {
		USART_ClearFlag(USART3,USART_IT_IDLE);
    temp = USART3->SR;
    temp = USART3->DR; 
		temp=temp;
    DMA_Cmd(DMA1_Channel3,DISABLE);
    UART3_RECV_LEN = MAX_UART_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);

		#ifdef DEBUG_USART3_IRQHANDLER
		uint8_t i;
		#endif
		
    if(UART3_RECV_LEN>3)// && UART3_RECV_BUF[0] == LEADBYTE && UART3_RECV_BUF[1] == LEADBYTE  )
    {
			#ifdef DEBUG_USART3_IRQHANDLER
			printf("[USART3 DMA RECV data info:]\r\n[");
			printf("%s\r\n",UART3_RECV_BUF);
			for(i=0;i<UART3_RECV_LEN;i++)
			{
				//printf("%02x ",UART3_RECV_BUF[i]);
				printf("%c",UART3_RECV_BUF[i]);
			}
			printf("]____\r\n");
			#endif
			
			//MBUS数据传递测试
			//test_get_data(UART3_RECV_BUF,UART3_RECV_LEN);
			#ifdef MBUS
			MBUS_RecvPacket(UART3_RECV_BUF,UART3_RECV_LEN);
			#endif
			
			#ifdef GPRS
			GPRS_RecvPacket(UART3_RECV_BUF,UART3_RECV_LEN);
			#endif
    }
		            
    DMA_SetCurrDataCounter(DMA1_Channel3, MAX_UART_LEN);
    DMA_Cmd(DMA1_Channel3,ENABLE);
	}
	
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/



/**************************************************************
**********不使用DMA时的USART2的中断接收函数**********************
***************************************************************
void USART2_IRQHandler()
{
	uint8_t RX_dat;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//判断发生接收中断
	{
		RX_dat=(USART_ReceiveData(USART2) & 0x7F);      //接收数据，整理除去前两位
	}
	RX_dat=(USART_ReceiveData(USART2) & 0x7F);
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);          //清除中断标志
	USART_SendData(USART2,RX_dat);             //发送数据
}
***************************************************************
**********不使用DMA时的USART2的中断接收函数**********************
**************************************************************/
