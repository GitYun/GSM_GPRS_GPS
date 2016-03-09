#include "gsm.h"
#include "delay.h"
#include "string.h"

char completeCmd[100];

/*
// 用于设置SIM900A的握手
char* AT = "AT\r\n";

// 用于关机, 此模式中，RTC单元仍然工作，能通过 STATUS 引脚获知此模式
char* AT_CPOWD0 = "AT+CPOWD=0\r\n";	// 紧急关机
char* AT_CPOWD1 = "AT+CPOWD=1\r\n";	// 正常关机，提示NORMAL POWER DOWN

// 精简设置指令
// 关闭回显，设置TA响应格式，保存当前配置到用户配置表0，获取本地时间戳（开启网络更新时间）
char* ATE0_V0_W0_CLTS1 = "ATE0V0&W0+CLTS=1\r\n";

char* ATE1_V1_W0_CLTS1 = "ATE1V1&W0+CLTS=1\r\n";

char* AT_CLPORT_Query_CIPSTATUS_Query = "AT+CLPORT?;+CIPSTATUS?\r\n";

char* AT_CIPSPRT0 ="AT+CIPSPRT=0\r\n"; // 不显示>，显示SEND OK
char* AT_CIPSPRT1 ="AT+CIPSPRT=1\r\n"; // 显示>，显示SEND OK(默认)
char* AT_CIPSPRT2 ="AT+CIPSPRT=2\r\n"; // 不显示>，不显示SEND OK
*/

/*
// 用于开启或关闭回显模式
char* ATE_OFF = "ATE0\r\n";	// 回显模式关闭
char* ATE_ON = "ATE1\r\n";	// 回显模式打开

// 用于设置 TA 响应内容的格式
char* ATV0 = "ATV0\r\n";		// 信息返回结果：<text><CR><LF>
														// 短数字型结果码：<numeric code><CR>
char* ATV1 = "ATV1\r\n";		// 信息返回结果：<CR><LF><text><CR><LF>
														// 长字符型结果码：<CR><LF><verbose code><CR><LF>

// 用于保存当前配置到非易失性存储器中
char* AT_W0 = "AT&W0\r\n";	// 保存当前配置到用于配置表0中
char* AT_W1 = "AT&W1\r\n";

// 用于显示当前配置
char* AT_V	= "AT&V\r\n";

// 用于设置移动台类别
char* AT_CGCLASS_Query = "AT+CGCLASS?\r\n";	// 查询指令
char* AT_CGCLASS = "AT+CGCLASS=\"B\"\r\n";

// 用于设置PDP上下文
char* AT_CGDCONT_Query = "AT+CGDCONT?\r\n";
char* AT_CGDCONT = "AT+CGDCON=1,\"IP\",\"CMNET\"\r\n";

// 用于设置附着和分离GPRS业务; 0--分离GPRS，1--附着GPRS
char* AT_CGATT_Query = "AT+CGATT?\r\n";
char* AT_CGATT_OFF = "AT+CGATT=0\r\n";
char* AT_CGATT_ON = "AT+CGATT=1\r\n";
*/

/*
// 用于设置使/失能睡眠模式
char* AT_SLEEP_ON = "AT+CSCLK=1\r\n";
char* AT_SLEEP_OFF = "AT+CSCLK=0\r\n";

// 用于设置CSD或GPRS链接模式
char* AT_CIPCSGP_Query = "AT+CIPCSGP?\r\n";
char* AT_CIPCSGP = "AT+CIPCSGP=1,\"CMNET\"\r\n";

// 用于设置本地端口号
char* AT_CLPORT_Query = "AT+CLPORT?\r\n";
char* AT_CLPORT_TCP = "AT+CLPORT=\"TCP\",\"1111\"\r\n";
char* AT_CLPORT_UDP = "AT+CLPORT=\"UDP\",\"2222\"\r\n";

// 用于设置为TCP或UDP单路或多路链接; 0--单路链接，1--多路链接
char* AT_CIPMUX_Query = "AT+CIPMUX?\r\n";
char* AT_CIPMUX_Single = "0\r\n";
char* AT_CIPMUX_Multi = "1\r\n";

// 用于配置GSM为服务器
char* AT_CIPSERVER_Query = "AT+CIPSERVER?\r\n";
char* AT_CIPSERVER = "AT+CIPSERVER=1,\"8008\"\r\n";

// 用于配置GSM为客户端
char* AT_CIPSTART_Test = "AT+CIPSTART=?\r\n";	// 	测试命令
char* AT_CIPSTART = "AT+CIPSTART=\"TCP\",\"114.215.84.250\",\"8008\"\r\n";

// 用于查询TCP或UDP链接的状况
char* AT_CIPSTATUS = "AT+CIPSTATUS\r\n";

// 用于发送数据
char* AT_CIPSEND = "AT+CIPSEND\r\n";

// 用于关闭TCP/UDP连接
char* AT_CIPCLOSE = "AT+CIPCLOSE\r\n";			// 关闭
char* AT_CIPCLOSE_Fast = "AT+CIPCLOSE=1\r\n";	// 快关
char* AT_CIPCLOSE_Slow = "AT+CIPCLOSE=0\r\n";	// 慢关

// 用于关闭移动场景
char* AT_CIPSHUT = "AT+CIPSHUT\r\n";

// 用于获取本地时间戳
char* AT_CLTS_ON = "AT+CLTS=1\r\n";		// 开启，更新网络时间
char* AT_CLTS_OFF = "AT+CLTS=0\r\n";	// 关闭

// 用于设置或显示当前时间
char* AT_CCLK_Query = "AT+CCLK?\r\n";

// 用于设置 TE-TA 固定波特率
char* AT_IPR_Query = "AT+IPR?\r\n";
char* AT_IPR = "AT+IPR=0\r\n";	// 自适应波特率
*/




char* GSM_CMD(gsm_Cmd_Tpye type, char* cmd, char* param) 
{
	memset(completeCmd, 0, 100);
	strcpy(completeCmd, AT);
	strcat(completeCmd, cmd);	
	if ( type == QUERY_CMD || type == EXE_CMD )																
	{																												
		char* c = strchr(completeCmd, '=');
		if ( c != NULL )																			
			*c = '\0';																					
	}
	else
	{
		(void)type;
	}
	
	if ( param != NULL )																													
		strcat(completeCmd, param);															
	strcat(completeCmd, "\r\n");															
	return completeCmd;																				
}

/**
  * @brief  检查GSM模块的状态
  * @param  无
  * @retval uint8_t: 1 - GSM处于开机状态；0 - GSM处于掉电状态
  */
/*
uint8_t GSM_Status()
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);
}
*/

void GSM_SoftPower_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	// GSM SoftPowen Pin(GPIOC.01)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}


void GSM_Status_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void GSM_PowerControl()
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_1, (BitAction)1);
	delayms(700);
	GPIO_WriteBit(GPIOC, GPIO_Pin_1, (BitAction)0);
}


/**
  * @brief  控制 GSM 模块的开关机切换
  * @param  无
  * @retval 1 - 切换成功；0 - 切换失败
  */
uint8_t GSM_PowerReverse()
{
	uint8_t status = GSM_Status();
	
	GSM_PowerControl();
	delayms(2500);
	if ( GSM_Status() != status )
		return 1;
	else
		return 0;
}

/**
  * @brief  重启 GSM 模块
  * @param  无
	* @retval retval: 1 - 重启成功；0 - 重启失败
  */
uint8_t GSM_Reboot(void)
{
	uint8_t retval = 0;
	if ( GSM_PowerReverse() == 1 )
		retval = GSM_PowerReverse();
	
	return retval;
}
