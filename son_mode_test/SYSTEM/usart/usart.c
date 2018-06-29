#include "usart.h"

USART_COM com1,com2,com3;

USART_COM com4;

u8 DMA_RX_BUF1[DMA_RX_BUFF_LEN];
u8 DMA_TX_BUF1[DMA_TX_BUFF_LEN];
u8 DMA_RX_BUF2[DMA_RX_BUFF_LEN];
u8 DMA_TX_BUF2[DMA_TX_BUFF_LEN];	
u8 DMA_RX_BUF3[DMA_RX_BUFF_LEN];
u8 DMA_TX_BUF3[DMA_TX_BUFF_LEN];
u8 DMA_RX_BUF4[DMA_RX_BUFF_LEN];
u8 DMA_TX_BUF4[DMA_TX_BUFF_LEN];	
	
	void Com1_init(void)
{
	com1.sem_DMA_RX=OSSemCreate(0);
	com1.lenRecMax=DMA_RX_BUFF_LEN;
	com1.DMA_RX_BUF=DMA_RX_BUF1;
	com1.DMA_TX_BUF=DMA_TX_BUF1;
	com1.lenRec=0;
	com1.lenSend=0;	
}
	
	void Com2_init(void)
{
	com2.sem_DMA_RX=OSSemCreate(0);
	com2.lenRecMax=DMA_RX_BUFF_LEN;
	com2.DMA_RX_BUF=DMA_RX_BUF2;
	com2.DMA_TX_BUF=DMA_TX_BUF2;
	com2.lenRec=0;
	com2.lenSend=0;
}

	void Com3_init(void)
{
	com3.sem_DMA_RX=OSSemCreate(0);
	com3.lenRecMax=DMA_RX_BUFF_LEN;
	com3.DMA_RX_BUF=DMA_RX_BUF3;
	com3.DMA_TX_BUF=DMA_TX_BUF3;
	com3.lenRec=0;
	com3.lenSend=0;
}

	void Com4_init(void)
{
	com4.sem_DMA_RX=OSSemCreate(0);
	com4.lenRecMax=DMA_RX_BUFF_LEN;
	com4.DMA_RX_BUF=DMA_RX_BUF4;
	com4.DMA_TX_BUF=DMA_TX_BUF4;
	com4.lenRec=0;
	com4.lenSend=0;
}
