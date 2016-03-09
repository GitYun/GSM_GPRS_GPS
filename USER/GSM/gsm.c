#include "gsm.h"
#include "delay.h"
#include "string.h"

char completeCmd[100];

/*
// ��������SIM900A������
char* AT = "AT\r\n";

// ���ڹػ�, ��ģʽ�У�RTC��Ԫ��Ȼ��������ͨ�� STATUS ���Ż�֪��ģʽ
char* AT_CPOWD0 = "AT+CPOWD=0\r\n";	// �����ػ�
char* AT_CPOWD1 = "AT+CPOWD=1\r\n";	// �����ػ�����ʾNORMAL POWER DOWN

// ��������ָ��
// �رջ��ԣ�����TA��Ӧ��ʽ�����浱ǰ���õ��û����ñ�0����ȡ����ʱ����������������ʱ�䣩
char* ATE0_V0_W0_CLTS1 = "ATE0V0&W0+CLTS=1\r\n";

char* ATE1_V1_W0_CLTS1 = "ATE1V1&W0+CLTS=1\r\n";

char* AT_CLPORT_Query_CIPSTATUS_Query = "AT+CLPORT?;+CIPSTATUS?\r\n";

char* AT_CIPSPRT0 ="AT+CIPSPRT=0\r\n"; // ����ʾ>����ʾSEND OK
char* AT_CIPSPRT1 ="AT+CIPSPRT=1\r\n"; // ��ʾ>����ʾSEND OK(Ĭ��)
char* AT_CIPSPRT2 ="AT+CIPSPRT=2\r\n"; // ����ʾ>������ʾSEND OK
*/

/*
// ���ڿ�����رջ���ģʽ
char* ATE_OFF = "ATE0\r\n";	// ����ģʽ�ر�
char* ATE_ON = "ATE1\r\n";	// ����ģʽ��

// �������� TA ��Ӧ���ݵĸ�ʽ
char* ATV0 = "ATV0\r\n";		// ��Ϣ���ؽ����<text><CR><LF>
														// �������ͽ���룺<numeric code><CR>
char* ATV1 = "ATV1\r\n";		// ��Ϣ���ؽ����<CR><LF><text><CR><LF>
														// ���ַ��ͽ���룺<CR><LF><verbose code><CR><LF>

// ���ڱ��浱ǰ���õ�����ʧ�Դ洢����
char* AT_W0 = "AT&W0\r\n";	// ���浱ǰ���õ��������ñ�0��
char* AT_W1 = "AT&W1\r\n";

// ������ʾ��ǰ����
char* AT_V	= "AT&V\r\n";

// ���������ƶ�̨���
char* AT_CGCLASS_Query = "AT+CGCLASS?\r\n";	// ��ѯָ��
char* AT_CGCLASS = "AT+CGCLASS=\"B\"\r\n";

// ��������PDP������
char* AT_CGDCONT_Query = "AT+CGDCONT?\r\n";
char* AT_CGDCONT = "AT+CGDCON=1,\"IP\",\"CMNET\"\r\n";

// �������ø��źͷ���GPRSҵ��; 0--����GPRS��1--����GPRS
char* AT_CGATT_Query = "AT+CGATT?\r\n";
char* AT_CGATT_OFF = "AT+CGATT=0\r\n";
char* AT_CGATT_ON = "AT+CGATT=1\r\n";
*/

/*
// ��������ʹ/ʧ��˯��ģʽ
char* AT_SLEEP_ON = "AT+CSCLK=1\r\n";
char* AT_SLEEP_OFF = "AT+CSCLK=0\r\n";

// ��������CSD��GPRS����ģʽ
char* AT_CIPCSGP_Query = "AT+CIPCSGP?\r\n";
char* AT_CIPCSGP = "AT+CIPCSGP=1,\"CMNET\"\r\n";

// �������ñ��ض˿ں�
char* AT_CLPORT_Query = "AT+CLPORT?\r\n";
char* AT_CLPORT_TCP = "AT+CLPORT=\"TCP\",\"1111\"\r\n";
char* AT_CLPORT_UDP = "AT+CLPORT=\"UDP\",\"2222\"\r\n";

// ��������ΪTCP��UDP��·���·����; 0--��·���ӣ�1--��·����
char* AT_CIPMUX_Query = "AT+CIPMUX?\r\n";
char* AT_CIPMUX_Single = "0\r\n";
char* AT_CIPMUX_Multi = "1\r\n";

// ��������GSMΪ������
char* AT_CIPSERVER_Query = "AT+CIPSERVER?\r\n";
char* AT_CIPSERVER = "AT+CIPSERVER=1,\"8008\"\r\n";

// ��������GSMΪ�ͻ���
char* AT_CIPSTART_Test = "AT+CIPSTART=?\r\n";	// 	��������
char* AT_CIPSTART = "AT+CIPSTART=\"TCP\",\"114.215.84.250\",\"8008\"\r\n";

// ���ڲ�ѯTCP��UDP���ӵ�״��
char* AT_CIPSTATUS = "AT+CIPSTATUS\r\n";

// ���ڷ�������
char* AT_CIPSEND = "AT+CIPSEND\r\n";

// ���ڹر�TCP/UDP����
char* AT_CIPCLOSE = "AT+CIPCLOSE\r\n";			// �ر�
char* AT_CIPCLOSE_Fast = "AT+CIPCLOSE=1\r\n";	// ���
char* AT_CIPCLOSE_Slow = "AT+CIPCLOSE=0\r\n";	// ����

// ���ڹر��ƶ�����
char* AT_CIPSHUT = "AT+CIPSHUT\r\n";

// ���ڻ�ȡ����ʱ���
char* AT_CLTS_ON = "AT+CLTS=1\r\n";		// ��������������ʱ��
char* AT_CLTS_OFF = "AT+CLTS=0\r\n";	// �ر�

// �������û���ʾ��ǰʱ��
char* AT_CCLK_Query = "AT+CCLK?\r\n";

// �������� TE-TA �̶�������
char* AT_IPR_Query = "AT+IPR?\r\n";
char* AT_IPR = "AT+IPR=0\r\n";	// ����Ӧ������
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
  * @brief  ���GSMģ���״̬
  * @param  ��
  * @retval uint8_t: 1 - GSM���ڿ���״̬��0 - GSM���ڵ���״̬
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
  * @brief  ���� GSM ģ��Ŀ��ػ��л�
  * @param  ��
  * @retval 1 - �л��ɹ���0 - �л�ʧ��
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
  * @brief  ���� GSM ģ��
  * @param  ��
	* @retval retval: 1 - �����ɹ���0 - ����ʧ��
  */
uint8_t GSM_Reboot(void)
{
	uint8_t retval = 0;
	if ( GSM_PowerReverse() == 1 )
		retval = GSM_PowerReverse();
	
	return retval;
}
