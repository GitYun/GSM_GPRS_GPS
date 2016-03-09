#ifndef __TCPSEVER_H
#define __TCPSEVER_H

#include "stm32f10x.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

#define RPS_OK	"0"
#define GSM_RESPONSE_TIME	500
#define GSM_SEND_CMD_MAX	3
#define GSM_CHECK_RSP_MAX	3

typedef enum _SEND_TO_GSM_MSG_FORMAT {
	STRING = 0,
	NUMBER
} SNED_TO_GSM_MSG_FORMAT;

typedef enum _CMD_RESPONSE {
	LOOP_CMD_RESPONSE_TIMEOUT = 0,
	Query_CMD_HAS_BEEN_SETUP = 0,
	LOOP_CMD_RESPONSE_OK = 1
} CMD_RESPONSE;


typedef struct _TCP_CONNECT_STATUS{
	volatile uint8_t tcpConnectFlag;
} TCP_CONNECT_STATUS;

uint8_t TCPClient_Config(void);
uint8_t TCPClient_Reconnect(void);

CMD_RESPONSE QueryBeforeSet(const char*, const char*, const char*, const char*, ...);
CMD_RESPONSE LoopSetUntilResponsed(const char* setCmd, const char* setReponseStr, ...);

uint8_t ATCmdHand(const char *str);
uint8_t SendMsgToTCPServer(const char* msg);
uint8_t TCPClient_Close(void);

void SendCmdToGSM(const char* cmd, uint16_t waitResponseTime);
//void SendCmdToGSM(SNED_TO_GSM_MSG_FORMAT format, void* msg, uint16_t waitResponseTime);
uint8_t IsTCPServerQuery(const char* QueryStr);

#endif
