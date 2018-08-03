#include "led.h"
#include "includes.h"




//Target3хннЯ
void Target3_task(void *pdata)
{	  
    while (1)
    {
			OSTimeDlyHMSM(0, 0,0,100);
			LED3(ON);
			OSTimeDlyHMSM(0, 0,0,100);
			LED3(OFF);
		}
}
