#include "lora.h"


/*
M0 ---->PB0
M1 ---->PB1
AUX---->PB2


*/
#define LEADBYTE    0xFF
//#define SLAVER_LEADBYTE    0x00

uint8_t MASTER_PARAMETER[6] = {0xc0, 0x0, 0x1, 0x1a, 0x17, 0xc4}; //掉电保存,地址0x01,信道23,定点传输
uint8_t SLAVER_PARAMETER[6] = {0xc0, 0x0, 0x0, 0x1a, 0x17, 0xc4}; //掉电保存,地址0x00,信道23,定点传输

uint8_t CMD_GET_WORK_PARAM[3] = {0xC1,0xC1,0xC1};//返回6字节
uint8_t CMD_GET_VERSION_INFO[3] = {0xC3,0xC3,0xC3};//返回
uint8_t CMD_RESET[3] = {0xC4,0xC4,0xC4};//复位

void LoRa_Init(void)
{
	//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	//使能时钟端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 							//M0-->PB0
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   			//根据参数初始化GPIOB寄存器	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 							//M1-->PB1
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   			//根据参数初始化GPIOB寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 							//AUX-->PB2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   			//根据参数初始化GPIOB寄存器	
	
	while(PBin(2)==0){;}
	
	LoRa_InitParameter();
	
}
void LoRa_SetMode(uint8_t mode)
{
	switch(mode)
	{
		case MODE0:
			PBout(0)=0;
			PBout(1)=0;
			break;
		case MODE1:
			PBout(0)=1;
			PBout(1)=0;
			break;
		case MODE2:
			PBout(0)=0;
			PBout(1)=1;
			break;
		case MODE3:
			PBout(0)=1;
			PBout(1)=1;
			break;
		default:
			printf("[error in file:%s, line:%d]SET MODE ERROR\r\n",__FILE__,__LINE__);
	}
}
void LoRa_InitParameter(void)
{
	//LoRa_SetMode(MODE3);//在模式3下才可以执行
#ifdef MASTER
	LoRa_SendPacket(MASTER_PARAMETER,sizeof(MASTER_PARAMETER));
	
	#ifdef DEBUG_LORA_INIT_PARAM
	printf("MASTER PARAMETER INIT \r\n");
	#endif
#endif
#ifdef SLAVER
	LoRa_SendPacket(SLAVER_PARAMETER,sizeof(SLAVER_PARAMETER));
	
	#ifdef DEBUG_LORA_INIT_PARAM
	printf("SLAVER PARAMETER INIT \r\n");
	#endif
#endif
}

void LoRa_DeviceInfo(void *data,uint16_t len)
{
	if( ((uint8_t*)data)[0] == 0xc0 && len == 6 )//工作参数
	{
		uint8_t i;
		printf("[success] get working paramter:");
		for(i=0;i<len;i++)
		{
			printf("0x%02x,",((uint8_t*)data)[i]);
		}
		printf("\r\n");
	}
	if(((uint8_t*)data)[0] == 0xc3 && len == 4 )//版本信息
	{
		
		uint8_t i;
		printf("[success] get device version:");
		for(i=0;i<len;i++)
		{
			printf("0x%02x,",((uint8_t*)data)[i]);
		}
		printf("\r\n");
	}
}

void LoRa_GetParameter(void)//返回6字节数据   uint8_t *WorkParam
{
	LoRa_SendPacket(CMD_GET_WORK_PARAM,sizeof(CMD_GET_WORK_PARAM));
}
void LoRa_GetVersionInfo()//返回4字节数据  uint8_t *WorkParam
{
	LoRa_SendPacket(CMD_GET_VERSION_INFO,sizeof(CMD_GET_VERSION_INFO));
}
void LoRa_Reset(void)
{
	LoRa_SendPacket(CMD_RESET,sizeof(CMD_RESET));
	while(PBin(2)==0){;}//等待AUX拉高
}

void LoRa_SendPacket(uint8_t *data,uint8_t len)
{
	if(data[0]==0x00)//数据都是以0x00为第一个字节发送
	{
		LoRa_SetMode(MODE0);
	}
	else						//命令都是以0xcx为第一个字节发送
	{
		LoRa_SetMode(MODE3);
	}
	while(PBin(2)==0){;}
	
	USART2_Send_String(data,len);
	while(PBin(2)==0){;}
		
	#ifdef DEBUG_LORA_SEND_DATA
	uint8_t i;
		printf("send data info :")	;
	for(i=0;i<len;i++)
	{
		printf("0x%02x,",data[i]);
	}
	printf("\r\n");
	#endif
}
void LoRa_RecvPacket(uint8_t *data,uint8_t len)
{
	if(data==NULL || len==0)
		 return;
	if(app_sched_event_put(data, len, LoRa_PacketParse) != 0)
	{
		printf("[error]%s,%d\r\n",__FILE__,__LINE__);
		return;
	}
	#ifdef DEBUG_LORA_RECV_PACKET
	else
	{
		uint8_t i;
		printf("[success] app_sched_event_put data info :");
		for(i=0;i<len;i++)
		{
			printf("0x%02x,",data[i]);
		}
		printf("\r\n");
	}
	#endif
}

/***************************************************
************主从设备数据处理函数声明******************
***************************************************/
void MASTER_GPRS_ProductFunction(void *data,uint8_t len);
void SLAVER_MBUS_ProductFunction(void *data,uint8_t len);
/***************************************************
***************************************************/
static void LoRa_PacketParse(void *data,uint16_t len)
{
	//返回6字节数据，返回的工作参数
	//返回4字节数据，返回的版本信息
	//返回的前两个字节是FF FF，一般数据，进一步分析
	//uint8_t first = ((uint8_t*)data)[0];
	if(len > 2 && ((uint8_t*)data)[0] == LEADBYTE && ((uint8_t*)data)[1] == LEADBYTE) //一般数据处理
	{
		#ifdef MASTER
		MASTER_GPRS_ProductFunction(data,len);
		#endif
		
		#ifdef SLAVER
		SLAVER_MBUS_ProductFunction(data,len);
		#endif
	}
	else
		LoRa_DeviceInfo(data,len);
}

void MASTER_GPRS_ProductFunction(void *data,uint8_t len)
{
	//主设备处理收到的数据

	if(((uint8_t*)data)[2] == 0x02 && ((uint8_t*)data)[3] == 0x01)
	{
		
		#ifdef DEBUG_GPRS
		printf("[success]MASTER from LoRa:\r\n");
		printf("[MASTER get watermeter data]");
		uint8_t i;
		for(i=0;i<len;i++)
		{
			printf("0x%02x,",((uint8_t*)data)[i]);
		}
		printf("\r\n------------------------------------\r\n");
		#endif
		
		/*
		*
		*通过GPRS发送数据
		*
		*/
		printf("[MASTER to GPRS]\r\n");
	}
	
}
void SLAVER_MBUS_ProductFunction(void *data,uint8_t len)
{
	//从设备处理收到的数据
	
	if(((uint8_t*)data)[2] == 0x01 && ((uint8_t*)data)[3] == 0x01)
	{
		#ifdef DEBUG_MBUS
		printf("[success]SLAVER from LoRa:\r\n");
		printf("[SLAVER get CMD]");
		uint8_t i;
		for(i=0;i<len;i++)
		{
			printf("0x%02x,",((uint8_t*)data)[i]);
		}
		printf("\r\n------------------------------------\r\n");
		#endif
		//向水表下发查水表命令
		//USART3_Send_String((uint8_t*)"CHECK WATERMETER CMD",sizeof("CHECK WATERMETER CMD"));
		uint8_t CMD[5] = {0xff,0xff,0x01,0x01,0x00};
		USART3_Send_String(CMD,5);
	}
}

/*
LoRa_callbackTypedef LoRa_callback;
void LoRa_register_callback(callback cb,LoRaRegisterTypedef type)
{
	if(cb == NULL)
	{
		printf("LoRa_register_callback failed !\n");
		return;
	}
	if(type == GPRS)
	{
		LoRa_callback.GPRS_callback = cb;
	}
	if(type == MBUS)
	{
		LoRa_callback.MBUS_callback = cb;
	}
}
*/
