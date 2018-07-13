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
#include "_3762.h"

typedef enum {
	WorkStatus_Starting = 1,
	WorkStatus_Config,
	WorkStatus_Idle,
	WorkStatus_Error,
	WorkStatus_Test,
	WorkStatus_SonTest,
} WorkStatus;

typedef enum {
	ConfigStatus_link,
	ConfigStatus_datainit,
	ConfigStatus_addrinit,
	ConfigStatus_devinit,
} ConfigStatus;

ConfigStatus conf_status;

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

u8 son_test(INT32U timeout);
void Target1_task(void *pdata)
{
	
	u8 err = 0;
	
	OSTimeDlyHMSM(0, 0,1,0);//�ȴ��豸�ȶ�
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);

	// �豸�Լ�
	device_test();
	OSTimeSet(0);
	
	// workStatus = WorkStatus_Starting;
	workStatus = WorkStatus_SonTest;
	// workStatus = WorkStatus_Test;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //���������ʱ
		
		// �жϽڵ�����״̬
		// ����ڵ�Ͽ����ӣ��ƴ�++
		// ����Ͽ����ӳ���n�Σ��жϽڵ�Ͽ����ӣ������쳣�ȴ�������״̬
		
		// ����״̬��
		switch(workStatus) {
			case WorkStatus_Starting://����״̬
				if (PLC_STATE0 == 0) {// ���plcģ�������
					PLC_RST(0);// ��������10���븴λ״̬��Ȼ����Ϊ�ߵ�ƽ
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Test;// ģ�����ӡ���λ�ɹ���ת�����
					// workStatus = WorkStatus_Config;// ģ�����ӳɹ���ת����Ϣ�˶ԣ����ڵ�����
					conf_status = ConfigStatus_link;
				}
				break;
			case WorkStatus_Config://���ڵ�����״̬
				
				switch (conf_status) {
					case ConfigStatus_link:
						// 1���ж��Ƿ���AFN03F10������������,����У������ݽ��н���
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) goto TIMEOUTCHECK;//������
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) goto TIMEOUTCHECK;//У���
						if (!(readResult.AFN_code == AFN03 && readResult.F_code == F10)) goto TIMEOUTCHECK;//��AFN03F10ָ��
						// 1.1���ж�flash�д洢�����ڵ���Ϣ�Ƿ��뵱ǰ���ڵ��ϱ�����Ϣ��ͬ
						if (readResult.action == Null_Action) {
							// �����ͬ�������κδ���
						} else if (readResult.action == Hardware_Init) {// �������ͬ���г�ʼ������
							conf_status = ConfigStatus_addrinit;
						}
						
						TIMEOUTCHECK:
						if (OSTimeGet() >= 20) {//����20�룬�����·�AFN03F10
							
						}
						break;
						
					case ConfigStatus_addrinit:// ��ַ��ʼ��
						// 1�������������ڵ��ַָ������ڵ��ַ����Ϊ1
						// 2���ȴ����ڵ�ظ������޻ظ�����Ԥ��ʱ�䣬�ظ����͸�ָ��
						// 3�����ڵ�ظ��ɹ���������������ʼ��
						break;
					
					case ConfigStatus_datainit:// ��������ʼ��
						break;
						
					default:
						break;
				}
				
				break;
			case WorkStatus_Idle://����״̬
				// ����յ��ز����ڵ�����֡�����н������������������֡�����лظ�������Ǳ�������֡����������ظ���λ��
				// ����յ���λ����ָ�����376.2͸��ת��ָ��·����ز����ڵ�
				break;
			case WorkStatus_Error://�쳣״̬
				break;
			case WorkStatus_Test://����״̬������ʹ��
				// ������յ�����1�����ݣ�ת��������2
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// ������յ�����2�����ݣ�ת��������1
				// receive_msg_from_com4(DEV_TIMEOUT_10);
				receive_msg_from_com2(DEV_TIMEOUT_10);
				break;
			case WorkStatus_SonTest:
				son_test(DEV_TIMEOUT_10);
				// ������յ�����1�����ݣ�ת��������2
				receive_msg_from_com1(DEV_TIMEOUT_10);
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
		memcpy(com2.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// ������1���ܻ����������ݿ���������2�ķ��ͻ�����
		com2.lenSend = com1.lenRec;
		OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
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
		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// ������2���ܻ����������ݿ���������1�ķ��ͻ�����
		com1.lenSend = com2.lenRec;
		OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
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

u8 son_mode_readaddr[] = {0xFE, 0xFE, 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x13, 0x00, 0xDF, 0x16};
u8 son_mode_readaddrrp[] = {0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x93, 0x06, 0x79, 0x33, 0x54, 0x63, 0x35, 0x33, 0xCD, 0x16};
u8 son_mode_readstatus[] = {0xFE, 0xFE, 0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x11, 0x04, 0x34, 0x48, 0x33, 0x37, 0x64, 0x16};
u8 son_mode_readstatusrp[] = {0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x91, 0x1F, 0x34, 0x48, 0x33, 0x37, 0x33, 0x33, 0x33, 0x33, 0xC4, 0x35, 0xCC, 0x53, 0x87, 0x8B, 0x33, 0x74, 0x8B, 0x33, 0x45, 0x34, 0x33, 0x33, 0x34, 0x34, 0xCC, 0x34, 0x33, 0x33, 0x32, 0x32, 0x32, 0x9D, 0x16};
u8 son_mode_readpower[] = {0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x11, 0x04, 0x33, 0x33, 0x34, 0x33, 0x4B, 0x16};
u8 son_mode_readpowerrp[] = {0x68, 0x33, 0x00, 0x81, 0x04, 0x00, 0x40, 0x00, 0x00, 0x47, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x01, 0x00, 0x01, 0x00, 0x02, 0x14, 0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x91, 0x08, 0x33, 0x33, 0x34, 0x33, 0x83, 0x9b, 0x33, 0x33, 0x53, 0x16, 0x8c, 0x16};
u8 son_mode_readpowerrp645[] = {0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x91, 0x08, 0x33, 0x33, 0x34, 0x33, 0x83, 0x9b, 0x33, 0x33, 0x53, 0x16};

u8 son_mode_enter[] = {0x68, 0x46, 0x00, 0x21, 0x30, 0x02, 0x00, 0x68, 0x94, 0x00, 0xFD, 0x16};

u8 son_test(INT32U timeout) {
	u8 err = 0;
	// static u8 times;
	OSSemPend(Com2_rx_sem, timeout, &err);// ��ȡ�ź���
	if (err == OS_ERR_NONE) {
		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// ������2���ܻ����������ݿ���������1�ķ��ͻ�����
		com1.lenSend = com2.lenRec;
		OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
		
		if (memcmp(com2.DMA_RX_BUF, son_mode_readaddr, com2.lenRec) == 0) {//����ַָ��
			memcpy(com2.DMA_TX_BUF, son_mode_readaddrrp, 18);// ����������2�ķ��ͻ�����
			com2.lenSend = 18;
			OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
		} else if (memcmp(son_mode_readstatus, com2.DMA_RX_BUF, com2.lenRec) == 0) {// ��״̬
			memcpy(com2.DMA_TX_BUF, son_mode_readstatusrp, sizeof(son_mode_readstatusrp));// ����������2�ķ��ͻ�����
			com2.lenSend = sizeof(son_mode_readstatusrp);
			OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
		} 
		else if (memcmp(son_mode_readpower, com2.DMA_RX_BUF, com2.lenRec - 2) == 0) {// �����й�
			
				memcpy(com2.DMA_TX_BUF, son_mode_readpowerrp645, sizeof(son_mode_readpowerrp645));// ����������2�ķ��ͻ�����
				com2.lenSend = sizeof(son_mode_readpowerrp645);
				OSTimeDlyHMSM(0, 0, 0, 500);
				OSSemPost(Com2_tx_sem);// ������Ϣ������2����ʼ�������ݡ�
			
		}
		return 1;
	} else {
		return 0;
	}
	
}

