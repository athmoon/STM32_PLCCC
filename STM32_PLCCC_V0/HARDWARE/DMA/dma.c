#include "dma.h"
#include "includes.h"
#include "stm32f10x_dma.h"

/*******************************************************************************
DMA配置
*******************************************************************************/

	    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量 

 u16 DMA1_MEM_LEN;


//配置DMA 内存->外设 模式
void DMA_MP_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	DMA1_MEM_LEN=cndtr;
  DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = cndtr;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器  	 
} 
//开启DMA输出通道
void DMA_Tx_Enable(u8 u)
{ 
	switch(u)
	{	
		case 1:
			DMA_Cmd(DMA1_Channel4, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel4,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel4, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		case 2:
			DMA_Cmd(DMA1_Channel7, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel7,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel7, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		case 3:
			DMA_Cmd(DMA1_Channel2, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel2,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
			DMA_Cmd(DMA1_Channel2, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		default: break;
	}	
	
}

//配置DMA 外设->内存 模式
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
//开启DMA接收通道
void DMA_Rx_Enable(u8 u)
{ 
	switch(u)
	{
		case 1:	
			DMA_Cmd(DMA1_Channel5, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel5,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel5, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		case 2:
			DMA_Cmd(DMA1_Channel6, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel6,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel6, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		case 3:
			DMA_Cmd(DMA1_Channel3, DISABLE );  //关闭USART - DMA1 所指示的通道      
			DMA_SetCurrDataCounter(DMA1_Channel3,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
			USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
			DMA_Cmd(DMA1_Channel3, ENABLE);  //使能USART - DMA1 所指示的通道 
		break;
		default: break;
	}	
}
