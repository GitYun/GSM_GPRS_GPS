#include "TCPSever.h"
#include "gsm.h"
#include "delay.h"
#include "usart.h"

#define xRSP_SEND_OK			// 发送成功时返回>和SEND OK
#define ATE0_V0_ENABLE		// 关闭GSM回显，结果代码返回为数字（0代表OK，4代表ERROR）

// Configure Sim900a(GSM module) being a TCP client
uint8_t TCPClient_Config(void)
{
#if defined ( ATE0_V0_ENABLE )
	// 关闭回显，响应结果代码以数字显示，保存当前参数到用户配置表0，开启网络更新时间
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

// 向TCP Server循环发送命令 setCmd，直到响应或超时
CMD_RESPONSE LoopSetUntilResponsed(const char* setCmd, const char* setReponseStr, ...)
{
	CMD_RESPONSE retval;
	volatile char* vaStr;
	va_list arg_ptr;
	// 发送命令得到响应的最大时间：timeOut*(checkCount*GSM_RESPONSE_TIME) (ms)
	uint8_t timeOut = GSM_SEND_CMD_MAX;
	// 等待响应时间：checkCount * GSM_RESPONSE_TIME (ms)
	uint8_t checkCount;
	
	va_start(arg_ptr, setReponseStr);
	vaStr = va_arg(arg_ptr, char*); // 只使用一个扩展参数

	do
	{
		checkCount = GSM_CHECK_RSP_MAX;		// 发送 AT+CIPSTART 命令得到正确响应的检查次数最少为 3
		SendCmdToGSM( setCmd, GSM_RESPONSE_TIME );		
		while ( !ATCmdHand(setReponseStr) && !ATCmdHand((const char*)vaStr) )
		{
			checkCount--;
			if ( checkCount == 0)
				break;
			delayms(GSM_RESPONSE_TIME);	// 继续等待 TCP 连接响应
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

// 设置之前先查询
// 只使用一个扩展参数
CMD_RESPONSE QueryBeforeSet(const char* queryCmd, const char* queryResponse, 
		const char* setCmd, const char* setResponse, ...)
{
	CMD_RESPONSE retval;	
	volatile char* vaStr;
	va_list arg_ptr;
	va_start(arg_ptr, setResponse);
	vaStr = va_arg(arg_ptr, char*);
	
	SendCmdToGSM(queryCmd, GSM_RESPONSE_TIME);
	if ( !ATCmdHand(queryResponse) )	// 是否已设置？
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
	
#if defined ( RSP_SEND_OK )  // 发送成功显示 > 和 SEND OK
	int len = strlen(msg)+2+1;
	char sendMsg[len];
	
	strcpy(sendMsg, msg);
	strcat(sendMsg, "\x1A");	
	if( LoopSetUntilResponsed(AT_CIPSEND, ">", NULL) )
		result = (uint8_t)LoopSetUntilResponsed(sendMsg, "SEND OK", NULL);
#else
	// 发送成功不显示>和SEND OK
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
  * @brief  查找GSM返回信息中是否有给定的字符串
  * @param  str: 要查找的字符串
  * @retval 0 - 查找失败；1 - 查找成功
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


// STM32 通过 USART1 发送命令/数据给GSM(Sim900a)
void SendCmdToGSM(const char* cmd, uint16_t waitResponseTime)
{
	uint16_t len = (uint16_t)strlen(cmd);
	do
	{
		USART_SendData(USART1, (uint8_t)*cmd++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	} while(--len);
	// 等待 GSM 响应
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


