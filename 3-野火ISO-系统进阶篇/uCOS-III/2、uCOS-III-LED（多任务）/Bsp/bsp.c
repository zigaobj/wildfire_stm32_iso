#include "bsp.h"

/*
 * 函数名：BSP_Init
 * 描述  ：时钟初始化、硬件初始化
 * 输入  ：无
 * 输出  ：无
 */
void BSP_Init(void)
{
  SysTick_init();
  
  /* LED 端口初始化 */
  LED_GPIO_Config();  
}

/*
 * 函数名：SysTick_init
 * 描述  ：配置SysTick定时器
 * 输入  ：无
 * 输出  ：无
 */
void SysTick_init(void)
{
  /* 初始化并使能SysTick定时器 */
  SysTick_Config(SystemCoreClock/OS_CFG_TICK_RATE_HZ);
  
  /*  配置1ms 中断一次，即os的频率为1000hz */
	if (SysTick_Config(SystemCoreClock/OS_CFG_TICK_RATE_HZ))	
	{ 
		/* Capture error */ 
		while (1);
	}
}


/* --------------------------------------end of file--------------------------------------- */
