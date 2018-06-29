#ifndef __DISPLAY_H
#define	__DISPLAY_H
#include "stm32f10x.h"


void write_dmadata(u8 comNum, u8 len,u8 * buff);

u8 Selfcheck_display(u8 page);

u8 switch_page(u16 page);//ҳ���л�
void write_Reg(u8 regAdd,u16 dat);//д�Ĵ���
void read_Reg(u8 regAdd,u8 len);//���Ĵ���

u8 write_variable(u16 add,u32 dat);//�޸ı�����ֵ
void write_data(u16 add,u32 dat);
void read_Data(u16 Add,u8 len);//��ָ����ַ����

void write_Txt(u16 x,char Txt[]);//д�ı���Ϣ
u8 write_Text(u16 Add,char Txt[]);
void write_flash(u16 x,u16 buff[]);//��12�ε��д��flash

void Get_DwRtc(void);//��ȡ��ʾ����ǰʱ��
#endif
