#ifndef __DMA_H
#define	__DMA_H


#include "includes.h"




//DMA通道配置
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量
void DMA_MP_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//内存到外设
void DMA_Tx_Enable(u8 u);//开启DMA数据输出到外设
void DMA_PM_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//外设到内存
void DMA_Rx_Enable(u8 u);//开启DMA数据输出到内存

#endif
