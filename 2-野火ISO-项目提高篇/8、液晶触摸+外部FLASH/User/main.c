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
#include "bsp_SysTick.h"
#include "bsp_touch.h"
#include "bsp_spi_flash.h"
#include "bsp_led.h" 
#include "bsp_usart1.h"


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	uint8_t k;
  
  /* 系统定时器 1us 定时初始化 */
  SysTick_Init();
 
  LED_GPIO_Config();
  
  LCD_Init();	
  /* GRAM扫描方向为左下脚->右上角 */
  Lcd_GramScan(2);
  LCD_Clear(0, 0, 320, 240, BACKGROUND);
  
  USART1_Config();
  
  SPI_FLASH_Init();
  //SPI_FLASH_SectorErase(0);
  
  /* 触摸屏IO和中断初始化 */
  Touch_Init();
/*-------------------------------------------------------------------------------------------------------*/  
  #if 1
  SPI_FLASH_BufferRead(&cal_flag, 0, 1);
  if( cal_flag == 0x55 )
  {
    SPI_FLASH_BufferRead((void*)cal_p, 1, sizeof(cal_p));
    SPI_FLASH_CS_HIGH();
    for( k=0; k<6; k++ )
          printf("\r\n rx = %LF \r\n",cal_p[k]);
  }
  else
  {
    /* 等待触摸屏校正完毕 */
    while(Touch_Calibrate() !=0);
  }
  #elif 0
  /* 等待触摸屏校正完毕 */
  while(Touch_Calibrate() !=0);
  #endif
/*------------------------------------------------------------------------------------------------------*/  
  //Touch_Init();
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
        LED1_TOGGLE;       
      }
    }		
  }
}

/* ------------------------------------------end of file---------------------------------------- */

