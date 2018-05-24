#include "led.h"
#include "includes.h"
#include "relay.h"
#include "display.h"
#include "rtc.h"
#include "def.h"

extern u8 workStatus;
extern calendar STM32_calendar;


//Target2ÈÎÎñ
void Target2_task(void *pdata)
{	  
		u8 i=0;
		u8 time_temp=0;
			
		LED1(ON);
		OSTimeDlyHMSM(0, 0,1,0);
		LED2(ON);
		OSTimeDlyHMSM(0, 0,1,0);
		LED3(ON);
		OSTimeDlyHMSM(0, 0,1,0);
		LED4(ON);
		OSTimeDlyHMSM(0, 0,1,0);
		LED5(ON);
		OSTimeDlyHMSM(0, 0,1,0);
		LED1(OFF);
		LED2(OFF);
		LED3(OFF);
		LED4(OFF);
		LED5(OFF);
		while(1)
		{
			OSTimeDlyHMSM(0, 0,0,50);
			switch(workStatus)
			{
				case Status_Idle:
					RELAY1(OFF);
					RELAY2(OFF);
					LED1(OFF);
					LED2(OFF);
					LED3(OFF);
					LED4(OFF);
					break;
				case Status_PrepareCharge:
					RELAY1(OFF);
					RELAY2(OFF);
					break;
				case Status_Charge:

					if(time_temp!=STM32_calendar.sec)
					{
						time_temp=STM32_calendar.sec;
						switch(i++%5)
						{
							case 0:
								LED1(OFF);
								LED2(OFF);
								LED3(OFF);
								LED4(OFF);				
								break;
							case 1:
								LED1(ON);
								LED2(OFF);
								LED3(OFF);
								LED4(OFF);
								break;
							case 2:
								LED1(ON);
								LED2(ON);
								LED3(OFF);
								LED4(OFF);
								break;
							case 3:
								LED1(ON);
								LED2(ON);
								LED3(ON);
								LED4(OFF);
								break;
							case 4:
								LED1(ON);
								LED2(ON);
								LED3(ON);
								LED4(ON);	
								i=0;
								break;
							default:break;						
						}									
					}
					break;
				case Status_PreparePay:
					i=0;
					LED1(OFF);
					LED2(OFF);
					LED3(OFF);
					LED4(OFF);
					break;
				case Status_Pay:
					i=0;
					LED1(OFF);
					LED2(OFF);
					LED3(OFF);
					LED4(OFF);
					break;
				case Status_Query:
					RELAY1(OFF);
					RELAY2(OFF);
					break;
				case Status_Unusual:
					RELAY1(OFF);
					RELAY2(OFF);
					break;
				case Status_Manage:
					RELAY1(OFF);
					RELAY2(OFF);
					break;
				default:break;	
			}			
			
			
//			OSTimeDlyHMSM(0, 0,0,100);
//			LED2(ON);
//			OSTimeDlyHMSM(0, 0,0,100);
//			LED2(OFF);
		}
}
