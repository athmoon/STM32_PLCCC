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
	
	OSTimeDlyHMSM(0, 0,1,0);//等待设备稳定
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
	//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)ElePrice,12);

	// 设备自检
	device_test();
	OSTimeSet(0);
	
	workStatus = WorkStatus_Test;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); //任务调度延时
		
		receive_msg_from_com1();
		receive_msg_from_com4();
		
		// 进入状态机
		switch(workStatus) {
			case WorkStatus_Starting://启动状态
				// 如果模块连接成功，转入空闲状态
				break;
			case WorkStatus_Idle://空闲状态
				// 
				break;
			case WorkStatus_Error://异常状态
				break;
			case WorkStatus_Test://测试状态，调试使用
				// 如果接收到串口1的数据，转发给串口2
				OSSemPend(Com1_rx_sem, 0,&err);
				if(err == OS_ERR_NONE){
					memcpy(com2.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// 将串口1接受缓冲区的数据拷贝到串口2的发送缓冲区
					com2.lenSend = com1.lenRec;
					OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
				}
				// 如果接收到串口2的数据，转发给串口1
				OSSemPend(Com2_rx_sem, 0,&err);
				if(err == OS_ERR_NONE){
					memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// 将串口2接受缓冲区的数据拷贝到串口1的发送缓冲区
					com1.lenSend = com2.lenRec;
					OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
				}
				// 如果接收到串口4的数据，转发给串口2
//				OSSemPend(Com4_rx_sem, 0,&err);
//				if(err == OS_ERR_NONE){
//					memcpy(com2.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// 将串口4接受缓冲区的数据拷贝到串口2的发送缓冲区
//					com2.lenSend = com4.lenRec;
//					OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
//				}
				break;
			default:
				break;
		}
		
	}
}

// 设备自检
void device_test(void) {
	
}

// 接收串口1发来的消息
void receive_msg_from_com1(void) {
	
}

// 接收PLC模块(串口4)发来的消息
void receive_msg_from_com4(void) {
	
}

