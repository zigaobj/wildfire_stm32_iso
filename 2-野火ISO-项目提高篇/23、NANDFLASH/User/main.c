/**
  ******************************************************************************
  * @file    main.c
  * @author  armfly
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   NANDFLASH 测试
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
#include "bsp_nand.h"
#include "bsp_led.h"

void NAND_Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
/*
 * PG9 接到了 NORFLASH 的 CE 端，为了不让 NORFLASH的信号 影响到 NANDFLASH
 * 这里先让 CE = 1 ,即 PG9 = 1 ,不然 SRAM 会测试不成功.
 * 或者在硬件设计时，NORFLASH 的 CE 管脚 上拉下.
 */
  GPIO_InitTypeDef GPIO_InitStructure; 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_SetBits(GPIOG,GPIO_Pin_9);
  
  LED_GPIO_Config();	
  
	/* USART1 config 115200 8-N-1 */
	USART1_Config();
  printf("\r\n 初始化时间有点长，请耐心等待...... \r\n");
	NAND_Init();
  
  /* ID = ADF1801D */	
  printf("\r\n NANDFLASH id 为 %x \r\n", NAND_ReadID());
  printf("\r\n NANDFLASH Test finished \r\n");
  
	for(;;)
	{
    LED2_TOGGLE;
    NAND_Delay(0x0FFFFF);
	}
}
/*********************************************END OF FILE**********************/
