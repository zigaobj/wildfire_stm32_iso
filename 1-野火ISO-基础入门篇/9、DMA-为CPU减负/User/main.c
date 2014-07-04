/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart1 DMA TX 测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */  
 
// DMA  data memory access 

#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_led.h"

extern uint8_t SendBuff[SENDBUFF_SIZE];
static void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		/* USART1 config 115200 8-N-1 */
		USART1_Config();
	
		USART1_DMA_Config();
	
		LED_GPIO_Config();
	
		printf("\r\n usart1 DMA TX 测试 \r\n");
	
		{
			uint16_t i;
			
			/*填充将要发送的数据*/
			for(i=0;i<SENDBUFF_SIZE;i++)
			{
				SendBuff[i]	 = 'A';
			}
		}
		
		/* USART1 向 DMA发出TX请求 */
		USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

		/* 此时CPU是空闲的，可以干其他的事情 */
		
		//例如同时控制LED
		for(;;)
		{
			LED1(ON);
			Delay(0xFFFFF);
			LED1(OFF);
			Delay(0xFFFFF);
		}
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
