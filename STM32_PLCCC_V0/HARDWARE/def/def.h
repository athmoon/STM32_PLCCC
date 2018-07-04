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


///////////////////flash����////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
///////////////////////////////////////////////////
//���ڵ�����ģʽд��flash��ַ
#define FLASH_SAVE_ADDR  0X08010000 	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
					
					
//////����3 485�ӿ�//////////////////////					
//rs485  �շ�ʹ��
#define READ  0
#define WRITE 1	//����485����
#define DIR3(a)	if (a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_1);\
					else\
					GPIO_ResetBits(GPIOB,GPIO_Pin_1);
///////////////////////////////////////////
//�궨��modbus������
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



#endif
