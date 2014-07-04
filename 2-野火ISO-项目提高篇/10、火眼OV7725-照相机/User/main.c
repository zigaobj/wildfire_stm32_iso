/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   火眼ov7725照相机实验
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
#include "bsp_ov7725.h"
#include "bsp_exti.h"
#include "bsp_led.h"

/*
 * 全局中断优先级的高低：按键 > sd卡 > 摄像头场中断   
 */

extern volatile uint8_t Ov7725_vsync ;

volatile uint8_t screen_flag;

/*
 * 要想中文显示成功
 * 需把字库拷贝到sd卡上，然后把sd卡插到开发板的卡槽上
 */
int main(void)
{
    uint8_t file_name[20];
    uint8_t num = 0;
/*--------------------------------------------------------------------------------------------------------*/		
	  /* 按键初始化 */
		EXTI_PC13_Config();
	
		/* LED 初始化 */
		LED_GPIO_Config();
		LED1_OFF;
		LED2_OFF;
/*--------------------------------------------------------------------------------------------------------*/		
	 /* 液晶初始化 */
		LCD_Init();
	  
		/* 设置液晶扫描方向为 左下角->右上角 */
		Lcd_GramScan( 2 );
		LCD_Clear(0, 0, 320, 240, BACKGROUND);
	
		/* 初始化sd卡文件系统，因为汉字的字库和bmp图片放在了sd卡里面 */
		Sd_fs_init();
/*--------------------------------------------------------------------------------------------------------*/	
		/* ov7725 gpio 初始化 */
		Ov7725_GPIO_Config();
	
		/* ov7725 寄存器配置初始化 */
		while(Ov7725_Init() != SUCCESS);
	
		/* ov7725 场信号线初始化 */
		VSYNC_Init();	
		Ov7725_vsync = 0;
/*-------------------------------------------------------------------------------------------------------*/	
		while(1)
		{
        if( Ov7725_vsync == 2 )
        {
            FIFO_PREPARE;  			/*FIFO准备*/					
            ImagDisp();					/*采集并显示*/
            Ov7725_vsync = 0;			
        }        
        /* screen_flag 在按键中断函数里面置位 */
        if( screen_flag == 1 )
        {                    
            sprintf((char *)&file_name,"/camera%d",num++);           
                    
            /* 设置液晶扫描方向为 右下角->左上角 */
            Lcd_GramScan( 3 );
            Screen_shot(0,0,320,240,file_name);
            
            /* 截图完毕LED2灭 */
            LED2_ON;
            screen_flag = 0;
        }
		}
}

/* ---------------------------------------------end of file----------------------------------------------*/


	  

