/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   停止模式唤醒实验
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
#include "bsp_exti.h" 
#include "bsp_usart1.h"
#include "bsp_SysTick.h"


void SYSCLKConfig_STOP(void);



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */ 
int main(void)
{	
	/* 配置 led */
	LED_GPIO_Config();

	/* 配置外部中断引脚 */
	EXTI_Key_Config(); 
	
	/* 配置串口 */
	USART1_Config();
	
  /* 配置SysTick 为10us中断一次 */
	SysTick_Init();
	
	printf("\r\n 野火ISO开发板，停止模式实验 \r\n");	

	
	/* 使能电源管理单元的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	printf("\r\n 进入停止模式 \r\n");

	/* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒*/
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
	
	while(1)                            
	{	
		
		/* wait interrupt */
		LED1( ON );			  // 亮
		Delay_us(10000);    	// 10000 * 10us = 100ms
		LED1( OFF );		  // 灭

		LED2( ON );			  // 亮
		Delay_us(10000);   	// 10000 * 10us = 100ms
		LED2( OFF );		  // 灭

		LED3( ON );			  // 亮
		Delay_us(10000);    	// 10000 * 10us = 100ms
		LED3( OFF );		  // 灭	  		
		
	}
}

/**
  * @brief  停机唤醒后配置系统时钟: 使能 HSE, PLL
  *         并且选择PLL作为系统时钟.
  * @param  None
  * @retval None
  */
void SYSCLKConfig_STOP(void)
{
	ErrorStatus HSEStartUpStatus;
  /* 使能 HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* 等待 HSE 准备就绪 */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {

    /* 使能 PLL */ 
    RCC_PLLCmd(ENABLE);

    /* 等待 PLL 准备就绪 */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* 选择PLL作为系统时钟源 */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* 等待PLL被选择为系统时钟源 */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}
/*********************************************END OF FILE**********************/
