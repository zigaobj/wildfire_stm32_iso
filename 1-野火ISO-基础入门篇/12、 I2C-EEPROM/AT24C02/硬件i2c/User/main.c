/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I2C EEPROM(AT24C02)测试，测试信息通过USART1打印在电脑的超级终端
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
#include "bsP_i2c_ee.h"
#include "bsP_led.h"
#include <string.h>

#define  EEP_Firstpage      0x00
u8 I2c_Buf_Write[256];
u8 I2c_Buf_Read[256];
void I2C_Test(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{ 
  
  /* 串口1初始化 */
	USART1_Config();
	
	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");
	//LED_GPIO_Config();

	/* I2C 外设初(AT24C02)始化 */
	I2C_EE_Init();

	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");	
	 	 
	I2C_Test();
  
  while (1)
  {      
  }
}

/**
  * @brief  I2C(AT24C02)读写测试
  * @param  无
  * @retval 无
  */
void I2C_Test(void)
{
	u16 i;

	printf("写入的数据\n\r");
    
	for ( i=0; i<=255; i++ ) //填充缓冲
  {   
    I2c_Buf_Write[i] = i;

    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n\r");    
   }

  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
   //LED1(ON);
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);
	//LED1(OFF);   
  
  printf("\n\r写成功\n\r");
   
   printf("\n\r读出的数据\n\r");
  //将EEPROM读出数据顺序保持到I2c_Buf_Read中
    //LED2(ON);   
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); 
   //LED2(OFF);
   
  //将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			printf("错误:I2C EEPROM写入与读出的数据不一致\n\r");
			return;
		}
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n\r");
    
	}
  printf("I2C(AT24C02)读写测试成功\n\r");
}
/*********************************************END OF FILE**********************/
