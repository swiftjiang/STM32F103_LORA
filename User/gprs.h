#ifndef __GPRS_H__
#define __GPRS_H__

#include "app_scheduler.h"
#include "loraSet.h"
#include "uart.h"
#include "debug.h"
#include "usually.h"
#include "timer.h"
#include "lora.h"

#define str_Call Ready  "Call Ready\r\n"
#define str_SMS Ready   "SMS Ready\r\n"
#define ATTEST			"AT\r\n"
#define str_CGATT 			"AT+CGATT?\r\n"
#define str_CIPMODE 		"AT+CIPMODE=1\r\n"
#define str_CSTT 				"AT+CSTT=\"UNINET\"\r\n"
#define str_CIICR 			"AT+CIICR\r\n"
#define str_CIFSR 			"AT+CIFSR\r\n"
#define str_CIPSTART 		"AT+CIPSTART=\"TCP\",\"139.199.222.35\",\"2345\"\r\n"


typedef enum GPRS_Power
{
		POWER_OFF = 0,
		POWER_ON  =1,
} GPRS_PowerTypdef;

void GPRS_RecvPacket(uint8_t *data,uint8_t len);
void GPRS_PacketParse(void *data,uint16_t len);

void GPRS_Init(void);
void GPRS_EN_Init(void);
void GPRS_Power(GPRS_PowerTypdef status);
void GPRS_Reset(void);
uint8_t GPRS_SendATCMD(uint8_t *data);
void GPRS_ConnectServer(uint8_t *net,uint8_t *ip,uint8_t *port);
void GPRS_ConnectParse(void *data,uint16_t len);
	

/************************************************************************************/

/*************	本地函数声明	**************/
void System_Initialization(void); //STM32系统初始化
void Ram_Initialization(void);    //内存变量初始化

void CLR_Buf1(void);              //清串口1接收缓存
u8 Find(char *a);                 //查找字符串
void Second_AT_Command(char *b,char *a,u8 wait_time);  //发送AT指令
void Wait_CREG(void);                          //查询等待模块注册成功
void Set_ATE0(void);                           //取消回显
void Connect_Server(void);                     //配置GPRS连接参数，并进入透传模式
void Rec_Server_Data(void);                    //接收服务器数据并返回


#endif
