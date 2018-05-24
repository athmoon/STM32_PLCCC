#include "dma.h"
#include "includes.h"
#include "stm32f10x_dma.h"

/*******************************************************************************
DMA����
*******************************************************************************/

	    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ����� 

 u16 DMA1_MEM_LEN;


//����DMA �ڴ�->���� ģʽ
void DMA_MP_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	DMA1_MEM_LEN=cndtr;
  DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���  	 
} 
//����DMA���ͨ��
void DMA_Tx_Enable(u8 u)
{ 
	switch(u)
	{	
		case 1:
			DMA_Cmd(DMA1_Channel4, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel4,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel4, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		case 2:
			DMA_Cmd(DMA1_Channel7, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel7,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel7, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		case 3:
			DMA_Cmd(DMA1_Channel2, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel2,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel2, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		default: break;
	}	
	
}

//����DMA ����->�ڴ� ģʽ
void DMA_PM_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	
	DMA1_MEM_LEN=cndtr;
  DMA_DeInit(DMA_CHx);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  
	DMA_InitStructure.DMA_BufferSize = cndtr;  
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
	DMA_Init(DMA_CHx, &DMA_InitStructure);  	
} 
//����DMA����ͨ��
void DMA_Rx_Enable(u8 u)
{ 
	switch(u)
	{
		case 1:	
			DMA_Cmd(DMA1_Channel5, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel5,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel5, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		case 2:
			DMA_Cmd(DMA1_Channel6, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel6,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel6, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		case 3:
			DMA_Cmd(DMA1_Channel3, DISABLE );  //�ر�USART - DMA1 ��ָʾ��ͨ��      
			DMA_SetCurrDataCounter(DMA1_Channel3,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
			USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel3, ENABLE);  //ʹ��USART - DMA1 ��ָʾ��ͨ�� 
		break;
		default: break;
	}	
}
