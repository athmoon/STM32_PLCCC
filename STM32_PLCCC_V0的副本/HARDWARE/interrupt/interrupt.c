#include "interrupt.h"
#include "led.h"
#include "relay.h"
#include "includes.h"
#include "def.h"


u8 INPUT1=0;



void EXTI_Config(void)
{
        
        NVIC_InitTypeDef NVIC_InitStructure ;        
        EXTI_InitTypeDef EXTI_InitStructure;     
        GPIO_InitTypeDef GPIO_InitStructure;
        
        //PB3-6 enable Exti3-6
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	      GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
        GPIO_Init(GPIOB, &GPIO_InitStructure);   
   
       
	   //≈‰÷√÷–∂œ‘¥PB3
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
        
        EXTI_ClearITPendingBit(EXTI_Line4);
        EXTI_InitStructure.EXTI_Line = EXTI_Line4;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
        
        NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;                              
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;        
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                          
        NVIC_Init(&NVIC_InitStructure);                                                            
}



void EXTI4_IRQHandler(void)
{
	    OSIntEnter(); 
        if(EXTI_GetITStatus(EXTI_Line4) != RESET)
          {  
						if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)==Bit_RESET)
						  {
								 INPUT1=1;
							}
						else 
							{
								 INPUT1=0;
							}        
            EXTI_ClearITPendingBit(EXTI_Line4);               		
          }
		 OSIntExit(); 
}





