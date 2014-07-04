/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   待机唤醒实验
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
#include "bsp_usart1.h"

static void Delay(__IO u32 nCount);


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
int main(void)
{	
	/* config the led */
	LED_GPIO_Config();
	
	/* exti line config */
	EXTI_Key_Config(); 
	
	USART1_Config();	
	
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
	{
		printf("\r\n 使能电源管理时钟单元前的检测，待机唤醒复位 \r\n");

	}
	else
		printf("\r\n 使能电源管理时钟单元前的检测，上电复位 \r\n");
	
	
	/* 使能电源管理单元的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
	
	
	if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
	{
		printf("\r\n 使能后检测，待机唤醒复位 \r\n");

	}
	else
		printf("\r\n 使能后检测，上电复位 \r\n");
	
	
	while(1)                            
	{	
		/* wait interrupt */
		LED1( ON );			  // 亮
		Delay(0xFFFFF);
		LED1( OFF );		  // 灭

		LED2( ON );			  // 亮
		Delay(0xFFFFF);
		LED2( OFF );		  // 灭

		LED3( ON );			  // 亮
		Delay(0xFFFFF);
		LED3( OFF );		  // 灭	
	
	}
}


static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


/*********************************************END OF FILE**********************/
