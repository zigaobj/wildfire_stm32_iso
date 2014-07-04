/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试lcd显示英文字符
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
#include "bsp_ili9341_lcd.h"
#include "bsp_usart1.h"

extern void Lcd_Delay(__IO uint32_t nCount);

uint32_t n;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{  
  LCD_Init();	
  
	LCD_Clear(0, 0, 240, 320, BACKGROUND);	
  
	LCD_DispChar(60, 60, 'A', RED);

  
	LCD_DispStr(10, 10, (uint8_t *)"This is a lcd demo to display ascii", RED);	
	
	LCD_DispStr(40, 100, (uint8_t *)"count:", RED);
	
	for( n=0; n<500000; n++ )
	{
		LCD_DisNum(100, 100, n, RED);
		Lcd_Delay(0xAFFf>>4);
	}
	
	while( 1 ){}
}


