#include "profile.h"

#define LORA_MASTER_DATA        "MASTER"
#define LORA_SLAVER_DATA        "SLAVER"
#define GPRS_DATA               "GPRS"
#define MBUS_DATA               "MBUS"

static void Handle_lora_master(void* data,uint8_t len);
static void Handle_lora_slaver(void* data,uint8_t len);
static void Handle_gprs(void* data,uint8_t len);
static void Handle_mbus(void* data,uint8_t len);

void Profie_get_data(char* tag,void* data,uint8_t len)
{
    if(strcmp(LORA_MASTER_DATA,tag) == 0)
    {
        //GET master data
        Handle_lora_master(data,len);
    }

    if(strcmp(LORA_SLAVER_DATA,tag) == 0)
    {
        //GET slaver data
        Handle_lora_slaver(data,len);
    }

    if(strcmp(GPRS_DATA,tag) == 0)
    {
        //GET gprs data
        Handle_gprs(data,len);
    }

    if(strcmp(MBUS_DATA,tag) == 0)
    {
        //GET mbus data
        Handle_mbus(data,len);
    }

}


/*
uint8_t MASTER_SENDTEST[8]={0x00,0x00,0x17,0xff,0xff,0x00,0x01,0x00};
uint8_t SLAVER_SENDTEST[8]={0x00,0x01,0x17,0xff,0xff,0x00,0x00,0x00};
*/
static void Handle_lora_master(void* data,uint8_t len)
{
    ;
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
		char *gprs = "watermeter data\r\n";
		printf("[MASTER to GPRS]%s",gprs);
		USART3_Send_String((uint8_t *)gprs,strlen(gprs));
	}
}


/*
uint8_t MASTER_SENDTEST[8]={0x00,0x00,0x17,0xff,0xff,0x00,0x01,0x00};
uint8_t SLAVER_SENDTEST[8]={0x00,0x01,0x17,0xff,0xff,0x00,0x00,0x00};
*/
static void Handle_lora_slaver(void* data,uint8_t len)
{
    ;
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


static void Handle_gprs(void* data,uint8_t len)
{
    ;
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


static void Handle_mbus(void* data,uint8_t len)
{
    ;
    //data --> LoRa_SendPacket
	
	#ifdef DEBUG_MBUS
	uint8_t i;
	printf("[MBUS_PacketParse]\r\n*****");
	for(i=0;i<len;i++)
	{
		printf("0x%02x,",((uint8_t*)data)[i]);
	}
	printf("\r\n");
	#endif
	
	if(((uint8_t*)data)[2] == 0x02 && ((uint8_t*)data)[3] == 0x01)
	{
		uint8_t up2master[10] = {0x00,0x01,0x17,0xff,0xff};
		
		#ifdef DEBUG_MBUS
		printf("[SLAVER M-BUS get watermeter data]");
		#endif
		
		for(i=2;i<len;i++)
		{
			up2master[i+3] = ((uint8_t*)data)[i];
		}
		
		#ifdef DEBUG_MBUS
		printf("\r\n[SLAVER LoRa to MASTER watermeter data]");
		printf("\r\n------------------------------------\r\n");
		#endif
		
		//向MASTER上传查水表数据
		LoRa_SendPacket(up2master,len+3);
	}
}

/*
lora可优化
    LoRa_GetParameter()  
    LoRa_SendPacket()
    区分发送命令和发送数据，用两个函数表示，在函数里面设置模块工作模式




    LoRa_SendPacket()，增加tag参数，标识数据发送方身份（主/从），获取身份后自动增加数据头信息

*/


