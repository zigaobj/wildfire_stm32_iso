/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   systick系统定时器
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
#include "bsp_SysTick.h"
#include "bsp_led.h"

// 挨踢员

/*
 * t : 定时时间 
 * Ticks : 多少个时钟周期产生一次中断 
 * f : 时钟频率 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();

	/* 配置SysTick 为10us中断一次 */
	SysTick_Init();

	for(;;)
	{

		LED1( ON ); 
	    Delay_us(10000);    	// 10000 * 10us = 100ms
		//Delay_ms(100);
		LED1( OFF );
	  
		LED2( ON );
	    Delay_us(10000);		// 10000 * 10us = 100ms
		//Delay_ms(100);
		LED2( OFF );
	
		LED3( ON );
	    Delay_us(10000);		// 10000 * 10us = 100ms
		//Delay_ms(100);
		LED3( OFF );	

	}     
}
/*********************************************END OF FILE**********************/
