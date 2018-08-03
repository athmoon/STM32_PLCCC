#include "led.h"
#include "includes.h"
#include "rtc.h"
#include "def.h"
#include "usart.h"

extern USART_COM com1;
extern OS_EVENT * Com1_rx_sem;
extern OS_EVENT * Com1_tx_sem;

void send_to_com1(u8 *StrBuf, u8 strLen);
//Target2任务
void Target2_task(void *pdata)
{	  
	u8 err = 0;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);
		
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			OSSemPost(Com1_rx_sem);
		}
		
		OSSemPend(Com1_tx_sem, DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			send_to_com1(com1.DMA_TX_BUF, com1.lenSend);
		}
	}
}

void send_to_com1(u8 *StrBuf, u8 strLen) {
	u8 i;
	for (i = 0; i < strLen; i++) {
		USART_SendData(USART1, StrBuf[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}
