/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶触摸画板实验
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
#include "bsp_SysTick.h"
#include "bsp_touch.h"

extern volatile unsigned char touch_flag;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 系统定时器 1us 定时初始化 */
  SysTick_Init();
  
  LCD_Init();	
  /* GRAM扫描方向为左下脚->右上角 */
  Lcd_GramScan(2);
  LCD_Clear(0, 0, 320, 240, BACKGROUND);
  
/*------------------------------------------------------------------------------------------------------*/
  
  /* 触摸屏IO和中断初始化 */
  Touch_Init();
  
  /* 等待触摸屏校正完毕 */
  while(Touch_Calibrate() !=0);
  
  /* 触摸取色板初始化 */
  Palette_Init();
  
	while( 1 )
  {
    if(touch_flag == 1)			/*如果触笔按下了*/
    {
      /*获取点的坐标*/
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {					
        Palette_draw_point(display.x,display.y);	 										
      }
    }		
  }
}

/* ------------------------------------------end of file---------------------------------------- */

