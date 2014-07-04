/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   nordflash读写测试
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
#include "bsp_fsmc_nor.h"

/* 2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
 * 64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
 * 64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
 * 64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
 * 64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF
*/

/*
 * NOR 型号: S29GL128P
 *      容量: 16MB ,每个扇区128K字节，一共128个扇区
 * 访问方式 : 16bit 操作
 * 地址     : 0x6400 0000 - 0x64FF FFFF
 */

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	/* 配置串口为 115200 8-N-1 */
	USART1_Config();  

  NORFLASH_Test();
  
  while(1);
}
/*********************************************END OF FILE**********************/
