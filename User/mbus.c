#include "mbus.h"


void MBUS_RecvPacket(uint8_t *data,uint8_t len)
{
	if(data==NULL)
		 return;
	uint32_t temp = app_sched_event_put(data, len, MBUS_PacketParse);
	if(temp != 0)
	{
		printf("[error]%s,%d\r\n",__FILE__,__LINE__);
		return;
	}
	/*
	else
	{
		#ifdef DEBUG_MBUS
		;
		printf("[success]app_sched_event_put\r\n");
		#endif
	}
	*/
}

void MBUS_PacketParse(void *data,uint16_t len)
{
	
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




