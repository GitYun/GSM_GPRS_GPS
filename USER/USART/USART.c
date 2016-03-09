#include "usart.h"
#include "string.h"


char* USART1_RxBuff;
uint8_t USART1_RxCount;
char* USART2_RxBuff;
uint8_t USART2_RxCount;


void USART_GPIO_Config(USART_TypeDef* USARTx)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (USARTx == USART1)
	{
		// Tx1
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// Rx1
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if (USARTx == USART2)	// for Debug
	{
		//Configure USART1 Tx (PA 2) as alternate function push-pull
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		// Configure USART1 Rx (RA 3) as input floating
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
		GPIO_Init(GPIOA, &GPIO_InitStruct);		
	}
}


void USART_Config(USART_TypeDef* USARTx, uint32_t baudRate)
{
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = baudRate;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx, &USART_InitStruct);
}

void USARTx_IT_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
	USART_ITConfig(USARTx, USART_IT_RXNE, NewState);
	USART_Cmd(USARTx, NewState);
}

void USARTx_Init(USART_TypeDef* USARTx, uint32_t baudRate)
{
	if (USARTx == USART1)
	{
		USART1_RxCount = 0;
		USART1_RxBuff = (char*)calloc(RxBUF_MAX_LENTH, sizeof(char));
		USART_Config(USARTx, baudRate);
	}
	else if (USARTx == USART2)
	{
		USART2_RxCount = 0;		
		USART2_RxBuff = (char*)calloc(RxBUF_MAX_LENTH, sizeof(char));
		USART_Config(USARTx, baudRate);
	}
}


void USART_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStruct);
	
	
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStruct);
}

/*
void USART2_Config() {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	// config USART1 clock
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	// USART2 GPIO config
	//Configure USART1 Tx (PA 2) as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure USART1 Rx (RA 3) as input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// USART2 mode config
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
}
*/

void Clear_USART1Buf(void)
{
	memset(USART1_RxBuff, 0, RxBUF_MAX_LENTH);
  USART1_RxCount = 0;                   // 接收字符串的起始存储位置
}

void Clear_USART2Buf(void)
{
	memset(USART2_RxBuff, 0, RxBUF_MAX_LENTH);
  USART2_RxCount = 0;                   // 接收字符串的起始存储位置
}

// 用于输出调试信息
int fputc(int ch, FILE* f)
{
	USART_SendData(USART2, (unsigned char)ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
	return ch;
}

