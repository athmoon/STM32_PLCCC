#ifndef __SERVER_CMD_H
#define __SERVER_CMD_H

#include "includes.h"

#define SERVER_CMD_BUFLEN 255

typedef struct {
	u8 cmd;
	u8 datalen;
	u8 *databuf;
	OS_EVENT * sem_server_cmd;
}ServerCmd;

void ServerCmd_init(ServerCmd *sc);

#define SERVER_CONTROL 				0x01
#define SERVER_COMMUNICAT 		0x02
#define SERVER_HEARTBEAT 			0x03
#define SERVER_READMAC 				0x04

extern ServerCmd com2_cmd;

u8 check_data_header(u8 *src, u8 buf_len);
u8 check_data_tail(u8 *src, u8 buf_len);
u8 check_data_sum(u8 *src, u8 buf_len);
void translate_to_serverCmd(ServerCmd *sc, u8 *data_src, u8 buf_len);
void data_handle(ServerCmd *sc);
void package645_to_3762_afn13f1(u8 *src, u8 len);

#endif

