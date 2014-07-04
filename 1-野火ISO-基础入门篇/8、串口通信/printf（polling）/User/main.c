/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   USART printf example
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

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		/* USART1 config 115200 8-N-1 */
		USART1_Config();

		printf("\r\n this is a usart printf demo \r\n");

		printf("\r\n 欢迎使用野火 iSO STM32 开发板 \r\n");		

		for(;;)
		{
			
		}
}
/*********************************************END OF FILE**********************/
