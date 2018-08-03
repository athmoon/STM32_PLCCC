#include "includes.h"
#include "display.h"
#include "dma.h"
#include "usart.h"
#include "meter.h"
#include "rtc.h" 
#include "def.h"
#include "bsp.h"
#include "includes.h"
extern USART_COM com1,com2;
extern calendar DW_calendar;

extern u8 DMA_RX_BUF1[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF1[DMA_TX_BUFF_LEN];
extern u8 DMA_RX_BUF2[DMA_RX_BUFF_LEN];
extern u8 DMA_TX_BUF2[DMA_TX_BUFF_LEN];

static u8 wrtRegBuff[7]={HEAD1,HEAD2,0x04,WRITEREG,PAGEREGISTER,0x00,0x00};//寄存器查询指令
static u8 readRegBuff[6]={HEAD1,HEAD2,0x03,READREG,PAGEREGISTER,0x02};
static u8 wrtDateBuff[10]={HEAD1,HEAD2,0x07,WRITEDATA,0x00,0x00,0x00,0x00,0x00,0x00,};//写32位数据指令
static u8 readDateBuff[7]={HEAD1,HEAD2,0x04,READDATE,0x00,0x00,0x00};//读数据指令




void write_dmadata(u8 comNum, u8 len,u8 * buff)
{
	switch(comNum)
	{
		case 1:
			DMA_MP_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)buff,len);
			DMA_Tx_Enable(1);
			OSTimeDlyHMSM(0, 0,1,0);
			break;
		case 2:
			DMA_MP_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)buff,len);
			DMA_Tx_Enable(2);
			break;
			case 3:
			DMA_MP_Config(DMA1_Channel2,(u32)&USART3->DR,(u32)buff,len);
			DMA_Tx_Enable(3);
			break;
		default: break;
	}		
}

void write_Reg(u8 regAdd,u16 dat)
{
		int i;
		wrtRegBuff[4]=regAdd;
		wrtRegBuff[5]=dat>>8;
		wrtRegBuff[6]=dat;//页面地址
		for(i=0;i<7;i++)//发送查询指令
		{
			USART_SendData(USART1, (u8) wrtRegBuff[i]);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
		}
}

void read_Reg(u8 regAdd,u8 len)//读寄存器
{
		int i;
		readRegBuff[4]=regAdd;
		readRegBuff[5]=len;
		for(i=0;i<6;i++)//发送查询指令
		{
			USART_SendData(USART1, (u8) readRegBuff[i]);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
		}
}

u8 switch_page(u16 page)//切换页面
{
	int i;
	u8 flag=FALSE;
	u8 err;
	u8 regAdd=0x03;
	u8 dataLen=2;   
	for(i=0;i<20;i++)
	{
		write_Reg(regAdd,page);
		read_Reg(regAdd,dataLen);
		
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT,&err);
		if(err==OS_ERR_NONE)
		{
			if(DMA_RX_BUF1[0]==HEAD1 && DMA_RX_BUF1[1]==HEAD2)
			{
				if(DMA_RX_BUF1[2]==0x05 && DMA_RX_BUF1[3]==READREG)
				{
					if(DMA_RX_BUF1[4]==regAdd && DMA_RX_BUF1[5]==dataLen)
					{
						if(DMA_RX_BUF1[6]==0x00 && DMA_RX_BUF1[7]==page)
						{
							if(page==0x00)
							{
								OSTimeSet(0);
							}
							flag=TRUE;
							break;
						}
					}
				}
			}
		}
	}
	return flag;
}
void write_flash(u16 x,u16 buff[])
{
			int i;
			u8 buff1[24];
			wrtDateBuff[2]=27;
			wrtDateBuff[4]=x>>8;
			wrtDateBuff[5]=x;
			buff1[0]=buff[0]<<8;
			for(i=0;i<12;i++)
			{
				buff1[2*i]=buff[i]>>8;
				buff1[2*i+1]=buff[i];
			}
			for(i=0;i<30;i++)//发送查询指令
			{
				if(i<6)
				{
				USART_SendData(USART1, (u8) wrtDateBuff[i]);
				}else
				{
				USART_SendData(USART1, (u8) buff1[i-6]);
				}
				while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
			}
		OSTimeDlyHMSM(0, 0,0,50);
}
void write_data(u16 add,u32 dat)
{
		int i;
		wrtDateBuff[2]=7;
		wrtDateBuff[4]=add>>8;
		wrtDateBuff[5]=add;
		wrtDateBuff[6]=dat>>24;
		wrtDateBuff[7]=dat>>16;
		wrtDateBuff[8]=dat>>8;
		wrtDateBuff[9]=dat;
		for(i=0;i<10;i++)
		{
			USART_SendData(USART1, (u8) wrtDateBuff[i]);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
		}

}

void read_Data(u16 Add,u8 len)
{
		int i;
		readDateBuff[4]=Add>>8;
		readDateBuff[5]=Add;
		readDateBuff[6]=len;
		for(i=0;i<7;i++)//发送查询指令
		{
			USART_SendData(USART1, (u8) readDateBuff[i]);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
		}

}

void write_Txt(u16 add,char Txt[])//写文本信息
{			
			u8 i=0;
			u8 size=0;
			u8 buff[26];
			size=strlen(Txt);	
			buff[0]=0x5A;
			buff[1]=0xA5;
			buff[2]=size+3;
			buff[3]=0x82;
			buff[4]=add>>8;
			buff[5]=add;
				for(i=0;i<size;i++)
				{
					buff[i+6]=Txt[i];
				}
				for(i=0;i<size+6;i++)//发送查询指令
				{
					USART_SendData(USART1, (u8) buff[i]);
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}//等待传送结束
				}
}

u8 write_variable(u16 add,u32 dat)//修改变量数值
{
	u8 i;
	u8 flag=FALSE;
	u8 err;
	for(i=0;i<20;i++)
	{
		write_data(add,dat);
		read_Data(add,2);
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT,&err);
		if(err==OS_ERR_NONE)
		{
			if(DMA_RX_BUF1[0]==HEAD1 && DMA_RX_BUF1[1]==HEAD2)
			{
				if(DMA_RX_BUF1[2]==0x08 && DMA_RX_BUF1[3]==READDATE)
				{
					if((DMA_RX_BUF1[4]<<8)+DMA_RX_BUF1[5]==add)
					{
						if(DMA_RX_BUF1[6]==0x02)
						{						
							if(((DMA_RX_BUF1[7]<<24)+(DMA_RX_BUF1[8]<<16)+(DMA_RX_BUF1[9]<<8)+DMA_RX_BUF1[10])==dat)
							{
								flag=TRUE;
								break;
							}
						}
					}
				}
			}
		}
	}
	return flag;
}



u8 write_Text(u16 Add,char Txt[])
{
	u8 i=0,j=0;
	u8 err=0;
	u8 flag=FALSE;
	u8 size=strlen(Txt);
	char buff[30];
	for(i=0;i<10;i++)
	{
		write_Txt(Add,Txt);
		read_Data(Add,size/2);
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT,&err);
		if(err==OS_ERR_NONE)
		{
			if(DMA_RX_BUF1[0]==HEAD1 && DMA_RX_BUF1[1]==HEAD2)
			{
				if(DMA_RX_BUF1[2]==(size+4) && DMA_RX_BUF1[3]==READDATE)
				{
					if((DMA_RX_BUF1[4]<<8)+DMA_RX_BUF1[5]==Add)
					{
						if(DMA_RX_BUF1[6]==size/2)
						{	
							for(j=0;j<size;j++)
							{
							buff[j]=DMA_RX_BUF1[j+7];
							}
							if(strncmp(Txt,buff,size)==0)
							{
								flag=TRUE;
								break;
							}	
						}
					}
				}
			}
		}
	}
	return flag;
}

void Get_DwRtc(void)//获取显示屏当前时间
{			
			u8 err;
			u8 regAdd=0x20;
			u8 dataLen=7;
			read_Reg(regAdd,dataLen);
			OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT,&err);
			DW_calendar.w_year=2000+(DMA_RX_BUF1[6]/16*10+DMA_RX_BUF1[6]%16);
			DW_calendar.w_month=DMA_RX_BUF1[7]/16*10+DMA_RX_BUF1[7]%16;
			DW_calendar.w_date=DMA_RX_BUF1[8]/16*10+DMA_RX_BUF1[8]%16;
			DW_calendar.week=DMA_RX_BUF1[9]/16*10+DMA_RX_BUF1[9]%16;
			DW_calendar.hour=DMA_RX_BUF1[10]/16*10+DMA_RX_BUF1[10]%16;
			DW_calendar.min=DMA_RX_BUF1[11]/16*10+DMA_RX_BUF1[11]%16;
			DW_calendar.sec=DMA_RX_BUF1[12]/16*10+DMA_RX_BUF1[12]%16;
}

u8 Selfcheck_display(u8 page)
{
	u8 i=0;
	u8 err=0;
	u8 regAdd=0x03;
	u8 dataLen=2;
	u8 temp=0;
	for(i=0;i<10;i++)
	{
		read_Reg(regAdd,dataLen);	
		OSSemPend(com1.sem_DMA_RX,DEV_TIMEOUT,&err);
		if(err==OS_ERR_NONE)
		{
			if(DMA_RX_BUF1[0]==HEAD1 && DMA_RX_BUF1[1]==HEAD2)
			{
				if(DMA_RX_BUF1[2]==0x05 && DMA_RX_BUF1[3]==READREG)
				{
					if(DMA_RX_BUF1[4]==regAdd && DMA_RX_BUF1[5]==dataLen)
					{
						if(DMA_RX_BUF1[6]==0x00 && DMA_RX_BUF1[7]==page)
						{
							temp++;
						}
					}
				}
			}
		}
	}
	return temp;
}
