/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶显示中文
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

/*
 * 要想中文显示成功
 * 需把字库拷贝到sd卡上，然后把sd卡插到开发板的卡槽上
 */
int main(void)
{
   /* USART1 config */
		//USART1_Config();
		//printf("\r\n this is a fatfs test demo \r\n");
	
		LCD_Init();
	
		/* 设置Lcd Gram 扫描方向为: 右下角->左上角 */		
		Lcd_GramScan( 3 );	
		LCD_Clear(0, 0, 320, 240, BACKGROUND);	
		
		/* 初始化sd卡文件系统，因为汉字的字库和bmp图片放在了sd卡里面 */
		Sd_fs_init();
	
		/* 显示sd卡里面的bmp图片 */
		Lcd_show_bmp(0, 0,"/wildfire.bmp");
		Lcd_show_bmp( 10,10,"/pic1.bmp");		
    Lcd_show_bmp( 170,0,"/pic2.bmp"); 	
	
		Screen_shot(0,0,320,240,"/myscreen");
		
		while(1);
}

	  

