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
	
	workStatus = WorkStatus_Starting;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //任务调度延时
		
		
		// 进入状态机
		switch(workStatus) {
			case WorkStatus_Starting://启动状态
				if (PLC_STATE0 == 0) {// 如果plc模块插入了
					PLC_RST(0);// 保持最少10毫秒复位状态，然后置为高电平
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Test;// 模块连接、复位成功，转入测试
				}
				break;
			case WorkStatus_Idle://空闲状态
				// 
				break;
			case WorkStatus_Error://异常状态
				break;
			case WorkStatus_Test://测试状态，调试使用
				// 如果接收到串口2的数据，转发给串口4
				OSSemPend(Com2_rx_sem, DEV_TIMEOUT_10,&err);
				if(err == OS_ERR_NONE){
					memcpy(com4.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// 将串口2接受缓冲区的数据拷贝到串口4的发送缓冲区
					com4.lenSend = com2.lenRec;
					OSSemPost(Com4_tx_sem);// 发送消息给串口4，开始发送数据。
				}
				
				// 如果接收到串口4的数据，转发给串口2
				OSSemPend(Com4_rx_sem, DEV_TIMEOUT_10,&err);
				if(err == OS_ERR_NONE){
					memcpy(com2.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// 将串口4接受缓冲区的数据拷贝到串口2的发送缓冲区
					com2.lenSend = com4.lenRec;
					OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
				}
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

