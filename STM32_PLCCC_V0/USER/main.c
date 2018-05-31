#include "bsp.h"
#include "includes.h"
#include "led.h"
#include "sys.h"
#include "usart.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "usart4.h"
#include "dma.h"
#include "rtc.h"
#include "flash.h"
#include "interrupt.h"
#include "def.h"

//////////////////////////////Ӳ����ʼ������//////////////////////////////////
extern u8 DMA_RX_BUF1[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF1[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF2[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF2[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF3[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF3[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF4[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF4[DMA_TX_BUFF_LEN];
extern USART_COM com1,com2,com3,com4;

extern OS_EVENT * Com1_rx_sem;
extern OS_EVENT * Com1_tx_sem;
extern OS_EVENT * Com2_rx_sem;
extern OS_EVENT * Com2_tx_sem;
extern OS_EVENT * Com4_rx_sem;
extern OS_EVENT * Com4_tx_sem;
void OSSem_init(void);

void usart_init(void);

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


//Target3����
//�����������ȼ�
#define Target3_TASK_PRIO       			3 
//���������ջ��С
#define Target3_STK_SIZE  					  128
//���������ջ�ռ�	
OS_STK Target3_TASK_STK[Target3_STK_SIZE];
//�������ӿ�
void Target3_task(void *pdata);


//Target4����
//�����������ȼ�
#define Target4_TASK_PRIO       			4 
//���������ջ��С
#define Target4_STK_SIZE  					  128
//���������ջ�ռ�	
OS_STK Target4_TASK_STK[Target4_STK_SIZE];
//�������ӿ�
void Target4_task(void *pdata);

	
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
	
	//Ӳ����ʼ��
	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)  
 	OSTaskCreate(Target1_task,(void *)0,(OS_STK*)&Target1_TASK_STK[Target1_STK_SIZE-1],Target1_TASK_PRIO);						   
	OSTaskCreate(Target2_task,(void *)0,(OS_STK*)&Target2_TASK_STK[Target2_STK_SIZE-1],Target2_TASK_PRIO);
	OSTaskCreate(Target3_task,(void *)0,(OS_STK*)&Target3_TASK_STK[Target3_STK_SIZE-1],Target3_TASK_PRIO);
	OSTaskCreate(Target4_task,(void *)0,(OS_STK*)&Target4_TASK_STK[Target4_STK_SIZE-1],Target4_TASK_PRIO);
	
	usart_init();//���ڳ�ʼ��
	OSSem_init();//�ź�����ʼ��
	
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

void OSSem_init(void) {
	Com1_rx_sem = OSSemCreate(0);
	Com1_tx_sem = OSSemCreate(0);
	Com2_rx_sem = OSSemCreate(0);
	Com2_tx_sem = OSSemCreate(0);
	Com4_rx_sem = OSSemCreate(0);
	Com4_tx_sem = OSSemCreate(0);
}

void usart_init(void) {
	usart1_init(115200);//���ڳ�ʼ��
	usart2_init(9600);
	usart3_init(9600);
	uart4_init(9600);

	Com1_init();//���ڻ�������ʼ��
	Com2_init();
	Com3_init();
	Com4_init();
	//dma���ճ�ʼ��
	DMA_PM_Config(DMA1_Channel5,(u32)&USART1->DR,(u32)DMA_RX_BUF1, DMA_RX_BUFF_LEN);
	DMA_Rx_Enable(1);
	DMA_PM_Config(DMA1_Channel6,(u32)&USART2->DR,(u32)com2.DMA_RX_BUF, DMA_RX_BUFF_LEN);
	DMA_Rx_Enable(2);
	DMA_PM_Config(DMA1_Channel3,(u32)&USART3->DR,(u32)com3.DMA_RX_BUF, DMA_RX_BUFF_LEN);
	DMA_Rx_Enable(3);
	DMA_PM_Config(DMA2_Channel3,(u32)&UART4->DR,(u32)com4.DMA_RX_BUF, DMA_RX_BUFF_LEN);
	DMA_Rx_Enable(4);
	// �ȶ�һ�±�־λ����ֹ��һ֡���ݵ��׸��ֽڷ��Ͷ�ʧ
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	USART_GetFlagStatus(USART3, USART_FLAG_TC);
	USART_GetFlagStatus(UART4, USART_FLAG_TC);
}
