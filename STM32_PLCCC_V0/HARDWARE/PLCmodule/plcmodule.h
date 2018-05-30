#ifndef __PLCMODULE_H
#define	__PLCMODULE_H

#include "includes.h"

#define PLC_STATE0 PCin(9)
#define PLC_RST(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);

void plcmodule_init(void);

#endif
