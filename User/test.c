#include "test.h"

void test_get_data(uint8_t *data,uint8_t len)
{
	if(data==NULL)
		 return;
	uint32_t temp = app_sched_event_put(data, len, test_parse);
	if(temp != 0)
	{
		printf("[error]file:%s,line:%d,error is %d\r\n",__FILE__,__LINE__,temp);
		return;
	}
	else
	{
		printf("[success]app_sched_event_put\r\n");
	}
}
void test_parse(void *data,uint16_t len)
{
	uint8_t i;
	data=(uint8_t*)data;
	printf("[test_parse info] from M-BUS\r\n*****");
	for(i=0;i<len;i++)
	{
		printf("0x%02x,",((uint8_t*)data)[i]);
	}
	printf("\r\n------------------------------------\r\n");
	
	if(((uint8_t*)data)[2] == 0x02 && ((uint8_t*)data)[3] == 0x01)
	{
		uint8_t up2master[10] = {0x00,0x01,0x17,0xff,0xff};
		
		printf("[SLAVER get watermeter data]");
		for(i=2;i<len;i++)
		{
			printf("0x%02x,",((uint8_t*)data)[i]);
			up2master[i+3] = ((uint8_t*)data)[i];
		}
		printf("\r\n");
		//向MASTER上传查水表数据
		LoRa_SendPacket(up2master,len+3);
	}
	
}
/**************************************************************************************************************/

uint8_t MASTER_SENDTEST[8]={0x00,0x00,0x17,0xff,0xff,0x00,0x01,0x00};
uint8_t SLAVER_SENDTEST[8]={0x00,0x01,0x17,0xff,0xff,0x00,0x00,0x00};
uint8_t seq = 0;
void init_LoRa(void)
{
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
	
	LoRa_GetParameter();
	Delay_Ms(1000);
	LoRa_GetVersionInfo();
}

void timerLoRaHandler(void* arg)
{
	(void)arg;
	
	#ifdef MASTER
	MASTER_SENDTEST[5]=0x01;
	MASTER_SENDTEST[6]=0x01;
	MASTER_SENDTEST[7]=seq++;
	LoRa_SendPacket(MASTER_SENDTEST,sizeof(MASTER_SENDTEST));
	#endif
		
	#ifdef SLAVER
	SLAVER_SENDTEST[7]=seq++;
	LoRa_SendPacket(SLAVER_SENDTEST,sizeof(SLAVER_SENDTEST));
	#endif
}
extern timer_t timerLoRa;
void testLoRa(void)
{
	
	
	#ifdef MASTER
	timer_create(&timerLoRa,3000,OPT_TMR_PERIODIC,timerLoRaHandler,NULL);
	timer_start(&timerLoRa);
	#endif
	
	#ifdef SLAVER
	timer_create(&timerLoRa,5000,OPT_TMR_PERIODIC,timerLoRaHandler,NULL);
	timer_start(&timerLoRa);
	#endif
	
	
	
}
/**************************************************************************************************************/

timer_t testTimerTyp;
void testTimer(void)
{
	timer_create(&testTimerTyp,100,OPT_TMR_PERIODIC,timerHandler,NULL);
	timer_start(&testTimerTyp);
}
void timerHandler(void* arg)
{
	(void)arg;
	PBout(15)=~PBout(15);

}
/**************************************************************************************************************/

uint8_t MBUSDATA[5] = {0xFF,0xFF,0xC0,0x00,0x00};
uint8_t seq_mbus = 0;
void MBUS_Send5Bytes(void)
{
	MBUSDATA[4] = seq_mbus++;
	
	//printf("%d,%d,%d,%d,%d\r\n",MBUSDATA[0],MBUSDATA[1],MBUSDATA[2],MBUSDATA[3],MBUSDATA[4]);
	USART3_Send_String(MBUSDATA,5);
}
