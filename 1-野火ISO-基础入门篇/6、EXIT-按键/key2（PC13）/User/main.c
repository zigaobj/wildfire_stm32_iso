/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   按键测试（中断模式/EXTI模式）
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
#include "bsp_exti.h" 

#define CLI()      __set_PRIMASK(1)		/* 关闭总中断 */  
#define SEI() __set_PRIMASK(0)				/* 开放总中断 */ 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
int main(void)
{	
	/* config the led */
	LED_GPIO_Config();
	LED1_ON;
	
	CLI();
	SEI();
	
	/* exti line config */
	EXTI_PC13_Config(); 
	
	/* wait interrupt */
	while(1)                            
	{
	}
}
/*********************************************END OF FILE**********************/
