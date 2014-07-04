/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
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

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 

	/* 方法1，直接控制寄存器 */
	while(1)
	{
		// ODR GPIOB
		// 15 14 13 12     11 10 9 8    7 6 5 4    3 2 1 0
		// 0  0  0  0      0  0  0 0    0 0 0 0    0 0 0 0 (复位值)
		// 1  1  1  1      1  1  1 1    1 1 1 1    1 1 1 0

		GPIOB->ODR = 0XFFFE; 		//低电平，GPIOB0（LED0）灯亮
		Delay(0x0FFFFF);
		GPIOB->ODR = 0XFFFF;		//高电平，GPIOB0（LED0）灯灭

		// ODR GPIOF
		// 15 14 13 12     11 10 9 8    7 6 5 4    3 2 1 0
		// 0  0  0  0      0  0  0 0    0 0 0 0    0 0 0 0 (复位值)
		// 1  1  1  1      1  1  1 1    0 1 1 1    1 1 1 1	
		GPIOF->ODR = 0XFF7F; 		//低电平，GPIOF7（LED1）灯亮
		Delay(0x0FFFFF);
		GPIOF->ODR = 0XFFFF;		//高电平，GPIOF7（LED1）灯亮

		// ODR GPIOF
		// 15 14 13 12     11 10 9 8    7 6 5 4    3 2 1 0
		// 0  0  0  0      0  0  0 0    0 0 0 0    0 0 0 0 (复位值)
		// 1  1  1  1      1  1  1 0    1 1 1 1    1 1 1 1		 	
		GPIOF->ODR = 0XFEFF; 		//低电平，GPIOF8（LED2）灯亮
		Delay(0x0FFFFF);
		GPIOF->ODR = 0XFFFF;		//高电平，GPIOF8（LED2）灯亮 
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
