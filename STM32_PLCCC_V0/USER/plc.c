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

extern USART_COM com1,com2,com3;

void Target3_task(void *pdata)
{	
		u8 err = 0;
		
	
		OSTimeDlyHMSM(0, 0,1,0);//�ȴ��豸�ȶ�
		
		OSTimeSet(0);
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 10); //���������ʱ
			//1�ȴ��û�����
			OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
			
		}
}
