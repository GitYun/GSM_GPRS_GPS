#ifndef __GSM_H
#define __GSM_H

#include "stm32f10x.h"

#define GSM_Status()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#define GSM_PowerOn()		GSM_PowerControl()
#define GSM_PowerOff()						\
{																	\
	if ( GSM_Status() == 1 )				\
		GSM_PowerControl();						\
	delayms(2500);									\
	if ( GSM_Status() == 0  )				\
		return 1;											\
	else														\
		return 0;											\
}

#define CIPSTART_PARAM					"\"TCP\",\"114.215.84.250\",\"8008\""
#define CLPORT_PARAM						"\"TCP\",\"1111\""


#define AT											"AT"
#define ATE0_V0_W0_CLTS					"E0V0&W0+CLTS="
#define ATE1_V1_W0_CLTS  				"E1V1&W0+CLTS="
#define CPOWD										"+CPOWD="
#define CIPSPRT									"+CIPSPRT="
#define CIPSTART 								"+CIPSTART="
#define CIPSTATUS								"+CIPSTATUS="
#define CLPORT									"+CLPORT="
#define CIPCLOSE								"+CIPCLOSE="
#define CIPSHUT									"+CIPSHUT="
#define CIPSEND									"+CIPSEND="


#define AT_CIPSEND							"AT+CIPSEND\r\n"


typedef enum _GSM_CMD_TYPE{
	TEST_CMD	= 0,
	QUERY_CMD,
	SET_CMD,
	EXE_CMD
} gsm_Cmd_Tpye;


extern char completeCmd[100];

/*
//extern char* AT;

//extern char* AT_CPOWD0;
//extern char* AT_CPOWD1;

//extern char* ATE0_V0_W0_CLTS1;
//extern char* ATE1_V1_W0_CLTS1;
//extern char* AT_CLPORT_Query_CIPSTATUS_Query;

//extern char* AT_CIPSPRT0;
//extern char* AT_CIPSPRT1;
//extern char* AT_CIPSPRT2;
*/

/*
extern char* ATE_OFF;
extern char* ATE_ON;
extern char* ATV0;
extern char* ATV1;
extern char* AT_W0;
extern char* AT_W1;
extern char* AT_V;

extern char* AT_CGCLASS_Query;
extern char* AT_CGCLASS;

extern char* AT_CGDCONT_Query;
extern char* AT_CGDCONT;

extern char* AT_CGATT_Query;
extern char* AT_CGATT_OFF;
extern char* AT_CGATT_ON;
*/

/*
//extern char* AT_CIPCSGP_Query;
//extern char* AT_CIPCSGP;

//extern char* AT_CLPORT_Query;
//extern char* AT_CLPORT_TCP;
//extern char* AT_CLPORT_UDP;

//extern char* AT_CIPMUX_Query;
//extern char* AT_CIPMUX_Single;
//extern char* AT_CIPMUX_Multi;

//extern char* AT_CIPSERVER_Query;
//extern char* AT_CIPSERVER;

//extern char* AT_CIPSTART_Test;
//extern char* AT_CIPSTART;

//extern char* AT_CIPSTATUS;

//extern char* AT_CIPSEND;

//extern char* AT_CIPCLOSE;
//extern char* AT_CIPCLOSE_Fast;
//extern char* AT_CIPCLOSE_Slow;

//extern char* AT_CIPSHUT;

//extern char* AT_CLTS_ON;
//extern char* AT_CLTS_OFF;

//extern char* AT_CCLK_Query;
//extern char* AT_CCLK;

//extern char* AT_IPR_Query;
//extern char* AT_IPR;
*/

char* GSM_CMD(gsm_Cmd_Tpye type, char* cmd, char* param);

//uint8_t GSM_Status(void);
void GSM_Status_GPIO_Config(void);
//void GSM_PowerOn(void);
void GSM_PowerControl(void);
uint8_t GSM_PowerReverse(void);
void GSM_SoftPower_GPIO_Config(void);
uint8_t GSM_Reboot(void);

#endif
