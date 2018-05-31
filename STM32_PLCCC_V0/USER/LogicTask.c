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
#include "plcmodule.h"

typedef enum {
	WorkStatus_Starting = 1,
	WorkStatus_Config,
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

u8 plc_cmd[] = {0x68, 0x38, 0x00, 0xC1, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x03, 0x02, 0x01,\
0xB1, 0x37, 0x01, 0x01, 0x00, 0x00, 0x5A, 0x5A, 0x00, 0x2C, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,\
0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x13, 0x03, 0x21, 0x13, 0x03, 0x21, 0x46, 0x4D,\
0x4A, 0x5A, 0x20, 0x05, 0x14, 0x02, 0x00, 0x01, 0x80, 0x38, 0x16};
u8 plc_cmd_reply[] = {0x68, 0x0F, 0x00, 0x01, 0x00, 0x00, 0x28, 0x32, 0x00, 0x00, 0x00, 0x01, 0x00, 0x5C, 0x16};

void device_test(void);
u8 receive_msg_from_com1(INT32U timeout);
u8 receive_msg_from_com2(INT32U timeout);
u8 receive_msg_from_com4(INT32U timeout);

void Target1_task(void *pdata)
{
	
	OSTimeDlyHMSM(0, 0,1,0);//�ȴ��豸�ȶ�
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
	//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)ElePrice,12);

	// �豸�Լ�
	device_test();
	OSTimeSet(0);
	
	workStatus = WorkStatus_Starting;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //���������ʱ
		
		
		// ����״̬��
		switch(workStatus) {
			case WorkStatus_Starting://����״̬
				if (PLC_STATE0 == 0) {// ���plcģ�������
					PLC_RST(0);// ��������10���븴λ״̬��Ȼ����Ϊ�ߵ�ƽ
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Test;// ģ�����ӡ���λ�ɹ���ת�����
					// workStatus = WorkStatus_Config;// ģ�����ӳɹ���ת����Ϣ�˶ԣ����ڵ�����
				}
				break;
			case WorkStatus_Config://���ڵ�����״̬
				// 1���ж��Ƿ���AFN03F10������������,����У������ݽ��н���
				// 1.1��
				// 2�����û�У��ȴ��Ƿ񳬹�20s������20s�������Ͳ�ѯ����
				break;
			case WorkStatus_Idle://����״̬
				// 
				break;
			case WorkStatus_Error://�쳣״̬
				break;
			case WorkStatus_Test://����״̬������ʹ��
				// ������յ�����1�����ݣ�ת��������4
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// ������յ�����4�����ݣ�ת��������2
				receive_msg_from_com4(DEV_TIMEOUT_10);
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
u8 receive_msg_from_com1(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com1_rx_sem, timeout, &err);// ��ȡ�ź���
	if (err == OS_ERR_NONE) {
		memcpy(com4.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// ������1���ܻ����������ݿ���������4�ķ��ͻ�����
		com4.lenSend = com1.lenRec;
		OSSemPost(Com4_tx_sem);// ������Ϣ������4����ʼ�������ݡ�
		return 1;
	} else {
		return 0;
	}
	
}

// ����1���������ݳɹ�������0����������ʧ�ܻ�����Ϊ��
u8 receive_msg_from_com2(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com2_rx_sem, timeout, &err);// ��ȡ�ź���
	if (err == OS_ERR_NONE) {
		memcpy(com4.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// ������2���ܻ����������ݿ���������4�ķ��ͻ�����
		com4.lenSend = com2.lenRec;
		OSSemPost(Com4_tx_sem);// ������Ϣ������4����ʼ�������ݡ�
		return 1;
	} else {
		return 0;
	}
	
}

// ����PLCģ��(����4)��������Ϣ
u8 receive_msg_from_com4(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com4_rx_sem, timeout, &err);// ��ȡ�ź���
	if (err == OS_ERR_NONE) {

//		if (memcmp(com4.DMA_RX_BUF, plc_cmd, com4.lenRec) == 0) {
//			com4.lenSend = plc_cmd_reply[1] + (plc_cmd_reply[2]<<8);
//			memcpy(com4.DMA_TX_BUF, plc_cmd_reply, com4.lenSend);
//			OSSemPost(Com4_tx_sem);// ������Ϣ������4����ʼ�������ݡ�
//			
//			com1.lenSend = plc_cmd_reply[1] + (plc_cmd_reply[2]<<8);
//			memcpy(com1.DMA_TX_BUF, plc_cmd_reply, com1.lenSend);
//			OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
//		} else {
			memcpy(com1.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// ������4���ܻ����������ݿ���������2�ķ��ͻ�����
			com1.lenSend = com4.lenRec;
			OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
//		}
		return 1;
	} else {
		return 0;
	}
}

