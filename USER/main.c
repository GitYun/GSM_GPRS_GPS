#include "stm32f10x.h"
#include "gsm.h"
#include "gps.h"
#include "tcpsever.h"
#include "usart.h"
#include "delay.h"
#include "data.h"

extern char* USART1_RxBuff;
extern char* USART2_RxBuff;
extern uint8_t USART1_RxCount;
extern uint8_t USART2_RxCount;

extern GPS_FLAG gps_flag;
extern GPS_DATA_BUF gps_buf_usart2;

TCP_CONNECT_STATUS tcpStatus;

GPS_INFO gpsInformation;
uint8_t isGpsFixPosition = 0;
uint8_t gpsDataInvaild = 0;
uint8_t gpsDataInvaildCounter = 0;
uint8_t getPositionFlag = 0;

// 循环发送数据测试
uint8_t cycleSendDataFlag = 0;
//static uint8_t dataSeqCounter = 0;

/**************** Private Function Prototype ********************/
static void tcpServerData_Handler(void);
static void gpsDataAnalysis(void); 

int main(void) 
{
	// RCC控制/状态寄存器
	if ( RCC->CSR & RCC_CSR_SFTRSTF )	// 软件复位标志位是否置位
		RCC->CSR |= RCC_CSR_RMVF;				// 写RMVF位清除复位标志
	
	tcpStatus.tcpConnectFlag = 0;	// TCP 链接状态初始化
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
												 RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_TIM2, ENABLE);
	
	GSM_Status_GPIO_Config();
	GSM_SoftPower_GPIO_Config();
	USART_GPIO_Config(USART1);
	USART_GPIO_Config(USART2);
	USARTx_Init(USART1, 115200);		// Connecting USART1 to Sim900a(GSM module)
	USARTx_IT_Cmd(USART1, ENABLE);
	USARTx_Init(USART2, 9600); 			// for Debug or receive GPS/zigbee data packs	
	USART_NVIC_Config();
	TIMx_Config(TIM2);
	TIM_NVIC_Config(TIM2_IRQn);
	
	if ( GSM_Status() == 0 )
		GSM_PowerOn();		// GSM 模块开机
	delayms(1000);

	do
	{
		static uint8_t rebootCount= 1;
		// 正确 TCP 连接大概需要耗时 5s
		if ( TCPClient_Config() == 1 )
			tcpStatus.tcpConnectFlag = 1;
		else
		{
			if ( rebootCount == 0 )
			{
				// to do 发短信报告错误
				for(;;)
				{
					// to do 接收短信重启设备
				}
			}
			rebootCount--;
			GSM_Reboot();		// GSM 模块重启
		}
	} while ( tcpStatus.tcpConnectFlag == 0 );
		
	SendMsgToTCPServer("TCP Client has been connected.\x0D\x0A"); // 函数将在发送字符串末尾添加 "\x1A"
	Clear_USART1Buf();

	while(1)
	{
		while(tcpStatus.tcpConnectFlag) 
		{
			if ( USART1_RxCount )	// USART1 接收来自 TCP Server 的数据
			{
				tcpServerData_Handler();
			}

			// 解析 GPS 数据
			if ( isGpsAnalysising() )
			{
				gpsDataAnalysis();
				if ( isGpsFixPosition )
				{
					char sendString[28];
					isGpsFixPosition = 0;
					
					outputLatitudeLongitude(&gpsInformation, (LATI_LONG_FORMAT)DMS_F);
					
					strcpy(sendString, gpsInformation.NS);
					strcat(sendString, gpsInformation.latitude);
					strcat(sendString, ",");
					strcat(sendString, gpsInformation.EW);
					strcat(sendString, gpsInformation.longitude);
					strcat(sendString, "\r\n");
					SendMsgToTCPServer(sendString);
					memset(sendString, 0, 28);
					
					sprintf(sendString, "%f", gpsInformation.speed);
					SendMsgToTCPServer(sendString);
					memset(sendString, 0, 28);
					
					sprintf(sendString, "%f", gpsInformation.direction);
					SendMsgToTCPServer(sendString);
					
					memset(gpsInformation.latitude, 0, 11);
					memset(gpsInformation.longitude, 0, 12);
				}
			}
			
			// for test
			if ( cycleSendDataFlag )	
			{
				SendMsgToTCPServer( (const char*)&dataTable[0] );
			}
			
#if defined ( USART2_ZIGBEE )			
			// send data packets from the zigbee to the TCP server
			if (USART2_RxCount)
			{
				char* msg;
				Clear_USART1Buf();
				SendCmdToGSM(AT_CCLK_Query);	// 获取时间
				delayms(500);
				msg = (char*)calloc((USART2_RxCount + USART1_RxCount - 10), sizeof(char));
				msg = strncpy(msg, &USART1_RxBuff[7], (USART1_RxCount -8));	// 时间戳格式 +CCLK: <time>
				msg = strncat(msg, USART2_RxBuff, (USART2_RxCount -1));		// ZigBee 数据内容
				SendMsgToTCPServer(msg);
				free(msg);
				Clear_USART1Buf();
				Clear_USART2Buf();
			}
#endif // USART2_ZIGBEE
		}
		
		if ( GSM_Status() == 1 )
		{
			if ( LoopSetUntilResponsed(GSM_CMD(QUERY_CMD, CIPSTATUS, NULL), 
				"IP INITIAL", NULL) == (CMD_RESPONSE)LOOP_CMD_RESPONSE_OK )
			{
				if ( TCPClient_Reconnect() == 1 )
				{
					tcpStatus.tcpConnectFlag = 1;
					SendMsgToTCPServer("\r\nTCP Client has been reconnected.\r\n");
				}			
			}
			else
			{
				LoopSetUntilResponsed( GSM_CMD(EXE_CMD, CIPSHUT, NULL), "SHUT OK", NULL );
			}
		}
	}
}


static void tcpServerData_Handler()
{
	// 检查从TCP Server接收到的数据是否是数据结束符
	if (USART1_RxBuff[USART1_RxCount-1] == '#')
	{
		if (IsTCPServerQuery("TCP Close#"))
		{
			SendMsgToTCPServer("\r\nTCP Client closeing...\r\n");	
			if( TCPClient_Close() )
			{				
				tcpStatus.tcpConnectFlag = 0;
				LoopSetUntilResponsed( GSM_CMD( EXE_CMD, CIPSHUT, NULL), "SHUT OK", NULL);							
			}
			else
				SendMsgToTCPServer("TCP Client close fail.");
		}
		else if ( IsTCPServerQuery("Get Position#") )
		{
			if ( getPositionFlag == 1 )
				SendMsgToTCPServer("\r\nProcessing...\r\n");
			else
			{
				USARTx_IT_Cmd(USART2, ENABLE);
				getPositionFlag = 1;	// 置 TCP Server 获取一次定位标志位	
			}			
		}
		else if ( IsTCPServerQuery("Cycle send#") )
		{
			cycleSendDataFlag = 1;
			TIMx_Start(TIM2);
		}
		else if ( IsTCPServerQuery("GSM Shutdown#") )
		{
			SendMsgToTCPServer("\r\nGSM Shutdowning...\r\n");
			if ( TCPClient_Close() )
			{
				tcpStatus.tcpConnectFlag = 0;
				if ( !LoopSetUntilResponsed( GSM_CMD(SET_CMD, CPOWD, "1"), "NORMAL POWER DOWN", NULL) )
					SendMsgToTCPServer("\r\nGSM Shutdown fail.\r\n");
			}
		}
		else if ( IsTCPServerQuery("Device Reboot#") )
		{
			SendMsgToTCPServer("\r\nWaiting...\r\n");
			if ( TCPClient_Close() )
			{
				LoopSetUntilResponsed( GSM_CMD(EXE_CMD, CIPSHUT, NULL), "SHUT OK", NULL);
				if ( GSM_PowerReverse() == 1 )
				{
					// 设置 PRIMASK = 1
					__disable_irq();	// 屏蔽除NMI和硬Fault外的所有中断
/*					
					__set_PRIMASK((uint32_t)1);	// 作用同上一语句
					SCB->AIRCR |= (AIRCR_VECTKEY_WRITE | SCB_AIRCR_VECTRESET);	// 产生系统复位
					for(;;);	// 等待中断
*/
					NVIC_SystemReset();	// 产生系统复位
				}
			}
		}
		else
		{
			SendMsgToTCPServer("\r\nNot this cmd.\r\n");
		}
		Clear_USART1Buf();	
	}	
}

static void gpsDataAnalysis()
{
#if defined ( USART2_DEBUG )
	// 调试时GPS信息从串口2打印出来表示串口2接收数据没有问题
	printf(gps_buf_usart2.buf);
#endif
	// 解析GPRMC 数据类型，解析成功后数据存储在 GPS_INFO 数据结构变量 gpsInformation 中
	if ( parseGpsData(gps_buf_usart2.buf, &gpsInformation, (GPS_DATA_TYPE)GPRMC) )	
	{	
		isGpsFixPosition = 1;		// 已经定位好了
		getPositionFlag = 0;		// 清 TCP Server 获取一次定位信标志位
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
	else
	{
		gpsDataInvaildCounter++;
		if ( gpsDataInvaildCounter >= 20 )	// 如果数据无效超过20次
		{
			gpsDataInvaildCounter = 0;
			gpsDataInvaild = 1;
		}
	}
	
	cleanGpsDataBuf(gps_buf_usart2.buf);
	Clear_USART2Buf();	
	cleanGpsFlag(&gps_flag);		//	gps_flag.gpsAnalysis = 0;
}
