#include "usart.h"
#include "usart2.h"
#include "dma.h"
#include "includes.h"


extern USART_COM com2;

void usart2_init(u32 bound)
	{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2��GPIOAʱ��
     //USART1_TX   PA.2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
   
    //USART1_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	}
	
	
	void USART2_IRQHandler(void)
{
		unsigned char num=0;
	
		OSIntEnter();
    
    if(USART_GetITStatus(USART2,USART_IT_IDLE) == SET)
    {
       num = USART2->SR;
       num = USART2->DR;
			 USART_ClearITPendingBit(USART3, USART_IT_IDLE);			
       DMA_Cmd(DMA1_Channel6,DISABLE);
			 USART_DMACmd(USART2,USART_DMAReq_Rx,DISABLE);
       num =DMA_RX_BUFF_LEN-DMA_GetCurrDataCounter(DMA1_Channel6);       
		   com2.DMA_RX_BUF [num] = '\0';
       DMA1_Channel6->CNDTR=DMA_RX_BUFF_LEN;         
			 com2.lenRec =num;
			 USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);			
			 DMA_Cmd(DMA1_Channel6,ENABLE);
			 OSSemPost(com2.sem_DMA_RX );
    }
		OSIntExit();
}
