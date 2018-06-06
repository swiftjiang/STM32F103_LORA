#include "mbus.h"


void MBUS_RecvPacket(uint8_t *data,uint8_t len)
{
	if(data==NULL)
		 return;
	uint32_t temp = app_sched_event_put(data, len, MBUS_PacketParse);
	if(temp != 0)
	{
		#ifdef DEBUG_MBUS
		printf("[error]%s,%d\r\n",__FILE__,__LINE__);
		#endif
		return;
	}
	
	else
	{
		;
		#ifdef DEBUG_MBUS
		printf("[success]app_sched_event_put\r\n");
		#endif
	}
	
}

void MBUS_PacketParse(void *data,uint16_t len)
{
	Profie_get_data(MBUS_DATA,data,len);
}




