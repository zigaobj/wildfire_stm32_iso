/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc 测试，显示时间格式为: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_rtc.h"

// N = 2^32/365/24/60/60 = 136 年

/*时间结构体*/
struct rtc_time systmtime;


/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int main()
{			
	  	/* 配置RTC秒中断优先级 */
	  RTC_NVIC_Config();
	
	  USART1_Config();
	
	  RTC_CheckAndConfig(&systmtime);
	
	  /* Display time in infinite loop */
	  Time_Show(&systmtime);
}

/***********************************END OF FILE*********************************/

