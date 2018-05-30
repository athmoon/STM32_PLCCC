#include "usart.h"
#include "usart4.h"
#include "dma.h"
#include "includes.h"

extern USART_COM com4;

void uart4_init(u32 bound) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
//	RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART4|RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); 
	
	//UART4_TX   PC.10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
 
	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Uart4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;//抢占优先级4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART4, &USART_InitStructure); //初始化串口
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启中断
	USART_Cmd(UART4, ENABLE);
}

void UART4_IRQHandler(void)
{
	unsigned char num=0;

	OSIntEnter();
	
	if(USART_GetITStatus(UART4,USART_IT_IDLE) == SET)
	{
		 num = UART4->SR;
		 num = UART4->DR; 
		 DMA_Cmd(DMA2_Channel3,DISABLE); 
		 USART_DMACmd(UART4,USART_DMAReq_Rx,DISABLE);			
		 num =DMA_RX_BUFF_LEN-DMA_GetCurrDataCounter(DMA2_Channel3);
//	 com1.DMA_RX_BUF [num] = '\0';			
		 DMA2_Channel3->CNDTR=DMA_RX_BUFF_LEN;  		          
		 com4.lenRec=num;	
		 USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);	
		 DMA_Cmd(DMA2_Channel3,ENABLE);
		 OSSemPost(com4.sem_DMA_RX);
	}
	OSIntExit();
}
