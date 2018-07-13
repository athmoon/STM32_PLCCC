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
	
	OSTimeDlyHMSM(0, 0,1,0);//等待设备稳定
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);

	// 设备自检
	device_test();
	OSTimeSet(0);
	
	// workStatus = WorkStatus_Starting;
	workStatus = WorkStatus_SonTest;
	// workStatus = WorkStatus_Test;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //任务调度延时
		
		// 判断节点连接状态
		// 如果节点断开连接，计次++
		// 如果断开连接超过n次，判断节点断开连接，进入异常等待或启动状态
		
		// 进入状态机
		switch(workStatus) {
			case WorkStatus_Starting://启动状态
				if (PLC_STATE0 == 0) {// 如果plc模块插入了
					PLC_RST(0);// 保持最少10毫秒复位状态，然后置为高电平
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Test;// 模块连接、复位成功，转入测试
					// workStatus = WorkStatus_Config;// 模块连接成功，转入信息核对，主节点配置
					conf_status = ConfigStatus_link;
				}
				break;
			case WorkStatus_Config://主节点配置状态
				
				switch (conf_status) {
					case ConfigStatus_link:
						// 1、判断是否有AFN03F10主动上行数据,如果有，对数据进行解析
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) goto TIMEOUTCHECK;//无数据
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) goto TIMEOUTCHECK;//校验错
						if (!(readResult.AFN_code == AFN03 && readResult.F_code == F10)) goto TIMEOUTCHECK;//非AFN03F10指令
						// 1.1、判断flash中存储的主节点信息是否与当前主节点上报的信息相同
						if (readResult.action == Null_Action) {
							// 如果相同，不做任何处理
						} else if (readResult.action == Hardware_Init) {// 如果不相同进行初始化操作
							conf_status = ConfigStatus_addrinit;
						}
						
						TIMEOUTCHECK:
						if (OSTimeGet() >= 20) {//超过20秒，主动下发AFN03F10
							
						}
						break;
						
					case ConfigStatus_addrinit:// 地址初始化
						// 1、发送设置主节点地址指令，将主节点地址设置为1
						// 2、等待主节点回复，若无回复超过预设时间，重复发送该指令
						// 3、主节点回复成功，进入数据区初始化
						break;
					
					case ConfigStatus_datainit:// 数据区初始化
						break;
						
					default:
						break;
				}
				
				break;
			case WorkStatus_Idle://空闲状态
				// 如果收到载波主节点上行帧，进行解析，如果是主动上行帧，进行回复；如果是被动上行帧，将结果返回给上位机
				// 如果收到上位机的指令，按照376.2透传转发指令，下发给载波主节点
				break;
			case WorkStatus_Error://异常状态
				break;
			case WorkStatus_Test://测试状态，调试使用
				// 如果接收到串口1的数据，转发给串口2
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// 如果接收到串口2的数据，转发给串口1
				// receive_msg_from_com4(DEV_TIMEOUT_10);
				receive_msg_from_com2(DEV_TIMEOUT_10);
				break;
			case WorkStatus_SonTest:
				son_test(DEV_TIMEOUT_10);
				// 如果接收到串口1的数据，转发给串口2
				receive_msg_from_com1(DEV_TIMEOUT_10);
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
		memcpy(com2.DMA_TX_BUF, com1.DMA_RX_BUF, com1.lenRec);// 将串口1接受缓冲区的数据拷贝到串口2的发送缓冲区
		com2.lenSend = com1.lenRec;
		OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
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
		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// 将串口2接受缓冲区的数据拷贝到串口1的发送缓冲区
		com1.lenSend = com2.lenRec;
		OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
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
	OSSemPend(Com2_rx_sem, timeout, &err);// 读取信号量
	if (err == OS_ERR_NONE) {
		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);// 将串口2接受缓冲区的数据拷贝到串口1的发送缓冲区
		com1.lenSend = com2.lenRec;
		OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
		
		if (memcmp(com2.DMA_RX_BUF, son_mode_readaddr, com2.lenRec) == 0) {//读地址指令
			memcpy(com2.DMA_TX_BUF, son_mode_readaddrrp, 18);// 拷贝到串口2的发送缓冲区
			com2.lenSend = 18;
			OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
		} else if (memcmp(son_mode_readstatus, com2.DMA_RX_BUF, com2.lenRec) == 0) {// 读状态
			memcpy(com2.DMA_TX_BUF, son_mode_readstatusrp, sizeof(son_mode_readstatusrp));// 拷贝到串口2的发送缓冲区
			com2.lenSend = sizeof(son_mode_readstatusrp);
			OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
		} 
		else if (memcmp(son_mode_readpower, com2.DMA_RX_BUF, com2.lenRec - 2) == 0) {// 读正有功
			
				memcpy(com2.DMA_TX_BUF, son_mode_readpowerrp645, sizeof(son_mode_readpowerrp645));// 拷贝到串口2的发送缓冲区
				com2.lenSend = sizeof(son_mode_readpowerrp645);
				OSTimeDlyHMSM(0, 0, 0, 500);
				OSSemPost(Com2_tx_sem);// 发送消息给串口2，开始发送数据。
			
		}
		return 1;
	} else {
		return 0;
	}
	
}

