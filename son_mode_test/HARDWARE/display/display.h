#ifndef __DISPLAY_H
#define	__DISPLAY_H
#include "stm32f10x.h"


void write_dmadata(u8 comNum, u8 len,u8 * buff);

u8 Selfcheck_display(u8 page);

u8 switch_page(u16 page);//页面切换
void write_Reg(u8 regAdd,u16 dat);//写寄存器
void read_Reg(u8 regAdd,u8 len);//读寄存器

u8 write_variable(u16 add,u32 dat);//修改变量数值
void write_data(u16 add,u32 dat);
void read_Data(u16 Add,u8 len);//读指定地址数据

void write_Txt(u16 x,char Txt[]);//写文本信息
u8 write_Text(u16 Add,char Txt[]);
void write_flash(u16 x,u16 buff[]);//将12段电价写入flash

void Get_DwRtc(void);//获取显示屏当前时间
#endif
