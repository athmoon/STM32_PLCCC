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

					
//////////////急停指示灯//////////////////////
#define LED5(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_10);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_10);	
					
////////////急停接收引脚//////////////////
#define IN_PUT PBin(4)			
////////////////////////////////////////////////////////////////	

					
//////////////充电开关/////////////////
#define RELAY1(a)	if (a)\
					GPIO_SetBits(GPIOC,GPIO_Pin_12);\
					else\
					GPIO_ResetBits(GPIOC,GPIO_Pin_12);
#define RELAY2(a)	if (a)\
					GPIO_SetBits(GPIOD,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOD,GPIO_Pin_2);
/////////////////////////////////////////////////////////////////		


					
///////////////////flash定义////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
///////////////////////////////////////////////////
//电价写入flash地址
#define FLASH_SAVE_ADDR  0X08010000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
					
					
/////////检枪端口定义///////////////////					
#define Connect_gun PAin(5) 
#define Ready_gun   PAin(4)
#define Ready_EV		PCout(11)
/////////////////////////////////////////////////////////////////
					
					
					
//////串口3电表//////////////////////					
//rs485  收发使能
#define READ  0
#define WRITE 1	//控制485方向
#define DIR3(a)	if (a)\
					GPIO_SetBits(GPIOB,GPIO_Pin_1);\
					else\
					GPIO_ResetBits(GPIOB,GPIO_Pin_1);
///////////////////////////////////////////
//电表宏定义modbus功能码
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



/////////////串口2读卡器/////////////////////////////////////
#define HEAD1			0x5A
#define HEAD2			0xA5
#define READREG 	0x81
#define WRITEREG	0x80
#define READDATE	0x83
#define WRITEDATA	0x82
#define PAGEREGISTER 0X03
////////////////////////////////////////////////////////////






/////////////////系统工作状态//////////////////////////////
#define Status_Idle 0x00//空闲状态
#define Status_PrepareCharge  0x01//预充电状态
#define Status_Charge 0x02//充电状态
#define Status_PreparePay 0x03//预结算状态
#define Status_Pay 0x04//结算状态
#define Status_Query 0x05//查询状态
#define Status_Unusual 0x06//异常状态
#define Status_Manage 0x07//管理状态
///////////////////////////////////////////////////////////////

/////////////////////////////////故障分类//////////////////////////////
#define Urgent_Stop                1//故障，紧急停车
#define Over_I                     2//故障，电流过大

#define CardEmpty_Status_Prepare   3//故障，此卡为空卡
#define CardLock_Status_Prepare    4//故障，此卡已锁
#define CardMoney_Status_Prepare   5//故障，余额不足
#define CardAnomaly_Status_Prepare 6//故障，此卡异常
#define CardDelay_Status_Prepare   7//故障，刷卡超时

#define CardMoney_Status_Charge    8//故障，余额不足

#define CardError_Status_Pay       9//故障，卡号不对
#define CardNouse_Status_Pay       10//故障，此卡未充电
#define CardEmpty_Status_Pay       11//故障，此卡为空卡
#define CardAnomaly_Status_Pay     12//故障，此卡异常
#define CardDelay_Status_Pay       13//故障，刷卡超时


#define CardDelay_Status_Query     14//故障，刷卡超时

#define NO_GUN       							 15//未连接枪头
#define NO_READY   								 16//车辆未就绪
#define CHARGE_OVER   						 17//充电完毕
#define PW_err      							 18//密码错误
///////////////////////////////////////////////////////////////


////////////////////////初始化电价/////////////////////////////
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



//////////////////////////管理状态密码12345678////////////////////////
#define pward1 0x00
#define pward2 0xBC
#define pward3 0x61 
#define pward4 0x4E 
////////////////////////////////////////////////////////////////////////////


////////////////////充电过程中极值/////////////////////////////
#define Limit_use_Money 5//定义最低限制消费
#define Limit_surplus_Money 1//定义充电过程中最低余额
#define max_I 32000//充电过程中最大电流值
///////////////////////////////////////////////////////////////////



////////////////////////广告参数//////////////////////////////////////////
#define Time_to_Ad      60  //充电状态进入广告时间
#define Time_switch_Ad  5  //切换广告间隔
#define ad_num  				6  //广告数量
////////////////////////////////////////////////////////////////////


#endif
