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

void send_to_mainloop(void);

void Target3_task(void *pdata)
{	
		u8 err = 0;
		
	
		OSTimeDlyHMSM(0, 0, 1, 0);//等待设备稳定
		
		OSTimeSet(0);
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 10); //任务调度延时
			// 等待串口4信号量
			OSSemPend(com4.sem_DMA_RX,DEV_TIMEOUT_10,&err);
			if(err == OS_ERR_NONE){
				send_to_mainloop();
			}
			
		}
}

void send_to_com4(char *StrBuf) {
	u32 i;
	for (i = 0; i < strlen(StrBuf); i++) {
		USART_SendData(UART4, StrBuf[i]);
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}

void send_to_mainloop(void) {
	
}
