#include "server_cmd.h"
#include "_3762.h"

ServerCmd com2_cmd;

u8 SERVER_CMD_buf[SERVER_CMD_BUFLEN];
u8 _13f1_msg[12] = {0x04, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 _13f1_type[6] = {0x13, 0x01, 0x00, 0x00, 0x00, 0x00};

void ServerCmd_init(ServerCmd *sc) {
	sc->databuf = SERVER_CMD_buf;
	sc->sem_server_cmd = OSSemCreate(0);
}

u8 check_data_header(u8 *src, u8 buf_len) {
	if (buf_len < 7) return 0;// ָ�������Ϊ7
	if (src[0] != 0xFB) return 0;
	if (src[1] != 0xFB) return 0;
	return 1;
}
u8 check_data_tail(u8 *src, u8 buf_len) {
	if (buf_len < 7) return 0;// ָ�������Ϊ7
	if (src[buf_len - 2] != 0xFE) return 0;
	if (src[buf_len - 1] != 0xFE) return 0;
	return 1;
}

u8 check_data_sum(u8 *src, u8 buf_len){
	u8 sum = get_cs(src, buf_len - 3);
	if (sum == src[buf_len - 3]) return 1;
	return 0;
}

void translate_to_serverCmd(ServerCmd *sc, u8 *data_src, u8 buf_len) {
	sc->cmd = data_src[2];
	sc->datalen = data_src[3];
	memcpy(sc->databuf, &data_src[4], sc->datalen);
}

void data_handle(ServerCmd *sc) {
	switch (sc->cmd) {
		case SERVER_CONTROL:// 376.2ָ��
			// ֱ��ת��
			OSSemPost(sc->sem_server_cmd);
			break;
		case SERVER_COMMUNICAT:// 645�Զ���ָ��
			// ��645ָ���װһ��376.2�е�13F1����ת��ָ��
			package645_to_3762_afn13f1(sc->databuf, &sc->datalen);
			// ת��
			OSSemPost(sc->sem_server_cmd);
			break;
		case SERVER_HEARTBEAT:// ������ָ��
			// �ظ�������(���ô���2�ķ��Ͷ���Ϣ����)
			break;
		case SERVER_READMAC:// ��mac��ַ�����豸Ψһ���룩
			// �ظ�mac��ַ���豸Ψһ����
			break;
		default:// δָ֪��
			break;
	}
}

void package645_to_3762_afn13f1(u8 *src, u8 *len) {
	u8 addr[6];
	u8 datalen;
	u8 temp[SERVER_CMD_BUFLEN];
	//1������ַ
	memcpy(addr, &src[1], 6);
	//2�����㳤��
	datalen = 0x1F + *len;
	//3��ƴ��ָ��
	temp[0] = 0x68;
	temp[1] = datalen;
	temp[2] = 0x00;
	temp[3] = 0x41;
	memcpy(&temp[4], _13f1_msg, 12);
	memcpy(&temp[16], addr, 6);
	memcpy(&temp[22], _13f1_type, 6);
	temp[28] = *len;
	memcpy(&temp[29], src, *len);
	//4������У��
	temp[datalen - 2] = get_cs(temp, datalen - 2);
	temp[datalen - 1] = 0x16;
	//5������buffer
	memcpy(src, temp, datalen);
	*len = datalen;
}

u8 package_to_servercmd(u8 *dst, u8 *src, u8 len3762) {
	u8 len = 0;
	u8 temp[SERVER_CMD_BUFLEN];
	// ��һ�����ж�ָ����������ϱ����Ǳ����ظ�
	if (src[3] == 0x81) {// �����ظ�
		// ����Ǳ����ظ�����ָ��ֻ�����ǿ���ָ���ͨѶָ��
		// �ڶ������ж�AFN��F�룬�����13f1��ΪͨѶָ��ظ������������ָ����Ϊ����ָ��ظ�
		len = AFN13_F1_recive(temp, src, len3762);
		if (len != 0) {// 13F1ָ�ͨѶָ��
			len = package645_to_servercmd(dst, temp, len);
		} else {// ����ָ�����ָ��
			len = package3762_to_servercmd(dst, src, len3762, SERVER_CONTROL);
		}
	} else if (src[3] == 0xC1) {// �����ϱ�
		// ����������ϱ�����ָ��Ϊ���ڵ������ϱ�
		len = package3762_to_servercmd(dst, src, len3762, SERVER_AUTOUPDATE);
	}
	
	return len;
}

u8 package3762_to_servercmd(u8 *dst, u8 *src, u8 len3762, u8 cmdtype) {
	u8 len;
	len = len3762 + 7;
	dst[0] = 0xFB;
	dst[1] = 0xFB;
	dst[2] = cmdtype;
	dst[3] = len3762;
	memcpy(&dst[4], src, len3762);
	// ����У���
	dst[len - 3] = get_cs(dst, len - 3);
	dst[len - 2] = 0xFE;
	dst[len - 1] = 0xFE;
	
	return len;
}

u8 package645_to_servercmd(u8 *dst, u8 *src, u8 len645) {
	u8 len;
	len = len645 + 7;
	dst[0] = 0xFB;
	dst[1] = 0xFB;
	dst[2] = SERVER_COMMUNICAT;
	dst[3] = len645;
	memcpy(&dst[4], src, len645);
	// ����У���
	dst[len - 3] = get_cs(dst, len - 3);
	dst[len - 2] = 0xFE;
	dst[len - 1] = 0xFE;
	
	return len;
}

//u8 get_cs(u8 *src, u8 len) {
//	u8 i = 0;
//	u8 sum = 0;
//	for (; i < len; i++) {
//		sum += src[i];
//	}
//	return sum;
//}
