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

#define PLCCmdTimeWait (5 * 100)
#define AFN10F3TimeWait (10 * 100)

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
	ConfigStatus_flashInit,
	ConfigStatus_readData,
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

u8 STATUS_NUM[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};

void device_test(void);
u8 receive_msg_from_com1(INT32U timeout);
u8 receive_msg_from_com2(INT32U timeout);
u8 receive_msg_from_com4(INT32U timeout);
void send_msg_to_com1(u8* msg, u8 len);
void send_msg_to_com4(u8* msg, u8 len);

u8 son_test(INT32U timeout);
void Target1_task(void *pdata)
{
	
	//u8 err = 0;
	u8 PLC_STATE0_Count = 0;
	u8 waiting = 0;
	
	OSTimeDlyHMSM(0, 0,1,0);//�ȴ��豸�ȶ�
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
	main_node_data_init();// ���ڵ���Ϣ��ʼ������Flashȡ����

	// �豸�Լ�
	device_test();
	OSTimeSet(0);
	
	workStatus = WorkStatus_Starting;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //���������ʱ
		
		// �жϽڵ�����״̬
		if (PLC_STATE0 == 1) {//plcģ��δ����
			PLC_STATE0_Count++;// ����ڵ�Ͽ����ӣ��ƴ�++
		} else {
			PLC_STATE0_Count = 0;
		}
		// ����Ͽ����ӳ���n�Σ��жϽڵ�Ͽ����ӣ������쳣�ȴ�������״̬
		if (PLC_STATE0 >= 30) {
			// ���ӶϿ���ʱ
		}
		// ����״̬��
		switch(workStatus) {
			case WorkStatus_Starting://����״̬
				if (PLC_STATE0 == 0) {// ���plcģ�������
					PLC_RST(0);// ��������10���븴λ״̬��Ȼ����Ϊ�ߵ�ƽ
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Config;// ģ�����ӳɹ���ת����Ϣ�˶ԣ����ڵ�����
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
						
						// �ж��Ƿ���Ҫ�ظ����������е�������Ҫ���лظ�
						if (readResult.rep_len != 0) {// �ظ����ݳ��Ȳ�Ϊ0�����лظ�
							// �������ݸ�PLCģ�飨com4��
							send_msg_to_com4(readResult.rep_buf, readResult.rep_len);
						}
						// 1.1���ж�flash�д洢�����ڵ���Ϣ�Ƿ��뵱ǰ���ڵ��ϱ�����Ϣ��ͬ
						if (readResult.action == Null_Action) {
							// �����ͬ�������κδ�������ڵ���Ϣ��ѯ״̬
							conf_status = ConfigStatus_readData;
						} else if (readResult.action == Hardware_Init) {// �������ͬ���г�ʼ������
							conf_status = ConfigStatus_addrinit;
						}
						
						TIMEOUTCHECK:
						if (OSTimeGet() >= AFN10F3TimeWait) {//����10�룬�����·�AFN03F10��ѯ����
							send_msg_to_com4(AFN03_F10, AFN03_F10[1]);
							OSTimeSet(0);
						}
						break;
						
					case ConfigStatus_addrinit:// ��ַ��ʼ��
						if (waiting == 0) {
							// 1�������������ڵ��ַָ������ڵ��ַ����Ϊ0
							send_msg_to_com4(AFN05_F1, AFN05_F1[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3�����޻ظ�����Ԥ��ʱ�䣬�ظ����͸�ָ��
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2�����ڵ�ظ��ɹ���������������ʼ��
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//������
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//У���
						if (!(readResult.AFN_code == AFN00 && readResult.F_code == F1)) break;//��AFN00F1ָ��
						
						// ��ַ���óɹ����������ݳ�ʼ��
						conf_status = ConfigStatus_datainit;
						waiting = 0;
						break;
					
					case ConfigStatus_datainit:// ��������ʼ��
						if (waiting == 0) {
							// 1��������������ʼ��ָ��
							send_msg_to_com4(AFN01_F3, AFN01_F3[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3�����޻ظ�����Ԥ��ʱ�䣬�ظ����͸�ָ��
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2�����ڵ�ظ��ɹ������²�ѯ�ڵ���Ϣ������flash
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//������
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//У���
						if (!(readResult.AFN_code == AFN00 && readResult.F_code == F1)) break;//��AFN00F1ָ��
						
						// ���ݳ�ʼ���ɹ��������ѯ�ڵ�����ģʽ��дflash״̬
						conf_status = ConfigStatus_flashInit;
						waiting = 0;
						break;
						
					case ConfigStatus_flashInit:
						// �·�AFN03F10ָ����µĽڵ���Ϣ�洢��flash
						if (waiting == 0) {
							// 1�����Ͳ�ѯ�ڵ�����ģʽָ��
							send_msg_to_com4(AFN03_F10, AFN03_F10[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3�����޻ظ�����Ԥ��ʱ�䣬�ظ����͸�ָ��
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2�����ڵ�ظ��ɹ�������flash�����ڽ��������д��룩
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//������
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//У���
						if (!(readResult.AFN_code == AFN03 && readResult.F_code == F10)) break;//��AFN03F10ָ��
						
						conf_status = ConfigStatus_readData;
						waiting = 0;
						break;
					
					case ConfigStatus_readData:// ���ڵ�����
						//���ڵ���Ϣ�洢��flash
						workStatus = WorkStatus_Test;
						break;
						
					default:
						break;
				}
				
				break;
			case WorkStatus_Idle://����״̬
				// ����յ��ز����ڵ�����֡�����н������������������֡�����лظ�������Ǳ�������֡����������ظ���λ��
				// ��ʱʹ�ô���2������λ����Ϣ��
				// ����յ���λ����ָ��Ƚ��н���������ǶԴӽڵ�Ŀ��ƻ��ѯָ�����376.2͸��ת��ָ��·����ز����ڵ�
				receive_msg_from_com2(DEV_TIMEOUT_10);
				break;
			case WorkStatus_Error://�쳣״̬
				break;
			case WorkStatus_Test://����״̬������ʹ��
				// ������յ�����1�����ݣ�ת��������4
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// ������յ�����4�����ݣ�ת��������1
				receive_msg_from_com4(DEV_TIMEOUT_10);
				// receive_msg_from_com2(DEV_TIMEOUT_10);
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
		
//		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);
//		com1.lenSend = com2.lenRec;
//		OSSemPost(Com1_tx_sem);
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
			memcpy(com1.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// ������4���ܻ����������ݿ���������1�ķ��ͻ�����
			com1.lenSend = com4.lenRec;
			OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
		return 1;
	} else {
		return 0;
	}
}

// ������Ϣ������1
void send_msg_to_com1(u8* msg, u8 len) {
	memcpy(com1.DMA_TX_BUF, msg, len);// �����ݿ���������1�ķ��ͻ�����
	com1.lenSend = len;
	OSSemPost(Com1_tx_sem);// ������Ϣ������1����ʼ�������ݡ�
}

// ������Ϣ������4
void send_msg_to_com4(u8* msg, u8 len) {
	memcpy(com4.DMA_TX_BUF, msg, len);// �����ݿ���������4�ķ��ͻ�����
	com4.lenSend = len;
	OSSemPost(Com4_tx_sem);// ������Ϣ������4����ʼ�������ݡ�
}
