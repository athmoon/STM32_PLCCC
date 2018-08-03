#include "includes.h"
#include "def.h"
#include "usart.h"
#include "server_cmd.h"

extern USART_COM com2;
extern OS_EVENT * Com2_tx_sem;
extern ServerCmd com2_cmd;

void send_to_com2(u8 *StrBuf, u8 bufLen);
//Target4����
void Target4_task(void *pdata)
{	  
	u8 err = 0;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);
		
		// ��һ�������ܴ���2ָ��
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			// �ڶ�����ָ����� + ָ��ת��
			// У��֡ͷ
			if (check_data_header(com2.DMA_RX_BUF, com2.lenRec) != 0) {
				// У��֡β
				if (check_data_tail(com2.DMA_RX_BUF, com2.lenRec) != 0) {
					// У��ͼ���
					if (check_data_sum(com2.DMA_RX_BUF, com2.lenRec) != 0) {
						// ��������
						translate_to_serverCmd(&com2_cmd, com2.DMA_RX_BUF, com2.lenRec);
						// �����������ݲ������ж��Ƿ���Ҫת�������ڵ��ӽڵ㣩
						data_handle(&com2_cmd);
					}
				}
			}
		}
		
		// �������� �ȴ��ظ������ؽ��
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
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}//�ȴ����ͽ���
	}
}

