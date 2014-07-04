/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
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
#include "bsp_led.h"

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	 
 
	/* 方法3，使用寄存器控制IO */ 
	while(1)
	{

		LED1_TOGGLE;	 			//led1状态反转，亮	
    Delay(0x0FFFFF); 
		LED1_TOGGLE;				//led1状态反转，灭

		LED2_TOGGLE;				//led2状态反转，亮
		Delay(0x0FFFFF);	   
		LED2_TOGGLE;		  	//led2状态反转，灭

		LED3_TOGGLE;				//led3状态反转，亮
		Delay(0x0FFFFF);
		LED3_TOGGLE;				//led3状态反转，灭
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
