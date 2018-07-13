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
	
	OSTimeDlyHMSM(0, 0,1,0);//等待设备稳定
	
	RTC_Init();
	
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
	main_node_data_init();// 主节点信息初始化（从Flash取出）

	// 设备自检
	device_test();
	OSTimeSet(0);
	
	workStatus = WorkStatus_Starting;
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 1); //任务调度延时
		
		// 判断节点连接状态
		if (PLC_STATE0 == 1) {//plc模块未插入
			PLC_STATE0_Count++;// 如果节点断开连接，计次++
		} else {
			PLC_STATE0_Count = 0;
		}
		// 如果断开连接超过n次，判断节点断开连接，进入异常等待或启动状态
		if (PLC_STATE0 >= 30) {
			// 连接断开超时
		}
		// 进入状态机
		switch(workStatus) {
			case WorkStatus_Starting://启动状态
				if (PLC_STATE0 == 0) {// 如果plc模块插入了
					PLC_RST(0);// 保持最少10毫秒复位状态，然后置为高电平
					OSTimeDlyHMSM(0, 0, 0, 50);
					PLC_RST(1);
					workStatus = WorkStatus_Config;// 模块连接成功，转入信息核对，主节点配置
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
						
						// 判断是否需要回复，主动上行的数据需要进行回复
						if (readResult.rep_len != 0) {// 回复数据长度不为0，进行回复
							// 发送数据给PLC模块（com4）
							send_msg_to_com4(readResult.rep_buf, readResult.rep_len);
						}
						// 1.1、判断flash中存储的主节点信息是否与当前主节点上报的信息相同
						if (readResult.action == Null_Action) {
							// 如果相同，不做任何处理，进入节点信息查询状态
							conf_status = ConfigStatus_readData;
						} else if (readResult.action == Hardware_Init) {// 如果不相同进行初始化操作
							conf_status = ConfigStatus_addrinit;
						}
						
						TIMEOUTCHECK:
						if (OSTimeGet() >= AFN10F3TimeWait) {//超过10秒，主动下发AFN03F10查询命令
							send_msg_to_com4(AFN03_F10, AFN03_F10[1]);
							OSTimeSet(0);
						}
						break;
						
					case ConfigStatus_addrinit:// 地址初始化
						if (waiting == 0) {
							// 1、发送设置主节点地址指令，将主节点地址设置为0
							send_msg_to_com4(AFN05_F1, AFN05_F1[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3、若无回复超过预设时间，重复发送该指令
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2、主节点回复成功，进入数据区初始化
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//无数据
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//校验错
						if (!(readResult.AFN_code == AFN00 && readResult.F_code == F1)) break;//非AFN00F1指令
						
						// 地址设置成功，进入数据初始化
						conf_status = ConfigStatus_datainit;
						waiting = 0;
						break;
					
					case ConfigStatus_datainit:// 数据区初始化
						if (waiting == 0) {
							// 1、发送数据区初始化指令
							send_msg_to_com4(AFN01_F3, AFN01_F3[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3、若无回复超过预设时间，重复发送该指令
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2、主节点回复成功，重新查询节点信息，存入flash
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//无数据
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//校验错
						if (!(readResult.AFN_code == AFN00 && readResult.F_code == F1)) break;//非AFN00F1指令
						
						// 数据初始化成功，进入查询节点运行模式，写flash状态
						conf_status = ConfigStatus_flashInit;
						waiting = 0;
						break;
						
					case ConfigStatus_flashInit:
						// 下发AFN03F10指令，将新的节点信息存储到flash
						if (waiting == 0) {
							// 1、发送查询节点运行模式指令
							send_msg_to_com4(AFN03_F10, AFN03_F10[1]);
							waiting = 1;
							OSTimeSet(0);
						} else {
							// 3、若无回复超过预设时间，重复发送该指令
							if (OSTimeGet() > PLCCmdTimeWait) {
								waiting = 0;
							}
						}
						// 2、主节点回复成功，存入flash（已在解析函数中存入）
						if (!receive_msg_from_com4(DEV_TIMEOUT_10)) break;//无数据
						if (read_3762_str(com4.DMA_RX_BUF, com4.lenRec)) break;//校验错
						if (!(readResult.AFN_code == AFN03 && readResult.F_code == F10)) break;//非AFN03F10指令
						
						conf_status = ConfigStatus_readData;
						waiting = 0;
						break;
					
					case ConfigStatus_readData:// 读节点数据
						//将节点信息存储到flash
						workStatus = WorkStatus_Test;
						break;
						
					default:
						break;
				}
				
				break;
			case WorkStatus_Idle://空闲状态
				// 如果收到载波主节点上行帧，进行解析，如果是主动上行帧，进行回复；如果是被动上行帧，将结果返回给上位机
				// 暂时使用串口2接受上位机信息。
				// 如果收到上位机的指令，先进行解析，如果是对从节点的控制或查询指令，按照376.2透传转发指令，下发给载波主节点
				receive_msg_from_com2(DEV_TIMEOUT_10);
				break;
			case WorkStatus_Error://异常状态
				break;
			case WorkStatus_Test://测试状态，调试使用
				// 如果接收到串口1的数据，转发给串口4
				receive_msg_from_com1(DEV_TIMEOUT_10);
				
				// 如果接收到串口4的数据，转发给串口1
				receive_msg_from_com4(DEV_TIMEOUT_10);
				// receive_msg_from_com2(DEV_TIMEOUT_10);
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
		
//		memcpy(com1.DMA_TX_BUF, com2.DMA_RX_BUF, com2.lenRec);
//		com1.lenSend = com2.lenRec;
//		OSSemPost(Com1_tx_sem);
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
			memcpy(com1.DMA_TX_BUF, com4.DMA_RX_BUF, com4.lenRec);// 将串口4接受缓冲区的数据拷贝到串口1的发送缓冲区
			com1.lenSend = com4.lenRec;
			OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
		return 1;
	} else {
		return 0;
	}
}

// 发送信息给串口1
void send_msg_to_com1(u8* msg, u8 len) {
	memcpy(com1.DMA_TX_BUF, msg, len);// 将数据拷贝到串口1的发送缓冲区
	com1.lenSend = len;
	OSSemPost(Com1_tx_sem);// 发送消息给串口1，开始发送数据。
}

// 发送信息给串口4
void send_msg_to_com4(u8* msg, u8 len) {
	memcpy(com4.DMA_TX_BUF, msg, len);// 将数据拷贝到串口4的发送缓冲区
	com4.lenSend = len;
	OSSemPost(Com4_tx_sem);// 发送消息给串口4，开始发送数据。
}
