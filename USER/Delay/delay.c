#include "delay.h"

void delay_nus(unsigned long n)
{
	unsigned long j;
	if ( n == 0 )
		return;
	while(n--)
	{
		j=12;
		while(j--);
	}
}

void delayms(unsigned long n)
{
	if ( n == 0 )
		return;
	while(n--)
	{
		delay_nus(1030);
	}
}

void TIMx_Config(TIM_TypeDef* TIMx)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	// 脉冲计数器频率 f = 72MHz / 7200 = 0.01MHz, T = 0.1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseStructure.TIM_Period = 9999;			// 0.1ms * (9999+1) = 1000 ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 配置为任意值都无影响
	
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	
	TIM_ClearFlag(TIMx, TIM_IT_Update);
}

void TIMx_Start(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, ENABLE);
}

void TIMx_Stop(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}

void TIM_NVIC_Config(uint8_t TIMx_IT_IRQ)
{
	NVIC_InitTypeDef NVIC_InitStruture;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStruture.NVIC_IRQChannel = TIMx_IT_IRQ;
	NVIC_InitStruture.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruture.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruture.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruture);
	
	
}
