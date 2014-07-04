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
#include "bsp_lcd_st7735s.h"
#include "FIRE_lcd.h"
#include "FIRE_key_event.h"
#include "bsp_SysTick.h"

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	Site_t site= {0,0};
	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
	
	/* LCD初始化 */
	LCD_init();

	SysTick_Init();
	
	site.x=20;site.y=20;
	LCD_str(site,"LCD test OK",BLUE,RED);

	key_event_init();

	/* 方法2，使用固件库控制IO */
	while (1)
	{	
		deal_key_event();
	}

}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
