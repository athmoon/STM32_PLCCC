/*
	该文件为集控器与plc模块通讯线程
	通讯协议为376.2
	集控器为主机，plc模块为从机
	使用的串口号为串口4，使用DMA接受
*/
#include "sys.h"
#include "includes.h"
#include "usart.h"
#include "def.h"

extern USART_COM com4;
extern OS_EVENT * Com4_tx_sem;

void send_to_com4(u8 *StrBuf, u8 bufLen);

void Target3_task(void *pdata)
{	
		u8 err = 0;
		
		//OSTimeSet(0);
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 1); //任务调度延时
			
			// 等待串口4信号量
			
			OSSemPend(Com4_tx_sem, DEV_TIMEOUT_10,&err);
			if(err == OS_ERR_NONE){
				send_to_com4(com4.DMA_TX_BUF, com4.lenSend);
			}
			
		}
}

void send_to_com4(u8 *StrBuf, u8 bufLen) {
	u8 i;
	for (i = 0; i < bufLen; i++) {
		USART_SendData(UART4, StrBuf[i]);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}

