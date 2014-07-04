/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   AT24C02测试（软件模拟I2C）
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
#include "bsp_i2c_ee.h"

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		/* USART1 config 115200 8-N-1 */
		USART1_Config();

		printf("eeprom 软件模拟i2c测试例程 \r\n");		
  
    ee_Test();
  
		for(;;)		{
			
		}
}
/*********************************************END OF FILE**********************/
