#include "plcmodule.h"



void plcmodule_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);
		
	// PC9,STATE0,模块插入识别信号,插入为0,配置为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// PA8,RST,复位输出引脚,低电平复位,配置为推挽输出,初始化为低电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  // GPIO_SetBits(GPIOA, GPIO_Pin_8);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}
