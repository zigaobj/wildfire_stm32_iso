/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试tim3四个通道输出不同占空比的pwm波
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
#include "bsp_pwm_output.h"

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	
	/* TIM3 PWM波输出初始化，并使能TIM3 PWM输出 */
	TIM3_PWM_Init();
	
	while (1)
	{

	}
}
/*********************************************END OF FILE**********************/
