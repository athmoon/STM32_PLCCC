#include "led.h"
#include "sys.h"
#include "usart.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "includes.h"
#include "dma.h"
#include "rtc.h"
#include "flash.h"
#include "interrupt.h"
#include "def.h"

typedef enum {
	WorkStatus_Starting = 1,
	WorkStatus_Idle,
	WorkStatus_Error,
	WorkStatus_Test,
} WorkStatus;

extern calendar STM32_calendar;
// extern USART_COM com1;

WorkStatus workStatus = WorkStatus_Starting;

void device_test(void);
void receive_msg_from_com1(void);
void receive_msg_from_com4(void);

void Target1_task(void *pdata)
{
	
	OSTimeDlyHMSM(0, 0,1,0);//�ȴ��豸�ȶ�
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
	//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)ElePrice,12);

	// �豸�Լ�
	device_test();
	OSTimeSet(0);
	
	workStatus = WorkStatus_Test;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); //���������ʱ
		
		// OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		receive_msg_from_com1();
		receive_msg_from_com4();
		
		// ����״̬��
		switch(workStatus) {
			case WorkStatus_Starting://����״̬
				// ���ģ�����ӳɹ���ת�����״̬
				break;
			case WorkStatus_Idle://����״̬
				// 
				break;
			case WorkStatus_Error://�쳣״̬
				break;
			case WorkStatus_Test://����״̬������ʹ��
				// ������յ�����1�����ݣ�ת��������4
				// ������յ�����4�����ݣ�ת��������1
				break;
			default:
				break;
		}
		
	}
}

// �豸�Լ�
void device_test(void) {
	
}

// ���մ���1��������Ϣ
void receive_msg_from_com1(void) {
	
}

// ����PLCģ�鷢������Ϣ
void receive_msg_from_com4(void) {
	
}

