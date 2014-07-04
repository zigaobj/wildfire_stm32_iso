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
		LCD_Clear(0, 0, 240, 320, BACKGROUND);		
		
		/* 初始化sd卡文件系统，因为汉字的字库放在了sd卡里面 */
		Sd_fs_init();		
		LCD_DispStr(5, 5, (uint8_t *)"fatfs init success", RED);
		LCD_DispStr(5, 25, (uint8_t *)"insure the sdcard has insert......", RED);  
    
	  LCD_DispStrCH(50, 50, (uint8_t *)"野火开发板", RED);   
	  LCD_DispEnCh(30, 90, "Welcome to use 野火 ISO Stm32 开发板 bigo------", RED);
	
		while(1);
}

	  

