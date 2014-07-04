/**
  ******************************************************************************
  * @file FSMC/SRAM/fsmc_sram.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  This file provides a set of functions needed to drive the 
  *         IS61WV51216BLL SRAM memory mounted on STM3210E-EVAL board.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "bsp_fsmc_sram.h"
#include "bsp_led.h" 
#include <stdio.h>

/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup FSMC_SRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 64MB =0X0400 0000
// BANK1 0X6000 0000 ~ 0X63FF FFFF
// BANK2 0X6400 0000 ~ 0X67FF FFFF
// BANK3 0X6800 0000 ~ 0X6CFF FFFF
// BANK4 0X6C00 0000 ~ 0X6FFF FFFF
// #define Bank1_SRAM1_ADDR    ((uint32_t)0x60000000)
// #define Bank1_SRAM2_ADDR    ((uint32_t)0x64000000)
#define Bank1_SRAM3_ADDR    ((uint32_t)0x68000000)
// #define Bank1_SRAM4_ADDR    ((uint32_t)0x6C000000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None 
  * @retval : None
  */
void FSMC_SRAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF, ENABLE);
  
/*-- GPIO Configuration ------------------------------------------------------*/
  /* SRAM Data lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* SRAM Address lines configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | 
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  /* NOE and NWE configuration */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* NE3 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  
  /* NBL0, NBL1 configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
  GPIO_Init(GPIOE, &GPIO_InitStructure);

/*
 * PG9 接到了 NORFLASH 的 CE 端，为了不让 NORFLASH的信号 影响到 SRAM
 * 这里先让 CE = 1 ,即 PG9 = 1 ,不然 SRAM 会测试不成功.
 * 或者在硬件设计时，NORFLASH 的 CE 管脚 上拉下.
 */
  //GPIO_InitTypeDef GPIO_InitStructure; 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_SetBits(GPIOG,GPIO_Pin_9);
  
/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  
}

/**
  * @brief  Writes a Half-word buffer to the FSMC SRAM memory. 
  * @param pBuffer : pointer to buffer. 
  * @param WriteAddr : SRAM memory internal address from which the data 
  *        will be written.
  * @param NumHalfwordToWrite : number of half-words to write. 
  * @retval : None
  */
void FSMC_SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  for(; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(uint16_t *) (Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer++;
    
    /* Increment the address*/  
    WriteAddr += 2;
  }   
}

/**
  * @brief  Reads a block of data from the FSMC SRAM memory.
  * @param pBuffer : pointer to the buffer that receives the data read 
  *        from the SRAM memory.
  * @param ReadAddr : SRAM memory internal address to read from.
  * @param NumHalfwordToRead : number of half-words to read.
  * @retval : None
  */
void FSMC_SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  for(; NumHalfwordToRead != 0; NumHalfwordToRead--) /* while there is data to read */
  {
    /* Read a half-word from the memory */
    *pBuffer++ = *(__IO uint16_t*) (Bank1_SRAM3_ADDR + ReadAddr);

    /* Increment the address*/  
    ReadAddr += 2;
  }  
}

/*-----------------------------------------------------------------------------*/
#define BUFFER_SIZE        0x400      /* 1024个字节 */
#define WRITE_READ_ADDR    0x8000

static uint16_t TxBuffer[BUFFER_SIZE];
static uint16_t RxBuffer[BUFFER_SIZE];
static uint32_t WriteReadStatus = 0, Index = 0;

static void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
  uint16_t IndexTmp = 0;

  /* Put in global buffer same values */
  for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }
}

static void SRAM_Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}


void SRAM_Test(void)
{
  /* LED GPIO 初始化 */
	LED_GPIO_Config();
	
	LED2(OFF);
	LED3(OFF);
  
  //SRAM_Delay(0xFFFFFF);
  
  /* 配置 FSMC Bank1 NOR/SRAM3 */
  FSMC_SRAM_Init();

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
    printf("SRAM 测试成功\r\n");
    while(1)
    {
      LED2_TOGGLE;
      SRAM_Delay(0x0FFFFF);
    }    
  }                               
  else
  { /* KO */
    printf("SRAM 测试失败\r\n");		
    while(1)
    {
      LED3_TOGGLE;
      SRAM_Delay(0x0FFFFF);
    }
  }
}


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
