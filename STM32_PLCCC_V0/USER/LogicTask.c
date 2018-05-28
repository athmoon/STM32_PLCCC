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
		
		// OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
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
				// 如果接收到串口1的数据，转发给串口4
				// 如果接收到串口4的数据，转发给串口1
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

// 接收PLC模块发来的消息
void receive_msg_from_com4(void) {
	
}

