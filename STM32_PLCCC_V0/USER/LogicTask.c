#include "led.h"
#include "sys.h"
#include "usart.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "includes.h"
#include "dma.h"
#include "rtc.h"
#include "flash.h"
#include "interrupt.h"
#include "def.h"



extern u8 DMA_RX_BUF1[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF1[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF2[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF2[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF3[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF3[DMA_TX_BUFF_LEN];
extern USART_COM com1,com2,com3;
extern calendar STM32_calendar;



u8 workStatus;



void Target1_task(void *pdata)
{	
		u8 err = 0;
	
		usart1_init(115200);//串口初始化
		usart2_init(9600);
		usart3_init(9600);
	
		Com1_init();//串口缓冲区初始化
		Com2_init();
		Com3_init();
		//dma接收初始化
		DMA_PM_Config(DMA1_Channel5,(u32)&USART1->DR,(u32)DMA_RX_BUF1,DMA_RX_BUFF_LEN);
		DMA_Rx_Enable(1);
		DMA_PM_Config(DMA1_Channel6,(u32)&USART2->DR,(u32)com2.DMA_RX_BUF ,DMA_RX_BUFF_LEN);
		DMA_Rx_Enable(2);
		DMA_PM_Config(DMA1_Channel3,(u32)&USART3->DR,(u32)com3.DMA_RX_BUF ,DMA_RX_BUFF_LEN);
		DMA_Rx_Enable(3);
		
	
		OSTimeDlyHMSM(0, 0,1,0);//等待设备稳定
		
		
		RTC_Init();
		
		//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
		//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
		
		OSTimeSet(0);
		while(1)
		{
			OSTimeDlyHMSM(0, 0, 0, 10); //任务调度延时
			//1等待用户操作
			OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT_10,&err);
			
		}
}

