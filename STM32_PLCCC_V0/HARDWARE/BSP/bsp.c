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
	delay_init();	 //延时初始化
	//LED_init();		//初始化与LED连接的硬件接口
	plcmodule_init();
	
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级		
//	EXTI_Config();//系统外部中断初始化
	
}
