#include "bsp.h"
#include "includes.h"

/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			    10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				      64
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	
 			   
//Target1任务
//设置任务优先级
#define Target1_TASK_PRIO       			1 
//设置任务堆栈大小
#define Target1_STK_SIZE  		    		128
//创建任务堆栈空间	
OS_STK Target1_TASK_STK[Target1_STK_SIZE];
//任务函数接口
void Target1_task(void *pdata);


//Target2任务
//设置任务优先级
#define Target2_TASK_PRIO       			2 
//设置任务堆栈大小
#define Target2_STK_SIZE  					  128
//创建任务堆栈空间	
OS_STK Target2_TASK_STK[Target2_STK_SIZE];
//任务函数接口
void Target2_task(void *pdata);


	
 int main(void)
 {
	BSP_init();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
 }

 
	  
//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 	OSTaskCreate(Target1_task,(void *)0,(OS_STK*)&Target1_TASK_STK[Target1_STK_SIZE-1],Target1_TASK_PRIO);						   
	OSTaskCreate(Target2_task,(void *)0,(OS_STK*)&Target2_TASK_STK[Target2_STK_SIZE-1],Target2_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}
