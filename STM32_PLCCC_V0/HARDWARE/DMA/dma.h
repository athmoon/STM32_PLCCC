#ifndef __DMA_H
#define	__DMA_H


#include "includes.h"




//DMAͨ������
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ�����
void DMA_MP_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//�ڴ浽����
void DMA_Tx_Enable(u8 u);//����DMA�������������
void DMA_PM_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//���赽�ڴ�
void DMA_Rx_Enable(u8 u);//����DMA����������ڴ�

#endif
