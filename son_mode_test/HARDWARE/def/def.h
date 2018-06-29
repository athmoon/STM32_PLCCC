#ifndef __DEF_H
#define	__DEF_H



/* the macro definition to trigger the led on or off 
 * 1 - off
 - 0 - on
 */
#define ON  0
#define OFF 1	

////////////�ź����ȴ�ʱ��///////////////
#define DEV_TIMEOUT_10	10//�������ѯ�豸�ĳ�ʱ
#define DEV_TIMEOUT			100//�������ѯ�豸�ĳ�ʱ
///////////////////////////////////////////////////////////


///////////dma���շ���/////////////////////////////
#define DEV_REC_LEN			128	//������ջ�������С
#define DEV_SEND_LEN		32	//���巢�ͻ�������С
//////////////////////////////////////////////////////


/////////////////////���ָʾ��////////////////
#define LED1(a)	if (a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_7);\
					else\
					GPIO_ResetBits(GPIOC,GPIO_Pin_7);

#define LED2(a)	if (a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_8);

#define LED3(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_9);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_9);

#define LED4(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_8);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_8);	
/////////////////////////////////////////////////////////////		

					
//////////////��ָͣʾ��//////////////////////
#define LED5(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_10);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_10);	
					
////////////��ͣ��������//////////////////
#define IN_PUT PBin(4)			
////////////////////////////////////////////////////////////////	

					
//////////////��翪��/////////////////
#define RELAY1(a)	if (a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_12);\
					else\
					GPIO_ResetBits(GPIOC,GPIO_Pin_12);
#define RELAY2(a)	if (a)\
					GPIO_SetBits(GPIOD,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOD,GPIO_Pin_2);
/////////////////////////////////////////////////////////////////		


					
///////////////////flash����////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
///////////////////////////////////////////////////
//���д��flash��ַ
#define FLASH_SAVE_ADDR  0X08010000 	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
					
					
/////////��ǹ�˿ڶ���///////////////////					
#define Connect_gun PAin(5) 
#define Ready_gun   PAin(4)
#define Ready_EV		PCout(11)
/////////////////////////////////////////////////////////////////
					
					
					
//////����3���//////////////////////					
//rs485  �շ�ʹ��
#define READ  0
#define WRITE 1	//����485����
#define DIR3(a)	if (a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_1);\
					else\
					GPIO_ResetBits(GPIOB,GPIO_Pin_1);
///////////////////////////////////////////
//���궨��modbus������
#define READ_COIL_STATUS		0x01
#define READ_INPUT_STATUS 		0x02
#define READ_HOLDING_REGISTERS 	0x03
#define READ_INPUT_REGISTERS 	0x04
#define FORCE_SINGLE_COIL 		0x05
#define PRESET_SINGLE_REGISTER 	0x06
#define PRESET_MULTIPLE_REGISTERS 0x10
#define CONFIG_LOGIC			0x41
#define ILLEGAL_DATA_ADDRESS 0x01
#define ILLEGAL_DATA_VALUE 0x04
#define DEV_START_ADDR	1		//�������ѯ�豸����ʼ��ַ
#define DEV_REG_START		0		//�������ѯ�豸�ļĴ�����ʼ��ַ
#define DEV_REG_NUM			9		//�������ѯ�豸�ļĴ�������
///////////////////////////////////////////////////////////////////////



/////////////����2������/////////////////////////////////////
#define HEAD1			0x5A
#define HEAD2			0xA5
#define READREG 	0x81
#define WRITEREG	0x80
#define READDATE	0x83
#define WRITEDATA	0x82
#define PAGEREGISTER 0X03
////////////////////////////////////////////////////////////






/////////////////ϵͳ����״̬//////////////////////////////
#define Status_Idle 0x00//����״̬
#define Status_PrepareCharge  0x01//Ԥ���״̬
#define Status_Charge 0x02//���״̬
#define Status_PreparePay 0x03//Ԥ����״̬
#define Status_Pay 0x04//����״̬
#define Status_Query 0x05//��ѯ״̬
#define Status_Unusual 0x06//�쳣״̬
#define Status_Manage 0x07//����״̬
///////////////////////////////////////////////////////////////

/////////////////////////////////���Ϸ���//////////////////////////////
#define Urgent_Stop                1//���ϣ�����ͣ��
#define Over_I                     2//���ϣ���������

#define CardEmpty_Status_Prepare   3//���ϣ��˿�Ϊ�տ�
#define CardLock_Status_Prepare    4//���ϣ��˿�����
#define CardMoney_Status_Prepare   5//���ϣ�����
#define CardAnomaly_Status_Prepare 6//���ϣ��˿��쳣
#define CardDelay_Status_Prepare   7//���ϣ�ˢ����ʱ

#define CardMoney_Status_Charge    8//���ϣ�����

#define CardError_Status_Pay       9//���ϣ����Ų���
#define CardNouse_Status_Pay       10//���ϣ��˿�δ���
#define CardEmpty_Status_Pay       11//���ϣ��˿�Ϊ�տ�
#define CardAnomaly_Status_Pay     12//���ϣ��˿��쳣
#define CardDelay_Status_Pay       13//���ϣ�ˢ����ʱ


#define CardDelay_Status_Query     14//���ϣ�ˢ����ʱ

#define NO_GUN       							 15//δ����ǹͷ
#define NO_READY   								 16//����δ����
#define CHARGE_OVER   						 17//������
#define PW_err      							 18//�������
///////////////////////////////////////////////////////////////


////////////////////////��ʼ�����/////////////////////////////
#define PRICE1  101
#define PRICE2  102
#define PRICE3  103
#define PRICE4  104
#define PRICE5  105
#define PRICE6  106
#define PRICE7  107
#define PRICE8  108
#define PRICE9  109
#define PRICE10 110
#define PRICE11 120
#define PRICE12 130
/////////////////////////////////////////////////////////////////



//////////////////////////����״̬����12345678////////////////////////
#define pward1 0x00
#define pward2 0xBC
#define pward3 0x61 
#define pward4 0x4E 
////////////////////////////////////////////////////////////////////////////


////////////////////�������м�ֵ/////////////////////////////
#define Limit_use_Money 5//���������������
#define Limit_surplus_Money 1//�����������������
#define max_I 32000//��������������ֵ
///////////////////////////////////////////////////////////////////



////////////////////////������//////////////////////////////////////////
#define Time_to_Ad      60  //���״̬������ʱ��
#define Time_switch_Ad  5  //�л������
#define ad_num  				6  //�������
////////////////////////////////////////////////////////////////////


#endif
