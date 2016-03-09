#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void delay_nus(unsigned long n);
void delayms(unsigned long n);

void TIMx_Config(TIM_TypeDef* TIMx);
void TIMx_Start(TIM_TypeDef* TIMx);
void TIMx_Stop(TIM_TypeDef* TIMx);
void TIM_NVIC_Config(uint8_t TIMx_IT_IRQ);
#endif
