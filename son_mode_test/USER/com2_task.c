#include "includes.h"
#include "def.h"
#include "usart.h"

extern USART_COM com2;
extern OS_EVENT * Com2_rx_sem;
extern OS_EVENT * Com2_tx_sem;

void send_to_com2(u8 *StrBuf, u8 bufLen);
//Target4任务
void Target4_task(void *pdata)
{	  
	u8 err = 0;
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);
		
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			OSSemPost(Com2_rx_sem);
		}
		
		OSSemPend(Com2_tx_sem, DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			send_to_com2(com2.DMA_TX_BUF, com2.lenSend);
		}
	}
}

void send_to_com2(u8 *StrBuf, u8 bufLen) {
	u8 i;
	for (i = 0; i < bufLen; i++) {
		USART_SendData(USART2, StrBuf[i]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}
