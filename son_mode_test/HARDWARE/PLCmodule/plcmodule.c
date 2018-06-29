#include "plcmodule.h"



void plcmodule_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);
		
	// PC9,STATE0,ģ�����ʶ���ź�,����Ϊ0,����Ϊ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// PA8,RST,��λ�������,�͵�ƽ��λ,����Ϊ�������,��ʼ��Ϊ�͵�ƽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  // GPIO_SetBits(GPIOA, GPIO_Pin_8);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}
