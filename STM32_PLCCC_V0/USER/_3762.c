#include "_3762.h"

// 确认帧
u8 AFN00_F1[] = {0x68, 0x15, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x3E, 0x16};
// 否认帧
//u8 AFN00_F2[] = {};
	
// 硬件初始化
u8 AFN01_F1[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x00, 0x45, 0x16};
// 参数区初始化
u8 AFN01_F2[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x00, 0x47, 0x16};
// 数据区初始化
u8 AFN01_F3[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x04, 0x00, 0x4A, 0x16};

// 查询节点运行模式
u8 AFN03_F10[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0x48, 0x16};

// 查询主节点地址
u8 AFN03_F4[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x08, 0x00, 0x4D, 0x16};
// 设置主节点地址
u8 AFN05_F1[] = {0x68, 0x15, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x16};
	
// 查询从节点数量
u8 AFN10_F1[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x10, 0x01, 0x00, 0x58, 0x16};
// 查询从节点信息(从1开始读15个)
u8 AFN10_F2[] = {0x68, 0x12, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x01, 0x00, 0x0F, 0x64, 0x16};
	
// 广播指令包头
u8 AFN05_F3[] = {0x68, 0x1D, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x05, 0x04, 0x00, 0x00};

// 数据转发指令包头
u8 AFN13_F1_head[] = {0x68, 0x2B, 0x00, 0x41, 0x04, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
u8 get_cs(u8 *data, u8 len);
u8 check_cs(u8 *src_str, u8 src_len);
u8 get_AFN_code(u8 *src_str);
u16 get_F_code(u8 *src_str);
void AFN03_F10_data_translate(u8 *data);

MainNode mNode;
MainNode memory_data;
ReadResult readResult;
_3762_CMD cmd_struct;

/// AFN13F1转发指令
u8 AFN13_F1_translate_645(u8 *dst, u8 *src, u8 len) {
	u8 len_3762 = len + 31;
	memcpy(dst, AFN13_F1_head, 16);//包头
	dst[1] = len_3762;//修改长度
	memcpy(&dst[16], &src[1], 6);// 从节点地址
	dst[22] = AFN13;// AFN13
	dst[23] = 0x01;// F1
	dst[24] = 0x00;
	dst[25] = 0x00;// 通讯协议类型
	dst[26] = 0x00;// 通讯验收相关性标志
	dst[27] = 0x00;// 从节点附属节点数量n
	dst[28] = len;// 数据长度
	memcpy(&dst[29], src, len);// 数据
	dst[len_3762 - 2] = get_cs(dst, len_3762 - 2);
	dst[len_3762 - 1] = 0x16;
	
	return len_3762;// 返回3762数据总长度
}

/// AFN13F1接受指令
u8 AFN13_F1_recive(u8 *dst, u8 *src, u8 len_3762) {
	u8 len;
	// 1、计算校验和
	if (!check_cs(src, len_3762)) return 0;
	// 2、校验功能码
	if (!(src[22] == 0x13 && src[23] == 0x01 && src[24] == 0x00)) return 0;
	// 3、获取数据长度
	len = src[28];
	// 4、将数据copy至buffer
	memcpy(dst, &src[29], len);
	// 5、返回数据长度
	return len;
}
	
/// 376.2命令打包函数，将指令打包成字符串数据帧,第一个参数传入3762指令结构体，第二个参数传入缓存区地址，返回数据长度
u8 pack_3762_str(_3762_CMD cmd, u8 *dst) {
	switch(cmd.AFN_code) {
		case AFN00:// 确认否认帧
			switch(cmd.F_code) {
				case F1:// 确认
					memcpy(dst, AFN00_F1, 0x15);
					return 0x15;
				case F2:// 否认
					break;
				default:
					break;
			}
			break;
		case AFN03:
			switch(cmd.F_code) {
				case F10:// 路由运行模式
					memcpy(dst, AFN03_F10, 0x0F);
					break;
			}
			break;
		default:
			break;
	}
	
	return 0;
}

/// 376.2协议解析函数,未出错返回0，出错了返回错误码
u8 read_3762_str(u8 *src_str, u8 src_len) {
//	u8 *data;
//	u8 data_len;
	//判断起始位和末位
	if (src_str[0] == 0x68 && src_str[src_len-1] == 0x16) {
		//数据有效
		if (!check_cs(src_str, src_len)) return 0;//第一步、计算校验码
		readResult.AFN_code = get_AFN_code(src_str);// 读afn功能码
		readResult.F_code = get_F_code(src_str);// 读F码
		readResult.direction = src_str[3];// 读数据方向，是主动还是被动
		memcpy(readResult.info, &src_str[4], 6);// 读信息域，如果是被动，可以用于判断回复的是哪条指令
		switch(readResult.AFN_code) {//第二步、判断功能码
			case AFN00://确认∕否认
				switch(readResult.F_code) {
					case F1://确认
						readResult.action = Null_Action;
						readResult.rep_len = 0;
						//根据信息域的数据编号，来判断具体回复的是哪一条指令
						//判断可以交给主逻辑来做
						break;
					case F2://否认
						readResult.action = Null_Action;
						readResult.rep_len = 0;
						break;
					default:
						break;
				}
				break;
			case AFN01://初始化
				switch(readResult.F_code) {
					case F1://硬件初始化
						break;
					case F2://参数区初始化
						break;
					case F3://数据区初始化
						break;
					default:
						break;
				}
				break;
			case AFN02://数据转发
				switch(readResult.F_code) {
					case F1://转发通信协议数据帧
						break;
					default:
						break;
				}
				break;
			case AFN03://查询数据
				switch(readResult.F_code) {
					case F1://厂商代码和版本信息
						break;
					case F2://噪声值
						break;
					case F3://从节点侦听信息
						break;
					case F4://主节点地址
						break;
					case F5://主节点状态字和通信速率
						break;
					case F6://主节点干扰状态
						break;
					case F7://读取从节点监控最大超时 时间
						break;
					case F8://查询无线通信参数
						break;
					case F9://通信延时相关的广播时长查询
						break;
					case F10://本地通信模块运行模式信息
						//1、对比本地存储的模块运行模式信息是否与主节点上报的相同
						AFN03_F10_data_translate(&src_str[13]);//数据解析
						if (memcmp((u8 *)&mNode, (u8 *)&memory_data, 46) == 0) {//信息完全相同
							readResult.action = Null_Action;
						} else {//新的主节点，或者疾控器首次连接主节点，flash是FFFF
							readResult.action = Hardware_Init;// 进行硬件初始化
							//将新的主节点信息存入flash。
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&mNode,23);
						}
						//2、如果是主节点主动上报的信息，回复确认帧。
						if (readResult.direction == 0xC1) {
							readResult.rep_len = 0x15;
							memcpy(readResult.rep_buf, AFN00_F1, readResult.rep_len);
						}
						break;
					case F11://本地通信模块 AFN 索引
						break;
					default:
						break;
				}
				break;
			case AFN04://链路接口检测
				switch(readResult.F_code) {
					case F1://发送测试
						break;
					case F2://从节点点名
						break;
					case F3://本地通信模块报文通信测试
						break;
					default:
						break;
				}
				break;
			case AFN05://控制命令
				switch(readResult.F_code) {
					case F1://设置主节点地址
						break;
					case F2://允许/禁止从节点上报
						break;
					case F3://启动广播
						break;
					case F4://设置从节点监控最大超时时间
						break;
					case F5://设置无线通信参数
						break;
					default:
						break;
				}
				break;
			case AFN06://主动上报
				switch(readResult.F_code) {
					case F1://上报从节点信息
						break;
					case F2://上报抄读数据
						break;
					case F3://上报路由工况变动信息
						break;
					case F4://上报从节点信息及设备类型
						break;
					case F5://上报从节点事件
						break;
					default:
						break;
				}
				break;
			case AFN10://路由查询
				switch(readResult.F_code) {
					case F1://从节点数量
						break;
					case F2://从节点信息
						break;
					case F3://指定从节点的上一级中继路由信息
						break;
					case F4://路由运行状态
						break;
					case F5://未抄读成功的从节点信息
						break;
					case F6://主动注册的从节点信息
						break;
					default:
						break;
				}
				break;
			case AFN11://路由设置
				switch(readResult.F_code) {
					case F1://添加从节点
						break;
					case F2://删除从节点
						break;
					case F3://设置从节点固定中继路径
						break;
					case F4://设置工作模式
						break;
					case F5://激活从节点主动注册
						break;
					case F6://终止从节点主动注册
						break;
					default:
						break;
				}
				break;
			case AFN12://路由控制
				switch(readResult.F_code) {
					case F1://重启
						break;
					case F2://暂停
						break;
					case F3://恢复
						break;
					default:
						break;
				}
				break;
			case AFN13://路由数据转发
				switch(readResult.F_code) {
					case F1://监控从节点
						break;
					default:
						break;
				}
				break;
			case AFN14://路由数据抄读
				switch(readResult.F_code) {
					case F1://路由请求抄读内容
						break;
					case F2://路由请求集中器时钟
						break;
					case F3://请求依通信延时修正通信数据
						break;
					default:
						break;
				}
				break;
			case AFN15://文件传输
				switch(readResult.F_code) {
					case F1://文件传输方式 1
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

/// 计算校验和
u8 get_cs(u8 *data, u8 len) {
	u8 i = 0;
	u8 sum = 0;
	for (;i < len; i++) {
		sum = sum + data[i];
	}
	sum = sum & 0xFF;
	return sum;
}

u8 check_cs(u8 *src_str, u8 src_len) {
	u8 cs = get_cs(&src_str[3], src_len - 5);
	if (cs == src_str[src_len - 2]) {
		return 1;
	} else {
		return 0;
	}
}

u8 get_AFN_code(u8 *src_str) {
	return src_str[10];
}

u16 get_F_code(u8 *src_str) {
	return (src_str[11] << 8) + src_str[12];
}

void main_node_data_init(void) {
	//从flash读取载波主节点的本地数据进行初始化
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)memory_data,23);
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)&memory_data,23);
}

void AFN03_F10_data_translate(u8 *data) {
	mNode.com_type = data[0] & 0x0F;
	mNode.route_manage = (data[0] >> 4) & 0x01;
	mNode.message_mode = (data[0] >> 5) & 0x01;
	mNode.cycle_mode = data[0] >> 6;
	mNode.delay_param = data[1] & 0x07;
	mNode.fail_node_mode = (data[1] >> 3) & 0x03;
	mNode.broadcast_cmdmode = (data[1] >> 5) & 0x01;
	mNode.broadcast_signalmode = data[1] >> 6;
	mNode.signal_num = data[2] & 0x0F;
	mNode.speed_num = data[2] >> 4;
	mNode.low_vol_msg = data[3] & 0x07;
	mNode.node_time = data[6];
	memcpy(mNode.broadcast_time, &data[7], 2);
	memcpy(mNode.max_data_len, &data[9], 2);
	memcpy(mNode.max_files_len, &data[11], 2);
	mNode.update_time = data[13];
	memcpy(mNode.main_addr, &data[14], 6);
	memcpy(mNode.max_node_num, &data[20], 2);
	memcpy(mNode.node_num, &data[22], 2);
	memcpy(mNode.protocol_date, &data[24], 3);
	memcpy(mNode.protocol_backup_data, &data[27], 3);
	memcpy(mNode.vendor, &data[30], 9);
	memcpy(mNode.com_speed, &data[39], 2);
}
