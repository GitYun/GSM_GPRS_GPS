#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"

// AIRC �Ĵ�����[31:16]��ע����
#define AIRCR_VECTKEY_WRITE	0x05FA0000		// д����ע����
#define AIRCR_VECTKEY_READ	0xFA050000		// ������ע����

#define RxBUF_MAX_LENTH	100	

extern char* USART1_RxBuff;
extern uint8_t USART1_RxCount;

extern char* USART2_RxBuff;
extern uint8_t USART2_RxCount;

void USART_GPIO_Config(USART_TypeDef* USARTx);
void USART_Config(USART_TypeDef* USARTx, uint32_t baudRate);
void USARTx_IT_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USARTx_Init(USART_TypeDef* USARTx, uint32_t baudRate);

void USART_NVIC_Config(void);

void Clear_USART1Buf(void);
void Clear_USART2Buf(void);

int fputc(int ch, FILE* f);

#endif
