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

//���Ŀ�Ϊ����Σ�������״̬(1)�����ʱ��(6)������ʱ��(6)
//�����Ϊ���ݶΣ����������(������4�ֽ�)
#define DEV_REC_LEN			128	//������ջ�������С
#define DEV_SEND_LEN		32	//���巢�ͻ�������С

extern u8 DMA_RX_BUF2[DEV_REC_LEN];
extern u8 DMA_TX_BUF2[DEV_SEND_LEN];



static u8 Scan[8]={0x08,0x02,0x4d,0x02,0x00,0x26,0x9c,0x03};//ɨ�迨Ƭ
static u8 CheckKey[18]={0x12,0x02,0x46,0x0c,0x60,0x01,0x02,0x03,0x04,0xff,0xff,0xff,0xff,0xff,0xff,0x04,0xff,0x03};//��Կ��֤
static u8 ReadCard[7]={0x07,0x02,0x47,0x01,0xff,0xff,0x03};//��������
static u8 WriteCard[23]={0x17,0x02,0x48,0x11,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x03};//д������

extern char Start_normal[];
extern char Start_abnormal[];
	
////public  WriteCrad
//u8 CardId[4];
//float money;
//float paymoney;
//u8 CardStatus;//��¼��Ƭ�Ĺ���״̬
//u8 ChargeTime[6]={0xff,0xff,0xff,0xff,0xff,0xff};//�ӵ�ʱ��
//u8 PayTime[6]={0xff,0xff,0xff,0xff,0xff,0xff};//����ʱ��
//u8 time[6]={0x20,0x16,0x10,0x14,0x13,0x31};//ʵʱʱ��
//u8 flagAct=0;
#define LimitMoney 0//���������������

extern USART_COM com2;//������ڴ���2��ʹ��


// BCCУ��
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

//��������BCCУ��
u8 RecBCCChk(u8 buf[]) 
{ 
	u8 i=0;
	u8 Chkchar[32];
	for(i=0;i<buf[0];i++)Chkchar[i]=buf[i];//������У������
	BCC(Chkchar);
	if(Chkchar[buf[0]-2]==buf[buf[0]-2]) 
		return 1;
	else                                 
		return 0;
}


//Ѱ�������سɹ���־����������
u8 searchCard(u8 Id[])
{ 
	u8 flagPassSearch=0;
	int i;
	u8 err=0;
	for(i=0;i<8;i++)com2.DMA_TX_BUF[i]=Scan[i];
	write_dmadata(2,8,com2.DMA_TX_BUF);//���ͼ��Ƭָ��
	OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ�ˢ������Ӧ
	if(err==OS_ERR_NONE)
	{
		if(com2.lenRec==com2.DMA_RX_BUF[0])//Ӧ�𳤶���ȷ
		{ 
			if(RecBCCChk(com2.DMA_RX_BUF))//BCCУ����ȷ
			{
			if(com2.DMA_RX_BUF[2]==0)//Ӧ��״̬��
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
//�鿨�����سɹ���־��
u8 checkCard(u8 Id[],u8 key[])
{
	int i;
	u8 flagPassKey=0;
	u8 err;
	for(i=0;i<18;i++)com2.DMA_TX_BUF[i]=CheckKey[i];//���ƻ�������
	for(i=0;i<4;i++)com2.DMA_TX_BUF[i+5]=Id[i];//��д��Ƭ��
	for(i=0;i<6;i++)com2.DMA_TX_BUF[i+9]=key[i];//��д��Ƭ����
	BCC(com2.DMA_TX_BUF);
	write_dmadata(2,18,com2.DMA_TX_BUF);//���ͼ��Ƭָ��
	OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ�ˢ������Ӧ
	if(err==OS_ERR_NONE)
	{
		if(com2.lenRec==com2.DMA_RX_BUF[0])//Ӧ�𳤶���ȷ
		{ 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCCУ����ȷ
			{
				if(com2.DMA_RX_BUF[2]==0)//Ӧ��״̬��
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
		
		for(i=0;i<7;i++)com2.DMA_TX_BUF[i]=ReadCard[i];//���ƻ�������
		com2.DMA_TX_BUF[4]=area;//��д����������
		BCC(com2.DMA_TX_BUF);
		write_dmadata(2,7,com2.DMA_TX_BUF);//���Ͷ�������ָ��
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ���������Ӧ
		if(err==OS_ERR_NONE)
    {  if(com2.lenRec==com2.DMA_RX_BUF[0])//Ӧ�𳤶���ȷ
		    { 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCCУ����ȷ
			    {
				if(com2.DMA_RX_BUF[2]==0)//Ӧ��״̬��
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
		for(i=0;i<23;i++)com2.DMA_TX_BUF[i]=WriteCard[i];//���ƻ�������
		com2.DMA_TX_BUF[4]=area;//��д����������
		for(i=0;i<16;i++)com2.DMA_TX_BUF[i+5]=dat[i];
		BCC(com2.DMA_TX_BUF);
		write_dmadata(2,23,com2.DMA_TX_BUF);//���Ͷ�������ָ��
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ���������Ӧ
		if(err==OS_ERR_NONE)
    {  if(com2.lenRec==com2.DMA_RX_BUF[0])//Ӧ�𳤶���ȷ
		    { 
				if(RecBCCChk(com2.DMA_RX_BUF))//BCCУ����ȷ
			    {
				if(com2.DMA_RX_BUF[2]==0)//Ӧ��״̬��
				    {
				    	flagPassWrite=1;
            }
          }
        }
		 }	
    return flagPassWrite;
	}

//�������ϵ�ƥ�䲨����
void IniCard(u8 *flag)
 { 
	 u8 i=0;
	 u8 err;	 
	 com2.DMA_TX_BUF[0]=0x20;
			for(i=0;i<10;i++)
		{
			write_dmadata(2,1,com2.DMA_TX_BUF);//���ͼ���ָ���һ��
			OSTimeDlyHMSM(0, 0,0,10);
			write_dmadata(2,1,com2.DMA_TX_BUF);//���ͼ���ָ��ڶ���
			OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ�ˢ������Ӧ
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
		OSSemPend(com2.sem_DMA_RX,DEV_TIMEOUT,&err);//�ȴ�ˢ������Ӧ
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


//������Ϣ
u8 ReadMessage(u8 a[],u8 b[],u8 *c,u8 d[],u8 e[] ,float *f)//a ���ţ�b ��Կ��c��״̬��d�ӵ�ʱ�䣬e����ʱ��
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
							if(readCard(4,Area4Data))//��״̬
							{ 
								*c=Area4Data[0];//��״̬							
								for(i=0;i<6;i++) d[i]=Area4Data[i+1];//�ӵ�ʱ��
								for(i=0;i<6;i++) e[i]=Area4Data[i+7];//����ʱ��
								if(readCard(5,Area5Data))//�����
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

//�ӵ����
u8 Charge(u8 a[], u8 b[], u8 d[], u8 e[])//a ���ţ�b ��Կ,d ����ʱ�䣬e ʵʱʱ��
{  int i=0;
   u8 temp[16]={0};
   u8 flagCharge=0;	 
	 if(searchCard(a))
				{
					if(checkCard(a,b))
					{ for(i=0;i<=16;i++)temp[i]=0;
						temp[0]=1;//�޸�Ϊ�������
						for(i=0;i<=6;i++)temp[i+1]=e[i];//�޸ļӵ�ʱ��
						for(i=0;i<=6;i++)temp[i+7]=d[i];//���ƽ���ʱ��
						if(writeCard(4,temp)) flagCharge=1;												
					}
				}	
   return flagCharge;				
}
//֧������
u8 Pay(u8 a[], u8 b[], u8 d[], u8 e[], float *f)//a ���ţ�b ��Կ,d �ӵ�ʱ�䣬e ʵʱʱ��, f ������д��Ľ��
{  int i=0;
	u8 temp[16]={0};
	u8 g[4]={0};
	u8 flagPay=0;
	union floatToByte PaymoneyTemp;
	if(searchCard(a))
		{
			if(checkCard(a,b))
			{ 
				temp[0]=2;//�޸�Ϊ����ӵ�
				for(i=0;i<=6;i++)temp[i+1]=d[i];//���Ƽӵ�ʱ��
				for(i=0;i<=6;i++)temp[i+7]=e[i];//�޸Ľ���ʱ��
				if(writeCard(4,temp))
				{ for(i=0;i<=16;i++)temp[i]=0;
					PaymoneyTemp.gold=*f;
					g[0]=PaymoneyTemp.byteM[0];
					g[1]=PaymoneyTemp.byteM[1];
					g[2]=PaymoneyTemp.byteM[2];
					g[3]=PaymoneyTemp.byteM[3];
					for(i=0;i<=4;i++)temp[i]=g[i];	//��Ҫд��Ľ��							
					if(writeCard(5,temp)) flagPay=1;                         							  
				}							
			}
		}
	return flagPay;
}
