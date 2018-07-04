#ifndef __DEF_H
#define	__DEF_H



/* the macro definition to trigger the led on or off 
 * 1 - off
 - 0 - on
 */
#define ON  0
#define OFF 1	

////////////信号量等待时间///////////////
#define DEV_TIMEOUT_10	10//定义待查询设备的超时
#define DEV_TIMEOUT			100//定义待查询设备的超时
///////////////////////////////////////////////////////////


///////////dma接收发送/////////////////////////////
#define DEV_REC_LEN			128	//定义接收缓冲区大小
#define DEV_SEND_LEN		32	//定义发送缓冲区大小
//////////////////////////////////////////////////////


/////////////////////充电指示灯////////////////
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


///////////////////flash定义////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
///////////////////////////////////////////////////
//主节点运行模式写入flash地址
#define FLASH_SAVE_ADDR  0X08010000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
					
					
//////串口3 485接口//////////////////////					
//rs485  收发使能
#define READ  0
#define WRITE 1	//控制485方向
#define DIR3(a)	if (a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_1);\
					else\
					GPIO_ResetBits(GPIOB,GPIO_Pin_1);
///////////////////////////////////////////
//宏定义modbus功能码
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
#define DEV_START_ADDR	1		//定义待查询设备的起始地址
#define DEV_REG_START		0		//定义待查询设备的寄存器起始地址
#define DEV_REG_NUM			9		//定义待查询设备的寄存器数量
///////////////////////////////////////////////////////////////////////



#endif
