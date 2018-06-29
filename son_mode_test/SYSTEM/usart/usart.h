#ifndef __USART_H
#define __USART_H
#include "includes.h"

#define DMA_RX_BUFF_LEN 128
#define DMA_TX_BUFF_LEN 128

typedef struct 
{
	u8 lenRec;
	u8 lenSend;
	u8 lenRecMax;
	u8 lenSendMax;
	u8 * DMA_RX_BUF;
	u8 * DMA_TX_BUF;
	OS_EVENT * sem_DMA_RX;
}USART_COM;




void Com1_init(void);
void Com2_init(void);
void Com3_init(void);
void Com4_init(void);

#endif

