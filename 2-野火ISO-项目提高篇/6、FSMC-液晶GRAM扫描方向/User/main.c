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
	
  /* 通过修改GRAM的扫描方向，然后通过看液晶清屏的色块，就可以知道GRAM的实际扫描方向了 */
  Lcd_GramScan(2);
  LCD_Clear(0, 0, 320, 240, BACKGROUND);
  LCD_DispStr(10, 110, (uint8_t *)"LCD Gram test123456456789456123456789......", RED);
  
	LCD_Clear(0, 0, 100, 10, RED);
  //LCD_Clear(30, 10, 1, 20, RED);
//	LCD_DispChar(60, 60, 'A', RED);
//	
//	LCD_DispStr(10, 10, (uint8_t *)"This is a lcd demo to display ascii", RED);	
//	
//	LCD_DispStr(40, 100, (uint8_t *)"count:", RED);
//	
//	for( n=0; n<500000; n++ )
//	{
//		LCD_DisNum(100, 100, n, RED);
//		Lcd_Delay(0xAFFf>>4);
//	}
	
	while( 1 ){}
}


