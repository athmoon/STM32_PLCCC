#ifndef ___3762_H
#define ___3762_H

#include "includes.h"
#include "flash.h"
#include "def.h"

#define AFN00	0x00
#define AFN01	0x01
#define AFN02	0x02
#define AFN03	0x03
#define AFN04	0x04
#define AFN05	0x05
#define AFN06	0x06
#define AFN10	0x10
#define AFN11	0x11
#define AFN12	0x12
#define AFN13	0x13
#define AFN14	0x14
#define AFN15	0x15

#define F1	0x0100
#define F2	0x0200
#define F3	0x0400
#define F4	0x0800
#define F5	0x1000
#define F6	0x2000
#define F7	0x4000
#define F8	0x8000
#define F9	0x0101
#define F10	0x0201
#define	F11	0x0401
#define F12	0x0801
#define F13	0x1001
#define F14	0x2001
#define F15 0x4001
#define F16 0x8001

extern u8 AFN00_F1[];
extern u8 AFN01_F1[];
extern u8 AFN01_F2[];
extern u8 AFN01_F3[];
extern u8 AFN03_F10[];
extern u8 AFN03_F4[];
extern u8 AFN05_F1[];
extern u8 AFN10_F1[];
extern u8 AFN10_F2[];

typedef enum {
	Null_Action = 0,
	Hardware_Init = 1,
	
} Actions;

typedef struct {
	u8 com_type;//ͨѶ��ʽ
	u8 route_manage;//·�ɹ���ʽ
	u8 message_mode;//�ӽڵ���Ϣģʽ
	u8 cycle_mode;//���ڳ���ģʽ
	u8 delay_param;//������ʱ����֧��
	u8 fail_node_mode;//ʧ�ܽڵ��л�����ʽ
	u8 broadcast_cmdmode;//�㲥����ȷ�Ϸ�ʽ
	u8 broadcast_signalmode;//�㲥�����ŵ�ִ�з�ʽ
	u8 signal_num;//�ŵ�����
	u8 speed_num;//��������
	u8 low_vol_msg;//��ѹ����������Ϣ
	u8 node_time;//�ӽڵ������ʱʱ�䣨��λ��s��
	u8 broadcast_time[2];//�㲥�������ʱʱ�䣨��λ��s��
	u8 max_data_len[2];//���֧�ֵı��ĳ��ȣ�012c
	u8 max_files_len[2];//�ļ�����֧�ֵ���󵥸����ݰ����ȣ�0100
	u8 update_time;//���������ȴ�ʱ��
	u8 main_addr[6];//���ڵ��ַ
	u8 max_node_num[2];//֧�ֵ����ӽڵ�����
	u8 node_num[2];//��ǰ�ӽڵ�����
	u8 protocol_date[3];//ͨ��ģ��ʹ�õ�Э�鷢������(YYMMDD)
	u8 protocol_backup_data[3];//ͨ��ģ��ʹ�õ�Э����󱸰�����(YYMMDD)
	u8 vendor[9];//ͨ��ģ�鳧�̴��뼰�汾��Ϣ
	u8 com_speed[2];//ͨ������00
	u8 com_unit;//���ʵ�λ��ʶ
	//u8 msg_str[41];
}MainNode;
// ���ݳ��� 46 + 1 == u16 * 23 flash

typedef struct {
	u8 direction;
	u8 info[6];
	u8 AFN_code;
	u16 F_code;
	u8 rep_buf[128];
	u8 rep_len;
	Actions action;
} ReadResult;

// ����ӽڵ�ṹ��
typedef struct {
	u8 addr[6]; //�ӽڵ��ַ
	u8 switch_status;//����״̬
	u8 brightness;//����
	u32 power;//����
	u32 voltage;//��ѹ
	u32 current;//����
} SlaveNode;

typedef struct {
	u8 direction;
	u8 info[6];
	u8 AFN_code;
	u16 F_code;
} _3762_CMD;


extern _3762_CMD cmd_struct;
extern MainNode mNode;
extern ReadResult readResult;

u8 read_3762_str(u8 *src_str, u8 src_len);
void main_node_data_init(void);

u8 AFN13_F1_translate_645(u8 *dst, u8 *src, u8 len);
u8 AFN13_F1_recive(u8 *dst, u8 *src, u8 len_3762);

#endif
