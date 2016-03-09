/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "tcpsever.h"
#include "gps.h"
#include "usart.h"
#include "delay.h"

extern uint8_t getPositionFlag;

// 循环发送数据测试
extern uint8_t cycleSendDataFlag;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPS_DATA_BUF gps_buf_usart2;		//gps
GPS_FLAG gps_flag;
/* Private function prototypes -----------------------------------------------*/
void receiveGpsInformation(uint8_t ch);
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		SendMsgToTCPServer("Error 0x0001");
		delayms(1000);
		// 清硬件fault状态寄存器的状态标志，写1清除
		SCB->HFSR |= SCB_HFSR_DEBUGEVT | SCB_HFSR_FORCED | SCB_HFSR_VECTTBL;
		/* 
		** 说明：特殊功能寄存器只能用 MSR、MRS 汇编指令来访问，且也没有与之相关的访问地址；
		** 			 CM3 还专门设置了一条 CPS 指令，用来访问中断屏蔽寄存器（PRIMASK, FAULRMASK, BASEPRI）
		**---------------- 使用 CPS 指令 ---------------------------
		** CPSIE	i			;清除 PRIMASK = 0，使能中断
		** CPSID	i			;置位 PRIMASK = 1，屏蔽中断（除NMI和硬Fault外）
		** --------------- 使用 MSR、MRS 指令 ----------------------
		** MOVS	R0, #1	;存储立即数1到寄存器R0
		** MSR	PRIMASK, R0		;加载寄存器R0中的值到PRIMASK寄存器中
		** --------------- 使用 CMSIS API --------------------------
		** __disable_irq();		// 屏蔽除NMI和硬Fault外的所有中断
		** __enable_irq();		// 使能中断
		*/
		__disable_irq();
		// SCB 在 core_cm3.h 中预定义，为SCB_Type指针类型
		// SCB_AIRCR_SYSRESETREQ 在 stm32f10x.h 中定义，值为4
		// SCB_AIRCR_VECTRESET 在 stm32f10x.h 中定义，值为1
		SCB->AIRCR |= (AIRCR_VECTKEY_WRITE | SCB_AIRCR_VECTRESET);	// 产生系统复位
		for(;;);
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  接收 TCP Server 的发来的数据或向 TCP Server 发送 GPS 定位数据
  * @param  无
  * @retval 无
  */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		USART1_RxBuff[USART1_RxCount++] = USART_ReceiveData(USART1);		
		
		if (USART1_RxCount >= RxBUF_MAX_LENTH)
		{
			USART1_RxCount = 0;
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	
//	if (USART_GetITStatus(USART1, USART_IT_TXE) == SET)
//	{		
//		USART_ClearITPendingBit(USART1, USART_IT_TXE);
//	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
	if ( USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET )
	{
		USART1_RxBuff[USART1_RxCount++] = USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}	
}

void USART2_IRQHandler(void)
{
	char ch;
	if (USART_GetFlagStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART2);
		USART2_RxBuff[USART2_RxCount++] = ch;
		receiveGpsInformation(ch);
		
		if (USART2_RxCount >= RxBUF_MAX_LENTH)
		{
			USART2_RxCount = 0;
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
	if ( USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET )
	{
		receiveGpsInformation(USART_ReceiveData(USART2));
		USART_ClearFlag(USART2, USART_FLAG_ORE);
	}
}

/**
  * @brief  GPS 数据接收处理
  * @param  ch - 当前接收到的字符
  * @retval 无
  */
void receiveGpsInformation(uint8_t ch)
{
	if ( gps_flag.gpsAnalysis || !getPositionFlag )
		return;
	
	//如果收到字符'$'，便开始接收
	if ( (ch == '$') && (gps_flag.gpsAnalysis == 0) )	
	{
		gps_flag.gpsReceiver = 1;		//开始接收第一个数据，接收完清零
		gps_flag.gpsStop = 0;				//开始接收，接收完置位，表明停止接收了
	}
	
	if ( gps_flag.gpsReceiver )
	{
		gps_buf_usart2.buf[gps_buf_usart2.index++] = ch;
		
		//如果接收到换行，则一行接收完毕
		if ( ch == '\n' )
		{
			gps_buf_usart2.buf[gps_buf_usart2.index] = '\0';
			gps_flag.gpsReceiver = 0;
			gps_flag.gpsStop = 1;
			gps_flag.gpsAnalysis = 1;
			gps_buf_usart2.index = 0;
		}
	}
}


void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET )
	{
		// 1000ms 中断一次
		TIMx_Stop(TIM2);
		TIM2->CNT = 0;
		cycleSendDataFlag = 0;
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
