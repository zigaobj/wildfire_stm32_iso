/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：can测试实验(中断模式和回环)，并将测试信息通过USART1在超级终端中打印出来。         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "led.h"
#include "can.h" 

/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */

int main(void)
{	
	/* USART1 config */
	USART1_Config();
	
	/* LED config */
	LED_GPIO_Config();	
	
	printf( "\r\n 这个一个CAN（回环模式和中断模式）测试程序...... \r\n" );
	
	USER_CAN_Init();	
	printf( "\r\n CAN 回环测试初始化成功...... \r\n" );
	
	USER_CAN_Test();	
	printf( "\r\n CAN 回环测试成功...... \r\n" );
	
	
	while (1)
	{
	
	}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
