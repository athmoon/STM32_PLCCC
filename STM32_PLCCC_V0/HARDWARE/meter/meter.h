#ifndef __METER_H
#define	__METER_H
#include "stm32f10x.h"


typedef struct 
{
	u32 W;
	u16 U;
	u16 I;
}WUI;
u16 CRC16(unsigned char *puchMsg, unsigned char usDataLen);
char RecPakChk(unsigned char *buf,unsigned char LenOfRecPak);

u8 Search_meter(void);//查询电表读数
void meter_inspect(void);

u8 Selfcheck_meter(void);
#endif
