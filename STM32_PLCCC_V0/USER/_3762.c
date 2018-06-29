#include "_3762.h"

u8 AFN00_F1[] = {0x68, 0x15, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x3E, 0x16};
u8 AFN03_F10[] = {0x68, 0x0F, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x02, 0x01, 0x48, 0x16};
	//u8 AFN00_F2[] = {};
//u8 AFN13_F1[] = {};//ָ��ת����ͷ
	
u8 get_cs(u8 *data, u8 len);
u8 check_cs(u8 *src_str, u8 src_len);
u8 get_AFN_code(u8 *src_str);
u16 get_F_code(u8 *src_str);
void AFN03_F10_data_translate(u8 *data);

MainNode mNode;
MainNode memory_data;
ReadResult readResult;
_3762_CMD cmd_struct;
	
/// 376.2��������������ָ�������ַ�������֡,��һ����������3762ָ��ṹ�壬�ڶ����������뻺������ַ���������ݳ���
u8 pack_3762_str(_3762_CMD cmd, u8 *dst) {
	switch(cmd.AFN_code) {
		case AFN00:// ȷ�Ϸ���֡
			switch(cmd.F_code) {
				case F1:// ȷ��
					memcpy(dst, AFN00_F1, 0x15);
					return 0x15;
				case F2:// ����
					break;
				default:
					break;
			}
			break;
		case AFN03:
			switch(cmd.F_code) {
				case F10:// ·������ģʽ
					memcpy(dst, AFN03_F10, 0x0F);
					break;
			}
			break;
		default:
			break;
	}
}

/// 376.2Э���������,δ������0�������˷��ش�����
u8 read_3762_str(u8 *src_str, u8 src_len) {
//	u8 *data;
//	u8 data_len;
	//�ж���ʼλ��ĩλ
	if (src_str[0] == 0x68 && src_str[src_len-1] == 0x16) {
		//������Ч
		if (!check_cs(src_str, src_len)) return 0;//��һ��������У����
		readResult.AFN_code = get_AFN_code(src_str);// ��afn������
		readResult.F_code = get_F_code(src_str);// ��F��
		readResult.direction = src_str[3];// �����ݷ������������Ǳ���
		memcpy(readResult.info, &src_str[4], 6);// ����Ϣ������Ǳ��������������жϻظ���������ָ��
		switch(readResult.AFN_code) {//�ڶ������жϹ�����
			case AFN00://ȷ�ϨM����
				switch(readResult.F_code) {
					case F1://ȷ��
						readResult.action = Null_Action;
						readResult.rep_len = 0;
						//������Ϣ������ݱ�ţ����жϾ���ظ�������һ��ָ��
						//�жϿ��Խ������߼�����
						break;
					case F2://����
						readResult.action = Null_Action;
						readResult.rep_len = 0;
						break;
					default:
						break;
				}
				break;
			case AFN01://��ʼ��
				switch(readResult.F_code) {
					case F1://Ӳ����ʼ��
						break;
					case F2://��������ʼ��
						break;
					case F3://��������ʼ��
						break;
					default:
						break;
				}
				break;
			case AFN02://����ת��
				switch(readResult.F_code) {
					case F1://ת��ͨ��Э������֡
						break;
					default:
						break;
				}
				break;
			case AFN03://��ѯ����
				switch(readResult.F_code) {
					case F1://���̴���Ͱ汾��Ϣ
						break;
					case F2://����ֵ
						break;
					case F3://�ӽڵ�������Ϣ
						break;
					case F4://���ڵ��ַ
						break;
					case F5://���ڵ�״̬�ֺ�ͨ������
						break;
					case F6://���ڵ����״̬
						break;
					case F7://��ȡ�ӽڵ������ʱ ʱ��
						break;
					case F8://��ѯ����ͨ�Ų���
						break;
					case F9://ͨ����ʱ��صĹ㲥ʱ����ѯ
						break;
					case F10://����ͨ��ģ������ģʽ��Ϣ
						//1���Աȱ��ش洢��ģ������ģʽ��Ϣ�Ƿ������ڵ��ϱ�����ͬ
						AFN03_F10_data_translate(&src_str[13]);//���ݽ���
						if (memcmp((u8 *)&mNode, (u8 *)&memory_data, 46)) {//��Ϣ��ȫ��ͬ
							readResult.action = Null_Action;
						} else {//�µ����ڵ�
							readResult.action = Hardware_Init;
						}
						//2����������ڵ������ϱ�����Ϣ���ظ�ȷ��֡��
						if (readResult.direction == 0xC1) {
							readResult.rep_len = 0x15;
							memcpy(readResult.rep_buf, AFN00_F1, readResult.rep_len);
						}
						break;
					case F11://����ͨ��ģ�� AFN ����
						break;
					default:
						break;
				}
				break;
			case AFN04://��·�ӿڼ��
				switch(readResult.F_code) {
					case F1://���Ͳ���
						break;
					case F2://�ӽڵ����
						break;
					case F3://����ͨ��ģ�鱨��ͨ�Ų���
						break;
					default:
						break;
				}
				break;
			case AFN05://��������
				switch(readResult.F_code) {
					case F1://�������ڵ��ַ
						break;
					case F2://����/��ֹ�ӽڵ��ϱ�
						break;
					case F3://�����㲥
						break;
					case F4://���ôӽڵ������ʱʱ��
						break;
					case F5://��������ͨ�Ų���
						break;
					default:
						break;
				}
				break;
			case AFN06://�����ϱ�
				switch(readResult.F_code) {
					case F1://�ϱ��ӽڵ���Ϣ
						break;
					case F2://�ϱ���������
						break;
					case F3://�ϱ�·�ɹ����䶯��Ϣ
						break;
					case F4://�ϱ��ӽڵ���Ϣ���豸����
						break;
					case F5://�ϱ��ӽڵ��¼�
						break;
					default:
						break;
				}
				break;
			case AFN10://·�ɲ�ѯ
				switch(readResult.F_code) {
					case F1://�ӽڵ�����
						break;
					case F2://�ӽڵ���Ϣ
						break;
					case F3://ָ���ӽڵ����һ���м�·����Ϣ
						break;
					case F4://·������״̬
						break;
					case F5://δ�����ɹ��Ĵӽڵ���Ϣ
						break;
					case F6://����ע��Ĵӽڵ���Ϣ
						break;
					default:
						break;
				}
				break;
			case AFN11://·������
				switch(readResult.F_code) {
					case F1://��Ӵӽڵ�
						break;
					case F2://ɾ���ӽڵ�
						break;
					case F3://���ôӽڵ�̶��м�·��
						break;
					case F4://���ù���ģʽ
						break;
					case F5://����ӽڵ�����ע��
						break;
					case F6://��ֹ�ӽڵ�����ע��
						break;
					default:
						break;
				}
				break;
			case AFN12://·�ɿ���
				switch(readResult.F_code) {
					case F1://����
						break;
					case F2://��ͣ
						break;
					case F3://�ָ�
						break;
					default:
						break;
				}
				break;
			case AFN13://·������ת��
				switch(readResult.F_code) {
					case F1://��شӽڵ�
						break;
					default:
						break;
				}
				break;
			case AFN14://·�����ݳ���
				switch(readResult.F_code) {
					case F1://·�����󳭶�����
						break;
					case F2://·����������ʱ��
						break;
					case F3://������ͨ����ʱ����ͨ������
						break;
					default:
						break;
				}
				break;
			case AFN15://�ļ�����
				switch(readResult.F_code) {
					case F1://�ļ����䷽ʽ 1
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

/// ����У���
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
	//��flash��ȡ�ز����ڵ�ı������ݽ��г�ʼ��
	//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)ElePrice,12);
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
