#include "bsp.h"
#include "delay.h"
#include "led.h"
#include "sys.h"
#include "relay.h"
#include "interrupt.h"
#include "rtc.h" 
#include "gun.h"
#include "plcmodule.h"
void BSP_init(void)
{
	delay_init();	 //��ʱ��ʼ��
	//LED_init();		//��ʼ����LED���ӵ�Ӳ���ӿ�
	plcmodule_init();
	
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�		
//	EXTI_Config();//ϵͳ�ⲿ�жϳ�ʼ��
	
}
