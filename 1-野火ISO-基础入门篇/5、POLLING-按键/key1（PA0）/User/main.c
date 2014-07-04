/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试按键
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
#include "bsp_key.h" 

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

	/*config key*/
	Key_GPIO_Config();	
	
	while(1)                            
	{	   
		if( Key_Scan(GPIOA,GPIO_Pin_0) == KEY_ON  )
		{
			/*LED1反转*/
			LED1_TOGGLE;
		}   
	}
}
/*********************************************END OF FILE**********************/
