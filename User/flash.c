#include "flash.h"


/*
flash memory map
begin address = BEGIN_ADDR = 0x08000000
end   address = END_ADDR   = 0x0800FFFF

every page is 1K = 1024 Byte = 0x400 Byte

page(n) address is（n range is 0 -> 64）:
				begin address = PN_ADDR = BEGIN_ADDR + n*1024
				end   address = PN_ADDR + 0x3FF
*/

  

#define  STARTADDR  0x08003800			//预留0x3800 = 14K代码空间，剩余50K
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;      //Flash操作状态变量  




#define DATA_IS_WORD(len) do{\
															if(len%4 != 0)\
																return 0;\
														}while(0)



uint32_t ReadFlashNBtye(uint32_t Offset, uint8_t *ReadBuf, uint32_t len)   
{
		uint32_t DataNum = 0;  
    Offset = (uint32_t)(STARTADDR  + Offset);
    while(DataNum < len)   
    {
			*(ReadBuf + DataNum) = *(__IO uint8_t*) Offset++;  
      DataNum++;  
    }  
    return DataNum;  
}


uint32_t WriteFlashNWord(uint32_t Offset,uint8_t *Data,uint32_t len)
{
	uint32_t i;
	uint32_t num = 0;
	uint32_t temp = 0;
	
	DATA_IS_WORD(len);

	FLASH_UnlockBank1();  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
  FLASHStatus = FLASH_ErasePage(STARTADDR);//写入之前先擦除当前页
  if(FLASHStatus == FLASH_COMPLETE)  
  {  
		for(i=0;i<len;i=i+4)
		{
			temp = Data[i+3];
			temp = (temp<<8)|Data[i+2];
			temp = (temp<<8)|Data[i+1];
			temp = (temp<<8)|Data[i];
			//printf("temp = %x\r\n",temp);
			FLASHStatus = FLASH_ProgramWord(STARTADDR + Offset, temp);
			Offset = Offset + 4;
			num++;
			//printf("%d\r\n",i);
		}
  }  
  FLASH_LockBank1();
	
	return num*4;
}
/*
void WriteFlashNWord(uint32_t Offset,uint32_t *Data,uint32_t len)
{
	uint32_t i;

	FLASH_UnlockBank1();  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);   
  FLASHStatus = FLASH_ErasePage(STARTADDR);//写入之前先擦除当前页
  if(FLASHStatus == FLASH_COMPLETE)  
  {  
		for(i=0;i<len;i++)
		{
			FLASHStatus = FLASH_ProgramWord(STARTADDR + Offset, Data[i]);
			Offset = Offset + 4;
			//printf("%d\r\n",i);
		}
  }  
  FLASH_LockBank1();  
}
*/


