#include "TCPSever.h"
#include "gsm.h"
#include "delay.h"
#include "usart.h"

#define xRSP_SEND_OK			// ���ͳɹ�ʱ����>��SEND OK
#define ATE0_V0_ENABLE		// �ر�GSM���ԣ�������뷵��Ϊ���֣�0����OK��4����ERROR��

// Configure Sim900a(GSM module) being a TCP client
uint8_t TCPClient_Config(void)
{
#if defined ( ATE0_V0_ENABLE )
	// �رջ��ԣ���Ӧ���������������ʾ�����浱ǰ�������û����ñ�0�������������ʱ��
	LoopSetUntilResponsed( GSM_CMD(SET_CMD, ATE0_V0_W0_CLTS, "1"), "0", NULL );
#else
	LoopSetUntilResponsed( GSM_CMD(SET_CMD, ATE1_V1_W0_CLTS, "1"), "OK", NULL );
#endif
	
	LoopSetUntilResponsed( GSM_CMD(SET_CMD, CIPSPRT, "2"), RPS_OK, NULL );
	
//	QueryBeforeSet(AT_CGATT_Query, "+CGATT: 1", AT_CGATT_ON, RPS_OK, NULL);
//	QueryBeforeSet(AT_CIPCSGP_Query, "+CIPCSGP: 1,\"CMNET\"", AT_CIPCSGP, RPS_OK, NULL);

	LoopSetUntilResponsed(GSM_CMD(SET_CMD, CLPORT, CLPORT_PARAM), RPS_OK, NULL);

	return (uint8_t)LoopSetUntilResponsed( GSM_CMD(SET_CMD, CIPSTART, CIPSTART_PARAM), "CONNECT OK", "ALREADY CONNECT");
}

uint8_t TCPClient_Reconnect(void)
{
	return (uint8_t)LoopSetUntilResponsed( GSM_CMD(SET_CMD, CIPSTART, CIPSTART_PARAM), 
																						 "CONNECT OK", "ALREADY CONNECT");
}

// ��TCP Serverѭ���������� setCmd��ֱ����Ӧ��ʱ
CMD_RESPONSE LoopSetUntilResponsed(const char* setCmd, const char* setReponseStr, ...)
{
	CMD_RESPONSE retval;
	volatile char* vaStr;
	va_list arg_ptr;
	// ��������õ���Ӧ�����ʱ�䣺timeOut*(checkCount*GSM_RESPONSE_TIME) (ms)
	uint8_t timeOut = GSM_SEND_CMD_MAX;
	// �ȴ���Ӧʱ�䣺checkCount * GSM_RESPONSE_TIME (ms)
	uint8_t checkCount;
	
	va_start(arg_ptr, setReponseStr);
	vaStr = va_arg(arg_ptr, char*); // ֻʹ��һ����չ����

	do
	{
		checkCount = GSM_CHECK_RSP_MAX;		// ���� AT+CIPSTART ����õ���ȷ��Ӧ�ļ���������Ϊ 3
		SendCmdToGSM( setCmd, GSM_RESPONSE_TIME );		
		while ( !ATCmdHand(setReponseStr) && !ATCmdHand((const char*)vaStr) )
		{
			checkCount--;
			if ( checkCount == 0)
				break;
			delayms(GSM_RESPONSE_TIME);	// �����ȴ� TCP ������Ӧ
		}
		Clear_USART1Buf();
		if ( checkCount == 0 )
			timeOut--;
		else				
			break;
	} while ( timeOut );
	
	if( timeOut == 0 )
		retval = LOOP_CMD_RESPONSE_TIMEOUT;	// 0
	else
		retval = LOOP_CMD_RESPONSE_OK;	// 1
	
	va_end(arg_ptr);
	return retval;
}

// ����֮ǰ�Ȳ�ѯ
// ֻʹ��һ����չ����
CMD_RESPONSE QueryBeforeSet(const char* queryCmd, const char* queryResponse, 
		const char* setCmd, const char* setResponse, ...)
{
	CMD_RESPONSE retval;	
	volatile char* vaStr;
	va_list arg_ptr;
	va_start(arg_ptr, setResponse);
	vaStr = va_arg(arg_ptr, char*);
	
	SendCmdToGSM(queryCmd, GSM_RESPONSE_TIME);
	if ( !ATCmdHand(queryResponse) )	// �Ƿ������ã�
	{
		retval = LoopSetUntilResponsed(setCmd, setResponse, vaStr);
	}
	else
	{
		Clear_USART1Buf();
		retval = Query_CMD_HAS_BEEN_SETUP;	// 0
	}
	va_end(arg_ptr);
	return retval;
}

uint8_t SendMsgToTCPServer(const char* msg)
{
	uint8_t result = 0;
	
#if defined ( RSP_SEND_OK )  // ���ͳɹ���ʾ > �� SEND OK
	int len = strlen(msg)+2+1;
	char sendMsg[len];
	
	strcpy(sendMsg, msg);
	strcat(sendMsg, "\x1A");	
	if( LoopSetUntilResponsed(AT_CIPSEND, ">", NULL) )
		result = (uint8_t)LoopSetUntilResponsed(sendMsg, "SEND OK", NULL);
#else
	// ���ͳɹ�����ʾ>��SEND OK
	SendCmdToGSM(AT_CIPSEND, 10);
	SendCmdToGSM(msg, 0);
	SendCmdToGSM("\x1A", 0);
#endif
	
	return result;
}

uint8_t TCPClient_Close(void)
{
	if ( LoopSetUntilResponsed( GSM_CMD(SET_CMD, CIPCLOSE, "1"), "CLOSE OK", NULL ) )
		return 1;
	else
		return 0;
}

/**
  * @brief  ����GSM������Ϣ���Ƿ��и������ַ���
  * @param  str: Ҫ���ҵ��ַ���
  * @retval 0 - ����ʧ�ܣ�1 - ���ҳɹ�
  */
uint8_t ATCmdHand(const char *str)
{
	if ( str == NULL )
		return 0;
  if(strstr(USART1_RxBuff, str) != NULL)
	  return 1;
	else
		return 0;
}


// STM32 ͨ�� USART1 ��������/���ݸ�GSM(Sim900a)
void SendCmdToGSM(const char* cmd, uint16_t waitResponseTime)
{
	uint16_t len = (uint16_t)strlen(cmd);
	do
	{
		USART_SendData(USART1, (uint8_t)*cmd++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	} while(--len);
	// �ȴ� GSM ��Ӧ
	delayms(waitResponseTime);
}

/*
void SendCmdToGSM(SNED_TO_GSM_MSG_FORMAT format, void* msg, uint16_t waitResponseTime)
{
	if ( format == STRING )
	{
		uint16_t len = (uint16_t)strlen((const char*)msg);
		do
		{
			USART_SendData(USART1, (*(uint8_t*)msg)++);
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		} while(--len);
	}
	else if ( format == NUMBER )
	{
		USART_SendData(USART1, *(uint8_t*)msg);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
}
*/
uint8_t IsTCPServerQuery(const char* QueryStr)
{
	char* ptr = strstr(USART1_RxBuff, QueryStr);
	if (ptr)
	{
		if (strcmp(ptr, QueryStr) == 0)
			return 1;
	}
	return 0;
}


