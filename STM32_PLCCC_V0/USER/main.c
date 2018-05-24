#include "bsp.h"
#include "includes.h"

/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			    10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				      64
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);	
 			   
//Target1����
//�����������ȼ�
#define Target1_TASK_PRIO       			1 
//���������ջ��С
#define Target1_STK_SIZE  		    		128
//���������ջ�ռ�	
OS_STK Target1_TASK_STK[Target1_STK_SIZE];
//�������ӿ�
void Target1_task(void *pdata);


//Target2����
//�����������ȼ�
#define Target2_TASK_PRIO       			2 
//���������ջ��С
#define Target2_STK_SIZE  					  128
//���������ջ�ռ�	
OS_STK Target2_TASK_STK[Target2_STK_SIZE];
//�������ӿ�
void Target2_task(void *pdata);


	
 int main(void)
 {
	BSP_init();
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();
 }

 
	  
//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(Target1_task,(void *)0,(OS_STK*)&Target1_TASK_STK[Target1_STK_SIZE-1],Target1_TASK_PRIO);						   
	OSTaskCreate(Target2_task,(void *)0,(OS_STK*)&Target2_TASK_STK[Target2_STK_SIZE-1],Target2_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}
