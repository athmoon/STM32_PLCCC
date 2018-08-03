#include "includes.h"
#include "def.h"
#include "usart.h"
#include "server_cmd.h"

extern USART_COM com2;
extern OS_EVENT * Com2_tx_sem;
extern ServerCmd com2_cmd;

void send_to_com2(u8 *StrBuf, u8 bufLen);
//Target4任务
void Target4_task(void *pdata)
{	  
	u8 err = 0;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1);
		
		// 第一步，接受串口2指令
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT_10,&err);
		if(err == OS_ERR_NONE){
			// 第二步，指令解析 + 指令转义
			// 校验帧头
			if (check_data_header(com2.DMA_RX_BUF, com2.lenRec) != 0) {
				// 校验帧尾
				if (check_data_tail(com2.DMA_RX_BUF, com2.lenRec) != 0) {
					// 校验和检验
					if (check_data_sum(com2.DMA_RX_BUF, com2.lenRec) != 0) {
						// 解析数据
						translate_to_serverCmd(&com2_cmd, com2.DMA_RX_BUF, com2.lenRec);
						// 第三步、数据操作（判断是否需要转发给主节点或从节点）
						data_handle(&com2_cmd);
					}
				}
			}
		}
		
		// 第三步， 等待回复并返回结果
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
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}//等待传送结束
	}
}

