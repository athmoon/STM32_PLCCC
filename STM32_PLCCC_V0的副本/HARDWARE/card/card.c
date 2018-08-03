#include "includes.h"
#include "usart.h"
#include "card.h"
#include "led.h"
#include "display.h"
#include "meter.h"
#include "def.h"

union floatToByte
{
	float gold;
  u8    byteM[4];	
};

//第四块为管理段，包括：状态(1)、充电时间(6)、结算时间(6)
//第五块为数据段，包括：金额(浮点数4字节)
#define DEV_REC_LEN			128	//定义接收缓冲区大小
#define DEV_SEND_LEN		32	//定义发送缓冲区大小

extern u8 DMA_RX_BUF2[DEV_REC_LEN];
extern u8 DMA_TX_BUF2[DEV_SEND_LEN];



static u8 Scan[8]={0x08,0x02,0x4d,0x02,0x00,0x26,0x9c,0x03};//扫描卡片
static u8 CheckKey[18]={0x12,0x02,0x46,0x0c,0x60,0x01,0x02,0x03,0x04,0xff,0xff,0xff,0xff,0xff,0xff,0x04,0xff,0x03};//密钥验证
static u8 ReadCard[7]={0x07,0x02,0x47,0x01,0xff,0xff,0x03};//读卡数据
static u8 WriteCard[23]={0x17,0x02,0x48,0x11,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03};//写卡数据

extern char Start_normal[];
extern char Start_abnormal[];
	
////public  WriteCrad
//u8 CardId[4];
//float money;
//float paymoney;
//u8 CardStatus;//记录卡片的工作状态
//u8 ChargeTime[6]={0xff,0xff,0xff,0xff,0xff,0xff};//加电时间
//u8 PayTime[6]={0xff,0xff,0xff,0xff,0xff,0xff};//结算时间
//u8 time[6]={0x20,0x16,0x10,0x14,0x13,0x31};//实时时间
//u8 flagAct=0;
#define LimitMoney 0//定义最低限制消费

extern USART_COM com2;//定义关于串口2的使用


// BCC校验
void BCC(u8 SerBfr[])
{
	int i=0;
	u8 BCC = 0;
	for (i = 0;i < (SerBfr[0] - 2);i++) 
	{
		BCC ^= SerBfr[i];
	}
	SerBfr[SerBfr[0]-2] = ~BCC;
}

//接收数据BCC校验
u8 RecBCCChk(u8 buf[]) 
{ 
	u8 i=0;
	u8 Chkchar[32];
	for(i=0;i<buf[0];i++)Chkchar[i]=buf[i];//复制需校验数据
	BCC(Chkchar);
	if(Chkchar[buf[0]-2]==buf[buf[0]-2]) 
		return 1;
	else                                 
		return 0;
}


//寻卡，返回成功标志，带出卡号
u8 searchCard(u8 Id[])
{ 
	u8 flagPassSearch=0;
	int i;
	u8 err=0;
	for(i=0;i<8;i++)com2.DMA_TX_BUF[i]=Scan[i];
	write_dmadata(2,8,com2.DMA_TX_BUF);//发送激活卡片指令
	OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待刷卡器响应
	if(err==OS_ERR_NONE)
	{
		if(com2.lenRec==com2.DMA_RX_BUF[0])//应答长度正确
		{ 
			if(RecBCCChk(com2.DMA_RX_BUF))//BCC校验正确
			{
			if(com2.DMA_RX_BUF[2]==0)//应答状态对
			{
				for(i=0;i<4;i++)
				Id[i]=com2.DMA_RX_BUF[i+8];
				flagPassSearch=1;
			}
		  }
	  }
	}
	return flagPassSearch;
}
//验卡，返回成功标志，
u8 checkCard(u8 Id[],u8 key[])
{
	int i;
	u8 flagPassKey=0;
	u8 err;
	for(i=0;i<18;i++)com2.DMA_TX_BUF[i]=CheckKey[i];//复制基本数据
	for(i=0;i<4;i++)com2.DMA_TX_BUF[i+5]=Id[i];//填写卡片号
	for(i=0;i<6;i++)com2.DMA_TX_BUF[i+9]=key[i];//填写卡片密码
	BCC(com2.DMA_TX_BUF);
	write_dmadata(2,18,com2.DMA_TX_BUF);//发送激活卡片指令
	OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待刷卡器响应
	if(err==OS_ERR_NONE)
	{
		if(com2.lenRec==com2.DMA_RX_BUF[0])//应答长度正确
		{ 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCC校验正确
			{
				if(com2.DMA_RX_BUF[2]==0)//应答状态对
				{
					flagPassKey=1;
				}
			}
	  }
	}
	return flagPassKey;
}
	u8 readCard(u8 area,u8 data[])
	{
		int i;
		u8 flagPassRead=0;
		u8 err;
		
		for(i=0;i<7;i++)com2.DMA_TX_BUF[i]=ReadCard[i];//复制基本数据
		com2.DMA_TX_BUF[4]=area;//填写读卡的区域
		BCC(com2.DMA_TX_BUF);
		write_dmadata(2,7,com2.DMA_TX_BUF);//发送读卡数据指令
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待读卡器相应
		if(err==OS_ERR_NONE)
    {  if(com2.lenRec==com2.DMA_RX_BUF[0])//应答长度正确
		    { 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCC校验正确
			    {
				if(com2.DMA_RX_BUF[2]==0)//应答状态对
				    {
				    	flagPassRead=1;
							for(i=0;i<=16;i++) data[i]=com2.DMA_RX_BUF[i+4];					
						} 
			    }
		    }
		}
		return flagPassRead;
	}
	u8 writeCard(u8 area ,u8 dat[])
	{ int i;
		u8 flagPassWrite=0;
		u8 err;
		for(i=0;i<23;i++)com2.DMA_TX_BUF[i]=WriteCard[i];//复制基本数据
		com2.DMA_TX_BUF[4]=area;//填写读卡的区域
		for(i=0;i<16;i++)com2.DMA_TX_BUF[i+5]=dat[i];
		BCC(com2.DMA_TX_BUF);
		write_dmadata(2,23,com2.DMA_TX_BUF);//发送读卡数据指令
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待读卡器相应
		if(err==OS_ERR_NONE)
    {  if(com2.lenRec==com2.DMA_RX_BUF[0])//应答长度正确
		    { 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCC校验正确
			    {
				if(com2.DMA_RX_BUF[2]==0)//应答状态对
				    {
				    	flagPassWrite=1;
            }
          }
        }
		 }	
    return flagPassWrite;
	}

//读卡器上电匹配波特率
void IniCard(u8 *flag)
 { 
	 u8 i=0;
	 u8 err;	 
	 com2.DMA_TX_BUF[0]=0x20;
			for(i=0;i<10;i++)
		{
			write_dmadata(2,1,com2.DMA_TX_BUF);//发送激活指令第一次
			OSTimeDlyHMSM(0, 0,0,10);
			write_dmadata(2,1,com2.DMA_TX_BUF);//发送激活指令第二次
			OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待刷卡器响应
				if(err==OS_ERR_NONE)
				{
					if(com2.DMA_RX_BUF[0]==0x06)  
					{						
						*flag=1;
						break;
					}
				}
		}
		if(*flag)
		{
			flag=0;
			write_Text(0x0610,Start_normal);
		}
		else 
		{
		write_Text(0x0610,Start_abnormal);
		}
}
 
u8 Selfcheck_Card(void)
{
	u8 i=0;
	u8 err=0;
	u8 temp=0;
	com2.DMA_TX_BUF[0]=0x06;
	com2.DMA_TX_BUF[1]=0x01;
	com2.DMA_TX_BUF[2]=0x41;
	com2.DMA_TX_BUF[3]=0x00;
	com2.DMA_TX_BUF[4]=0xB9;
	com2.DMA_TX_BUF[5]=0x03;
	for(i=0;i<10;i++)
	{
		write_dmadata(2,6,com2.DMA_TX_BUF);
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//等待刷卡器响应
		if(err==OS_ERR_NONE)
		{
			if(com2.DMA_RX_BUF[2]==0x00)  
			{		
				temp++;
			}
		}
	}
	return temp;
}


//读卡信息
u8 ReadMessage(u8 a[],u8 b[],u8 *c,u8 d[],u8 e[] ,float *f)//a 卡号，b 密钥，c卡状态，d加电时间，e结算时间
  { int i;
		union floatToByte moneyTemp;
		u8 g[4]={0};
		u8 Area4Data[16]={0};
    u8 Area5Data[16]={0};
		u8 flagMessage=0;
		if(searchCard(a))
					{
						if(checkCard(a,b))
						{					
							if(readCard(4,Area4Data))//读状态
							{ 
								*c=Area4Data[0];//卡状态							
								for(i=0;i<6;i++) d[i]=Area4Data[i+1];//加电时间
								for(i=0;i<6;i++) e[i]=Area4Data[i+7];//结算时间
								if(readCard(5,Area5Data))//读金额
								{
									for(i=0;i<4;i++)g[i]=Area5Data[i];
									moneyTemp.byteM[0]=g[0];
									moneyTemp.byteM[1]=g[1];
									moneyTemp.byteM[2]=g[2];
									moneyTemp.byteM[3]=g[3];
								  *f=moneyTemp.gold;
                  flagMessage=1;									
								}										
							}																			
						}
					}		
     return flagMessage;					
	}

//加电操作
u8 Charge(u8 a[], u8 b[], u8 d[], u8 e[])//a 卡号，b 密钥,d 结算时间，e 实时时间
{  int i=0;
   u8 temp[16]={0};
   u8 flagCharge=0;	 
	 if(searchCard(a))
				{
					if(checkCard(a,b))
					{ for(i=0;i<=16;i++)temp[i]=0;
						temp[0]=1;//修改为允许结算
						for(i=0;i<=6;i++)temp[i+1]=e[i];//修改加电时间
						for(i=0;i<=6;i++)temp[i+7]=d[i];//复制结算时间
						if(writeCard(4,temp)) flagCharge=1;												
					}
				}	
   return flagCharge;				
}
//支付操作
u8 Pay(u8 a[], u8 b[], u8 d[], u8 e[], float *f)//a 卡号，b 密钥,d 加电时间，e 实时时间, f 结算需写入的金额
{  int i=0;
	u8 temp[16]={0};
	u8 g[4]={0};
	u8 flagPay=0;
	union floatToByte PaymoneyTemp;
	if(searchCard(a))
		{
			if(checkCard(a,b))
			{ 
				temp[0]=2;//修改为允许加电
				for(i=0;i<=6;i++)temp[i+1]=d[i];//复制加电时间
				for(i=0;i<=6;i++)temp[i+7]=e[i];//修改结算时间
				if(writeCard(4,temp))
				{ for(i=0;i<=16;i++)temp[i]=0;
					PaymoneyTemp.gold=*f;
					g[0]=PaymoneyTemp.byteM[0];
					g[1]=PaymoneyTemp.byteM[1];
					g[2]=PaymoneyTemp.byteM[2];
					g[3]=PaymoneyTemp.byteM[3];
					for(i=0;i<=4;i++)temp[i]=g[i];	//需要写入的金额							
					if(writeCard(5,temp)) flagPay=1;                         							  
				}							
			}
		}
	return flagPay;
}
