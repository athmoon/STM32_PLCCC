/*
	���ļ�Ϊ��������plcģ��ͨѶ�߳�
	ͨѶЭ��Ϊ376.2
	������Ϊ������plcģ��Ϊ�ӻ�
	ʹ�õĴ��ں�Ϊ����4��ʹ��DMA����
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
		
	
		OSTimeDlyHMSM(0, 0, 1, 0);//�ȴ��豸�ȶ�
		
		OSTimeSet(0);
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 100); //���������ʱ
			// �ȴ�����4�ź���
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
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET){}//�ȴ����ͽ���
	}
}

void send_to_mainloop(void) {
	
}
