#include "led.h"
#include "includes.h"
#include "rtc.h"
#include "def.h"
#include "usart.h"

extern USART_COM com1;

void send_to_com1(u8 *StrBuf);
//Target2任务
void Target2_task(void *pdata)
{	  
	u8 err = 0;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 10);
		
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			send_to_com1(com1.DMA_RX_BUF);
		}
	}
}

void send_to_com1(u8 *StrBuf) {
	u32 i;
	for (i = 0; i < strlen((char *)StrBuf); i++) {
		USART_SendData(USART1, StrBuf[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}
