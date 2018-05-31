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
					// workStatus = WorkStatus_Config;// 模块连接成功，转入信息核对，主节点配置
				}
				break;
			case WorkStatus_Config://主节点配置状态
				// 1、判断是否有AFN03F10主动上行数据,如果有，对数据进行解析
				// 1.1、
				// 2、如果没有，等待是否超过20s，超过20s主动发送查询命令
				break;
			case WorkStatus_Idle://空闲状态
				// 
				break;
			case WorkStatus_Error://异常状态
				break;
			case WorkStatus_Test://测试状态，调试使用
				// 如果接收到串口1的数据，转发给串口4
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// 如果接收到串口4的数据，转发给串口2
				receive_msg_from_com4(DEV_TIMEOUT_10);
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
u8 receive_msg_from_com1(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com1_rx_sem, timeout, &err);// 读取信号量
	if (err == OS_ERR_NONE) {
		memcpy(com4.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// 将串口1接受缓冲区的数据拷贝到串口4的发送缓冲区
		com4.lenSend = com1.lenRec;
		OSSemPost(Com4_tx_sem);// 发送消息给串口4，开始发送数据。
		return 1;
	} else {
		return 0;
	}
	
}

// 返回1，接收数据成功，返回0，接收数据失败或数据为空
u8 receive_msg_from_com2(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com2_rx_sem, timeout, &err);// 读取信号量
	if (err == OS_ERR_NONE) {
		memcpy(com4.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// 将串口2接受缓冲区的数据拷贝到串口4的发送缓冲区
		com4.lenSend = com2.lenRec;
		OSSemPost(Com4_tx_sem);// 发送消息给串口4，开始发送数据。
		return 1;
	} else {
		return 0;
	}
	
}

// 接收PLC模块(串口4)发来的消息
u8 receive_msg_from_com4(INT32U timeout) {
	u8 err = 0;
	
	OSSemPend(Com4_rx_sem, timeout, &err);// 读取信号量
	if (err == OS_ERR_NONE) {

//		if (memcmp(com4.DMA_RX_BUF, plc_cmd, com4.lenRec) == 0) {
//			com4.lenSend = plc_cmd_reply[1] + (plc_cmd_reply[2]<<8);
//			memcpy(com4.DMA_TX_BUF, plc_cmd_reply, com4.lenSend);
//			OSSemPost(Com4_tx_sem);// 发送消息给串口4，开始发送数据。
//			
//			com1.lenSend = plc_cmd_reply[1] + (plc_cmd_reply[2]<<8);
//			memcpy(com1.DMA_TX_BUF, plc_cmd_reply, com1.lenSend);
//			OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
//		} else {
			memcpy(com1.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// 将串口4接受缓冲区的数据拷贝到串口2的发送缓冲区
			com1.lenSend = com4.lenRec;
			OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
//		}
		return 1;
	} else {
		return 0;
	}
}

