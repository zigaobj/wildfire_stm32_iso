/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   sram(IS62WV51216BLL)测试
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
#include "bsp_fsmc_sram.h"

/* 
 * 2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
 * 64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
 * 64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
 * 64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
 * 64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF
*/

/*
 * SRAM 型号: IS62WV51216BLL
 *      容量: 8Mbit = 1024 KB
 * 访问方式 : 16bit 操作
 * 地址     : 0X6800 0000 ~ 0X6800 0000 + 8Mbit
 */

#if 1


void SRAM_Delay(__IO u32 nCount);

#define BUFFER_SIZE        0x400      /* 1024个字节 */
//#define BUFFER_SIZE        0x40      /* 1024个字节 */
#define WRITE_READ_ADDR    0x8000


uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE];
uint32_t WriteReadStatus = 0, Index = 0;

void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);

/**
  * @brief  Main program.
  * @param  None
  * @retval : None
  */
int main(void)
{
  /* LED GPIO 初始化 */
	LED_GPIO_Config();
	
	LED2(OFF);
	LED3(OFF);
  
  /* 配置 FSMC Bank1 NOR/SRAM3 */
  FSMC_SRAM_Init();
  //LED1(ON);
  
  /* 填充要发送的缓冲区 */
  Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x3212);
  
  /* 将缓冲区的数据写到SRAM里面 */
  FSMC_SRAM_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
  
  SRAM_Delay(0x0FFFFF);
  
  /* 将刚刚写到SRAM里面的数据读出来 */
  FSMC_SRAM_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);  

  /* 将写到SRM里面的数据和从SRAM里面的数据读出来比对 */   
  for (Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
  {
    if (RxBuffer[Index] != TxBuffer[Index])
    {
      WriteReadStatus = Index + 1;
    }
  }	

  if (WriteReadStatus == 0)
  {	/* OK */
    /* Turn on LD2 */
    while(1)
    {
      LED2_TOGGLE;
      SRAM_Delay(0x0FFFFF);
    }    
  }                               
  else  
  { /* KO */		
    while(1)
    {
      LED3_TOGGLE;
      SRAM_Delay(0x0FFFFF);
    }
  }
}

/*
 * 填充缓冲区
 */
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }
}

void SRAM_Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

#endif
/* ------------------------------------------------end of file-------------------------------------------- */

