/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   获取芯片的96bitID和FLASH大小
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
#include "bsp_chipid.h"
#include "bsp_led.h" 

extern uint32_t ChipUniqueID[3];
void Delay(__IO uint32_t nCount);


 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	LED_GPIO_Config();
  
  /* USART1 config 115200 8-N-1 */
	USART1_Config();
/*------------------------------------------------------------------------------------------------------*/
	/* to get the chipid and put it in ChipUniqueID[3] */
	Get_ChipID();
  
	/* printf the chipid */
	printf("\r\n芯片的唯一ID为: %X-%X-%X\r\n",
	        ChipUniqueID[0],ChipUniqueID[1],ChipUniqueID[2]);
	
	/* printf the flash memory amount */
	printf("\r\n芯片flash的容量为: %dK \r\n", *(__IO u16 *)(0X1FFFF7E0));
/*------------------------------------------------------------------------------------------------------*/	
	
	for(;;)
	{
		LED1_TOGGLE;
    Delay(0x0FFFFF);
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/
