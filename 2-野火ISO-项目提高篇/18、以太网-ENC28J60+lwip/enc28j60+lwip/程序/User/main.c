/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   enc28j60+lwip测试，具体测试步骤参考工程里面的文件readme.txt
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"

#include "err.h"
#include "etharp.h"
	
#include "netconfig.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h" 
#include "spi.h"
#include "bsp_led.h"

#include "cmd.h"
#include "httpd.h"

/*****************************************************************************************************************************/
    
__IO uint32_t LocalTime = 0; 
/* this variable is used to create a time reference incremented by 10ms */ 
	
/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  : 无
 * 调用  ：
 */
int main(void)
{ 
	/*初始化串口*/
  USART1_Config();  	

	/*初始化 以太网SPI接口*/
	ENC_SPI_Init(); 		

	/*初始化systick，用于定时轮询输入或给LWIP提供定时*/
	SysTick_Init();		

	/*初始化LED使用的端口*/
	LED_GPIO_Config(); 
	 
	printf("\r\n**************野火STM32_enc8j60+lwip移植实验*************\r\n");
	  
  	/* 初始化LWIP协议栈*/
	LwIP_Init(); 

	/*初始化web server 显示网页程序*/
	httpd_init();
  
  /* 初始化telnet   远程控制 程序 */   
  CMD_init();                                       

  /* Infinite loop */
  while ( 1 )
	{	
		/*轮询*/  
		LwIP_Periodic_Handle(LocalTime);		          	  
  }
}






