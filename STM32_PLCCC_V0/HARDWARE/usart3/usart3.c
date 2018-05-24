#include "usart.h"
#include "usart3.h"
#include "dma.h"
#include "includes.h"
#include "def.h"

extern USART_COM com3;

void usart3_init(u32 bound)
	{
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Ê¹ÄÜUSART3£¬GPIOAÊ±ÖÓ
		
		//usart3Í¨¹ý485Í¨ÐÅ£¬Í¨¹ýB1À´¿ØÖÆ·½Ïò
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
		
     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   //Usart3 NVIC ÅäÖÃ

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//×ÓÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
		NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷
  
   //USART ³õÊ¼»¯ÉèÖÃ

		USART_InitStructure.USART_BaudRate = bound;//Ò»°ãÉèÖÃÎª9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
		USART_InitStructure.USART_Parity = USART_Parity_Even;//ÎÞÆæÅ¼Ð£ÑéÎ»
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½

    USART_Init(USART3, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//¿ªÆôÖÐ¶Ï
    USART_Cmd(USART3, ENABLE);                    //Ê¹ÄÜ´®¿Ú 
	}
	void usart3R_init(u32 bound)
	{
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Ê¹ÄÜUSART3£¬GPIOAÊ±ÖÓ
		
		//usart3Í¨¹ý485Í¨ÐÅ£¬Í¨¹ýB1À´¿ØÖÆ·½Ïò
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
		
     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   //Usart3 NVIC ÅäÖÃ

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//×ÓÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
		NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷
  
   //USART ³õÊ¼»¯ÉèÖÃ

		USART_InitStructure.USART_BaudRate = bound;//Ò»°ãÉèÖÃÎª9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
		USART_InitStructure.USART_Parity = USART_Parity_Even;//ÎÞÆæÅ¼Ð£ÑéÎ»
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
		USART_InitStructure.USART_Mode = USART_Mode_Rx;//ÊÕ·¢Ä£Ê½

    USART_Init(USART3, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//¿ªÆôÖÐ¶Ï
    USART_Cmd(USART3, ENABLE);                    //Ê¹ÄÜ´®¿Ú 
	}
	void usart3T_init(u32 bound)
	{
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Ê¹ÄÜUSART3£¬GPIOAÊ±ÖÓ
		
		//usart3Í¨¹ý485Í¨ÐÅ£¬Í¨¹ýB1À´¿ØÖÆ·½Ïò
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
		
     //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

   //Usart3 NVIC ÅäÖÃ

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//ÇÀÕ¼ÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//×ÓÓÅÏÈ¼¶3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
		NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷
  
   //USART ³õÊ¼»¯ÉèÖÃ

		USART_InitStructure.USART_BaudRate = bound;//Ò»°ãÉèÖÃÎª9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//×Ö³¤Îª8Î»Êý¾Ý¸ñÊ½
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
		USART_InitStructure.USART_Parity = USART_Parity_Even;//ÎÞÆzæÅ¼Ð£ÑéÎ»
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎÞÓ²¼þÊý¾ÝÁ÷¿ØÖÆ
		USART_InitStructure.USART_Mode =USART_Mode_Tx;	//ÊÕ·¢Ä£Ê½

    USART_Init(USART3, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
//    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//¿ªÆôÖÐ¶Ï
    USART_Cmd(USART3, ENABLE);                    //Ê¹ÄÜ´®¿Ú 
	}
	
	
	void USART3_IRQHandler(void)
{
		unsigned char num=0;
	
		OSIntEnter();
    
    if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)
    {
       num = USART3->SR;
       num = USART3->DR; 
//			 USART_ClearITPendingBit(USART3, USART_IT_IDLE);
       DMA_Cmd(DMA1_Channel3,DISABLE); 
			 USART_DMACmd(USART3,USART_DMAReq_Rx,DISABLE);			
       num =DMA_RX_BUFF_LEN-DMA_GetCurrDataCounter(DMA1_Channel3);
			 com3.DMA_RX_BUF [num] = '\0';			
       DMA1_Channel3->CNDTR=DMA_RX_BUFF_LEN;  		          
			 com3.lenRec=num;	
			 USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);	
			 DMA_Cmd(DMA1_Channel3,ENABLE);
			 OSSemPost(com3.sem_DMA_RX );
    }
		OSIntExit();
}
