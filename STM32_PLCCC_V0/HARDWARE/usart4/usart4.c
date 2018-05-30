#include "usart.h"
#include "usart4.h"
#include "dma.h"
#include "includes.h"

extern USART_COM com4;

void uart4_init(u32 bound) {
	//GPIO�˿�����
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//Uart4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;//��ռ���ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); //��ʼ������
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//�����ж�
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
