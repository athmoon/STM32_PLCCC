#ifndef __CARD_H
#define	__CARD_H
#include "stm32f10x.h"

void IniCard(u8 *flag);
u8 ReadMessage(u8 a[],u8 b[],u8 *c,u8 d[],u8 e[] ,float *f);
u8 Charge(u8 a[], u8 b[], u8 d[], u8 e[]);
u8 Pay(u8 a[], u8 b[], u8 d[], u8 e[], float *f);
u8 Selfcheck_Card(void);

#endif
