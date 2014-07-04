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
#include "bsp_usart1.h"
#include "bsp_ds18b20.h"

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 配置SysTick 为1us中断一次 */
	SysTick_Init();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	USART1_Config();	
	printf("\r\n this is a ds18b20 test demo \r\n");
	
	while( DS18B20_Init())	
	{
		printf("\r\n no ds18b20 exit \r\n");
	}	
	printf("\r\n ds18b20 exit \r\n");
	
	for(;;)
	{	
		printf("\r\n temperature %.1f\r\n",DS18B20_Get_Temp());				 
		Delay_ms(1000);		/* 1s 读取一次温度值 */
		LED1_TOGGLE;
	}    
}
/*********************************************END OF FILE**********************/
