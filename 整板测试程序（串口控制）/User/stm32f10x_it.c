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
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

// printf()
#include <stdio.h>

// 摄像头-火眼ov7725
#include "bsp_ov7725.h"

// usb mass storage
#include "bsp_sdio_sdcard.h"
#include "usb_lib.h"
#include "usb_istr.h"


// 摄像头-火眼ov7725
extern u8 Ov7725_vsync;

// iso 整板测试
volatile uint8_t rec_cmd;
extern void Soft_Reset(void);

// 触摸屏
volatile unsigned char touch_flag;

// 系统定时器
extern void TimingDelay_Decrement(void);

// 呼吸灯
extern uint8_t indexWave[];

// can 
extern __IO uint32_t ret;

// rtc
extern uint32_t TimeDisplay;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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
  TimingDelay_Decrement();
}

/* 串口1接收中断 */
void USART1_IRQHandler(void)
{
   //uint8_t rec_cmd;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{     
			rec_cmd = USART_ReceiveData(USART1);
      //printf("%c \r\n", rec_cmd);
      if(rec_cmd == '\r' )  // 回车
      {
        Soft_Reset();
      }
	}	 
}

// EXTI Line --> PF9 触摸屏中断
void EXTI9_5_IRQHandler(void)
{ 
  if(EXTI_GetITStatus(EXTI_Line9) != RESET)
  {	
    touch_flag=1;
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}

/* ov7725 场中断 服务程序 */
void EXTI0_IRQHandler(void)
{
    if ( EXTI_GetITStatus(EXTI_Line0) != RESET ) 	//检查EXTI_Line0线路上的中断请求是否发送到了NVIC 
    {
        if( Ov7725_vsync == 0 )
        {
            FIFO_WRST_L(); 	                      //拉低使FIFO写(数据from摄像头)指针复位
            FIFO_WE_H();	                        //拉高使FIFO写允许
            
            Ov7725_vsync = 1;	   	
            FIFO_WE_H();                          //使FIFO写允许
            FIFO_WRST_H();                        //允许使FIFO写(数据from摄像头)指针运动
        }
        else if( Ov7725_vsync == 1 )
        {
            FIFO_WE_L();                          //拉低使FIFO写暂停
            Ov7725_vsync = 2;
        }        
        EXTI_ClearITPendingBit(EXTI_Line0);		    //清除EXTI_Line0线路挂起标志位        
    }    
}

/* 呼吸灯中断服务函数 */
void TIM3_IRQHandler(void)
{	
	static uint8_t pwm_index = 0;			//用于PWM查表
	static uint8_t period_cnt = 0;		//用于计算周期数
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;
			if(period_cnt >= 10)										//若输出的周期数大于10，输出下一种脉冲宽的PWM波
			{
				
				TIM3->CCR3 = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
				pwm_index++;												//标志PWM表的下一个元素
			
				if( pwm_index >=  40)								//若PWM脉冲表已经输出完成一遍，重置PWM查表标志
				{
					pwm_index=0;								
				}
				
				period_cnt=0;												//重置周期计数标志
		}
		TIM_ClearITPendingBit (TIM3, TIM_IT_Update);	//必须要清除中断标志位
	}
}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB中断和CAN接收中断服务程序，USB跟CAN公用I/O，这里只用到CAN的中断。
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
  
	if( CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET )
  {
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    
    if((RxMessage.ExtId==0x1234)  && 
       (RxMessage.IDE==CAN_ID_EXT)&& 
       (RxMessage.DLC==2)         && 
       ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xDECA))
    {
      ret = 1; 
    }
    else
    {
      ret = 0; 
    }
  }
  else
  {
    USB_Istr();
  }
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	  {
	    /* Clear the RTC Second interrupt */
	    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	    /* Enable time update */
	    TimeDisplay = 1;
	
	    /* Wait until last write operation on RTC registers has finished */
	    RTC_WaitForLastTask();
	  }
}


/*
 * 函数名：SDIO_IRQHandler
 * 描述  ：在SDIO_ITConfig(）这个函数开启了sdio中断	，
 			数据传输结束时产生中断
 * 输入  ：无		 
 * 输出  ：无
 */
void SDIO_IRQHandler(void) 
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB低优先级中断请求
 * 输入  ：无		 
 * 输出  ：无
 */
#if 0
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}
#endif
/*
 * 函数名：USB_HP_CAN1_TX_IRQHandler
 * 描述  ：USB高优先级中断请求 或发送中断
 * 输入  ：无		 
 * 输出  ：无
 */
void USB_HP_CAN1_TX_IRQHandler(void)
{
  CTR_HP();
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
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
