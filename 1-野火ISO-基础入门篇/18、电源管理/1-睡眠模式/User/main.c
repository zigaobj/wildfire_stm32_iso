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
#include "bsp_usart1.h"


static void Delay(__IO u32 nCount);


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
int main(void)
{	
	/* 配置 led */
	LED_GPIO_Config();
	
	/* 配置exti中断 */
	EXTI_Key_Config(); 
	
	/* 配置串口为中断模式 */
	USART1_Config();
	
	printf("\r\n 野火ISO开发板，睡眠实验 \r\n");

	
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
		
		__WFI();					//进入睡眠模式，等待中断唤醒
	}
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}



/*********************************************END OF FILE**********************/
