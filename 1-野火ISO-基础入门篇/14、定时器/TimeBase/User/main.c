/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   tim2 timebase(ms级别) test
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
#include "bsp_led.h"
#include "bsp_TiMbase.h"

volatile u32 time = 0; // ms 计时变量 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	/* led 端口配置 */ 
	LED_GPIO_Config();

	/* TIM2 定时配置 */	
  TIM2_Configuration();
	
	/* 实战定时器的中断优先级 */
	TIM2_NVIC_Configuration();

	/* TIM2 重新开时钟，开始计时 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	
  while(1)
  {
    if ( time == 1000 ) /* 1000 * 1 ms = 1s 时间到 */
    {
      time = 0;
			/* LED1 取反 */      
			LED1_TOGGLE; 
    }        
  }
}
/*********************************************END OF FILE**********************/
