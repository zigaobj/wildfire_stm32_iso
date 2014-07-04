/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶触摸控制LED亮灭
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
#include "bsp_ili9341_lcd.h"
#include "bsp_sdfs_app.h"
#include "bsp_bmp.h"
#include "bsp_SysTick.h"
#include "bsp_touch.h"
#include "bsp_spi_flash.h"
#include "bsp_led.h" 
#include "even_process.h"




/*
三个坐标系统的转换：


触摸屏：
				column 240
			 
			 x
        _ _ _ _ _ _
       ^           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
	(0,0) - - - - - ->  y       
				
液晶显示字体及清屏等函数：
				
				column 240
			 
										x
  (0,0) ----------->
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 V- - - - - - 

       y
			 
液晶显示图像：
				
				column 240		 
									 X	
				-----------A
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |  320        
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
			 |           |
		 Y <- - - - - -(0,0) 

       

转换关系：
Y字 = 320 - X触摸
X字 = Y触摸

X图像 = X触摸
Y图像 = 240-Y触摸

Y图像 = 240 - X字
X图像 = 320 - Y字


*/




/**
  * @brief  LCD触摸控制及显示例程，
	*					示范如何使用触摸进行人机交互以及使用LCD显示变量
  * @param  无
  * @retval 无
 */
int main(void)
{
		uint8_t k;
	
		/* 初始化LCD */
		LCD_Init();	
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();	
	
		/* 初始化串口 */
	  USART1_Config();
	
		/* 系统定时器 1us 定时初始化 */
	 	SysTick_Init();				
	 
		/* 初始化LED */
		LED_GPIO_Config();	
	
		/* 初始化外部FLASH */
		SPI_FLASH_Init();
		//SPI_FLASH_SectorErase(0);
  
		/* 触摸屏IO和中断初始化 */
		Touch_Init();			
		
		/*-------------------------------------------------------------------------------------------------------*/  
		#if 1
		SPI_FLASH_BufferRead(&cal_flag, 0, 1);
		if( cal_flag == 0x45 )
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
	
		/* 显示LED控制界面图片 */
		Lcd_show_bmp(0, 0,"/led/ui_window.bmp");

		while(1)
		{
			even_process();
		}

}


/******************* end of file**************************/

