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
extern USART_COM com1,com2,com3,com4;

OS_EVENT * Com1_rx_sem;
OS_EVENT * Com1_tx_sem;
OS_EVENT * Com2_rx_sem;
OS_EVENT * Com2_tx_sem;
OS_EVENT * Com4_rx_sem;
OS_EVENT * Com4_tx_sem;

WorkStatus workStatus = WorkStatus_Starting;

void device_test(void);
void receive_msg_from_com1(void);
void receive_msg_from_com4(void);

void Target1_task(void *pdata)
{
	
	u8 err;
	
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
				// ������յ�����1�����ݣ�ת��������2
				OSSemPend(Com1_rx_sem, 0,&err);
				if(err == OS_ERR_NONE){
					memcpy(com2.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// ������1���ܻ����������ݿ���������2�ķ��ͻ�����
					com2.lenSend = com1.lenRec;
					OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
				}
				// ������յ�����2�����ݣ�ת��������1
				OSSemPend(Com2_rx_sem, 0,&err);
				if(err == OS_ERR_NONE){
					memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// ������2���ܻ����������ݿ���������1�ķ��ͻ�����
					com1.lenSend = com2.lenRec;
					OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
				}
				// ������յ�����4�����ݣ�ת��������2
//				OSSemPend(Com4_rx_sem, 0,&err);
//				if(err == OS_ERR_NONE){
//					memcpy(com2.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// ������4���ܻ����������ݿ���������2�ķ��ͻ�����
//					com2.lenSend = com4.lenRec;
//					OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
//				}
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

// ����PLCģ��(����4)��������Ϣ
void receive_msg_from_com4(void) {
	
}

