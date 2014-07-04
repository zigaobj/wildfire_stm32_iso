/**
  ******************************************************************************
  * @file    FSMC/NOR/fsmc_nor.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   This file provides a set of functions needed to drive the
  *          M29W128FL, M29W128GL and S29GL128P NOR memories mounted
  *          on STM3210E-EVAL board.
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
#include "bsp_fsmc_nor.h"
#include "bsp_led.h"
#include <stdio.h>
#include "app_ui.h"

#ifdef LCD_DISPLAY
	#include "bsp_ili9341_lcd.h"
#endif


/*
 * S29GL128P 内存组织结构： 每个扇区128K字节，一共128个扇区。总容量为 16M字节。按16Bit访问
 * 挂在STM32上，对应的物理地址范围为 ： 0x6400 0000 - 0x64FF FFFF.  只能按16Bit模式访问
 */

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup FSMC_NOR
  * @{
  */ 


/** @defgroup STM3210E_EVAL_FSMC_NOR_Private_Defines
  * @{
  */ 
/** 
  * @brief  FSMC Bank 1 NOR/SRAM2  
  */
#define Bank1_NOR2_ADDR       ((uint32_t)0x64000000)

/* Delay definition */   
#define BlockErase_Timeout    ((uint32_t)0x00A00000)
#define ChipErase_Timeout     ((uint32_t)0x30000000) 
#define Program_Timeout       ((uint32_t)0x00001400)     
/**
  * @}
  */ 


/** @defgroup STM3210E_EVAL_FSMC_NOR_Private_Macros
  * @{
  */
#define ADDR_SHIFT(A) (Bank1_NOR2_ADDR + (2 * (A)))
#define NOR_WRITE(Address, Data)  (*(__IO uint16_t *)(Address) = (Data))  
/**
  * @}
  */ 
  

/** @defgroup STM3210E_EVAL_FSMC_NOR_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroupSTM3210E_EVAL_FSMC_NOR_Private_Function_Prototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM3210E_EVAL_FSMC_NOR_Private_Functions
  * @{
  */

/**
  * @brief  Configures the FSMC and GPIOs to interface with the NOR memory.
  *         This function must be called before any write/read operation
  *         on the NOR.
  * @param  None
  * @retval None
  */
void FSMC_NOR_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure;
	
  //引脚配置
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

  /* 数据线配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
  /* 地址线配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* NOE 和 NWE 配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  

  /* NE2 配置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  
  /* PD6 作为忙信号, 配置为GPIO输入模式，软件查询 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /*-- FSMC Configuration ----------------------------------------------------*/
  p.FSMC_AddressSetupTime = 0x05;
  p.FSMC_AddressHoldTime = 0x00;
  p.FSMC_DataSetupTime = 0x09;
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
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

  /*!< Enable FSMC Bank1_NOR Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
}

/**
  * @brief  Reads NOR memory's Manufacturer and Device Code.
  * @param  NOR_ID: pointer to a NOR_IDTypeDef structure which will hold the 
  *         Manufacturer and Device Code.  
  * @retval None
  */
void FSMC_NOR_ReadID(NOR_IDTypeDef* NOR_ID)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0090);

  NOR_ID->Manufacturer_Code = *(__IO uint16_t *) ADDR_SHIFT(0x0000);
  NOR_ID->Device_Code1 = *(__IO uint16_t *) ADDR_SHIFT(0x0001);
  NOR_ID->Device_Code2 = *(__IO uint16_t *) ADDR_SHIFT(0x000E);
  NOR_ID->Device_Code3 = *(__IO uint16_t *) ADDR_SHIFT(0x000F);
}

/**
  * @brief  Erases the specified Nor memory block.
  * @param  BlockAddr: address of the block to erase.
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_EraseBlock(uint32_t BlockAddr)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE((Bank1_NOR2_ADDR + BlockAddr), 0x30);

  return (FSMC_NOR_GetStatus(BlockErase_Timeout));
}

/**
  * @brief  Erases the entire chip.
  * @param  None                      
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_EraseChip(void)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0080);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x0010);

  return (FSMC_NOR_GetStatus(ChipErase_Timeout));
}

/**
  * @brief  Writes a half-word to the NOR memory.
  * @param  WriteAddr: NOR memory internal address to write to.
  * @param  Data: Data to write. 
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00A0);
  NOR_WRITE((Bank1_NOR2_ADDR + WriteAddr), Data);

  return (FSMC_NOR_GetStatus(Program_Timeout));
}

/**
  * @brief  Writes a half-word buffer to the FSMC NOR memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  WriteAddr: NOR memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite: number of Half words to write. 
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  NOR_Status status = NOR_ONGOING; 

  do
  {
    /*!< Transfer data to the memory */
    status = FSMC_NOR_WriteHalfWord(WriteAddr, *pBuffer++);
    WriteAddr = WriteAddr + 2;
    NumHalfwordToWrite--;
  }
  while((status == NOR_SUCCESS) && (NumHalfwordToWrite != 0));
  
  return (status); 
}

/**
  * @brief  Writes a half-word buffer to the FSMC NOR memory. This function 
  *         must be used only with S29GL128P NOR memory.
  * @param  pBuffer: pointer to buffer. 
  * @param  WriteAddr: NOR memory internal address from which the data will be 
  *         written.
  * @param  NumHalfwordToWrite: number of Half words to write.
  *         The maximum allowed value is 32 Half words (64 bytes).
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_ProgramBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  uint32_t lastloadedaddress = 0x00;
  uint32_t currentaddress = 0x00;
  uint32_t endaddress = 0x00;

  /*!< Initialize variables */
  currentaddress = WriteAddr;
  endaddress = WriteAddr + NumHalfwordToWrite - 1;
  lastloadedaddress = WriteAddr;

  /*!< Issue unlock command sequence */
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA);

  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);  

  /*!< Write Write Buffer Load Command */
  NOR_WRITE(ADDR_SHIFT(WriteAddr), 0x0025);
  NOR_WRITE(ADDR_SHIFT(WriteAddr), (NumHalfwordToWrite - 1));

  /*!< Load Data into NOR Buffer */
  while(currentaddress <= endaddress)
  {
    /*!< Store last loaded address & data value (for polling) */
    lastloadedaddress = currentaddress;
 
    NOR_WRITE(ADDR_SHIFT(currentaddress), *pBuffer++);
    currentaddress += 1; 
  }

  NOR_WRITE(ADDR_SHIFT(lastloadedaddress), 0x29);
  
  return(FSMC_NOR_GetStatus(Program_Timeout));
}

/**
  * @brief  Reads a half-word from the NOR memory. 
  * @param  ReadAddr: NOR memory internal address to read from.
  * @retval Half-word read from the NOR memory
  */
uint16_t FSMC_NOR_ReadHalfWord(uint32_t ReadAddr)
{
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
  NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055);  
  NOR_WRITE((Bank1_NOR2_ADDR + ReadAddr), 0x00F0 );

  return (*(__IO uint16_t *)((Bank1_NOR2_ADDR + ReadAddr)));
}

/**
  * @brief  Reads a block of data from the FSMC NOR memory.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *         NOR memory.
  * @param  ReadAddr: NOR memory internal address to read from.
  * @param  NumHalfwordToRead : number of Half word to read.
  * @retval None
  */
void FSMC_NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  NOR_WRITE(ADDR_SHIFT(0x0555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AA), 0x0055);
  NOR_WRITE((Bank1_NOR2_ADDR + ReadAddr), 0x00F0);

  for(; NumHalfwordToRead != 0x00; NumHalfwordToRead--) /*!< while there is data to read */
  {
    /*!< Read a Halfword from the NOR */
    *pBuffer++ = *(__IO uint16_t *)((Bank1_NOR2_ADDR + ReadAddr));
    ReadAddr = ReadAddr + 2; 
  }  
}

/**
  * @brief  Returns the NOR memory to Read mode.
  * @param  None
  * @retval NOR_SUCCESS
  */
NOR_Status FSMC_NOR_ReturnToReadMode(void)
{
  NOR_WRITE(Bank1_NOR2_ADDR, 0x00F0);

  return (NOR_SUCCESS);
}

/**
  * @brief  Returns the NOR memory to Read mode and resets the errors in the NOR 
  *         memory Status Register.  
  * @param  None
  * @retval NOR_SUCCESS
  */
NOR_Status FSMC_NOR_Reset(void)
{
  NOR_WRITE(ADDR_SHIFT(0x00555), 0x00AA); 
  NOR_WRITE(ADDR_SHIFT(0x002AA), 0x0055); 
  NOR_WRITE(Bank1_NOR2_ADDR, 0x00F0); 

  return (NOR_SUCCESS);
}

/**
  * @brief  Returns the NOR operation status.
  * @param  Timeout: NOR progamming Timeout
  * @retval NOR_Status: The returned value can be: NOR_SUCCESS, NOR_ERROR
  *         or NOR_TIMEOUT
  */
NOR_Status FSMC_NOR_GetStatus(uint32_t Timeout)
{ 
  uint16_t val1 = 0x00, val2 = 0x00;
  NOR_Status status = NOR_ONGOING; 
  uint32_t timeout = Timeout;

  /*!< Poll on NOR memory Ready/Busy signal ----------------------------------*/
  while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) != RESET) && (timeout > 0)) 
  {
    timeout--;
  }

  timeout = Timeout;
  
  while((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET) && (timeout > 0))   
  {
    timeout--;
  }
  
  /*!< Get the NOR memory operation status -----------------------------------*/
  while((Timeout != 0x00) && (status != NOR_SUCCESS))
  {
    Timeout--;

    /*!< Read DQ6 and DQ5 */
    val1 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
    val2 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);

    /*!< If DQ6 did not toggle between the two reads then return NOR_Success */
    if((val1 & 0x0040) == (val2 & 0x0040)) 
    {
      return NOR_SUCCESS;
    }

    if((val1 & 0x0020) != 0x0020)
    {
      status = NOR_ONGOING;
    }

    val1 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
    val2 = *(__IO uint16_t *)(Bank1_NOR2_ADDR);
    
    if((val1 & 0x0040) == (val2 & 0x0040)) 
    {
      return NOR_SUCCESS;
    }
    else if((val1 & 0x0020) == 0x0020)
    {
      return NOR_ERROR;
    }
  }

  if(Timeout == 0x00)
  {
    status = NOR_TIMEOUT;
  } 

  /*!< Return the operation status */
  return (status);
}
/*----------------------------------------------------------------------------------------------------*/
#define BUFFER_SIZE        0x400      /* 1KB 1024个字节 */
#define WRITE_READ_ADDR    0x00000

uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE];
uint32_t WriteReadStatus = 0, Index = 0;
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);
void NOR_Delay(__IO uint32_t nCount);

NOR_IDTypeDef ID;

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

/*
 * 简单的延时函数
 */
void NOR_Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}

void NORFLASH_Test(void)
{
  /* 配置串口为 115200 8-N-1 */
	//USART1_Config();
	
  /* LED GPIO 初始化 */
	LED_GPIO_Config();
	
	LED2(OFF);
	LED3(OFF);
  
  /* NORFLASH初始化 */
  FSMC_NOR_Init();
  
  /* 读取NORFLASH ID */
	FSMC_NOR_ReadID(&ID);	
	printf("\r\nManufacturer_Code: %X \r\n", ID.Manufacturer_Code);
	printf("\r\nDevice_Code1: %X \r\n", ID.Device_Code1);
	printf("\r\nDevice_Code2: %X \r\n", ID.Device_Code2);
	printf("\r\nDevice_Code3: %X \r\n", ID.Device_Code3);
	
/* 
 * 一共16MB， 一共128块，每块128KB, 128*1024=131072=0x20000 
 * 写入前记得将相应的块擦除
 * 擦出比较慢，请等待
 */
	FSMC_NOR_EraseBlock(0x00000);
  
  /* 填充要发送的缓冲区 */
  Fill_Buffer(TxBuffer, BUFFER_SIZE, 0);
  
  FSMC_NOR_WriteBuffer(TxBuffer, 0, BUFFER_SIZE);
  
  FSMC_NOR_ReadBuffer(RxBuffer, 0, BUFFER_SIZE);
  
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
    printf("NORFLASH 测试成功\r\n");
		
		#ifdef LCD_DISPLAY
			LCD_DispEnCh(20,180,"NORFLASH 测试成功",BLUE);	
		#endif	
    while(1)
    {
      LED2_TOGGLE;
      NOR_Delay(0x0FFFFF);
    }    
  }                               
  else
  { /* KO */		
    while(1)
    {
			#ifdef LCD_DISPLAY
				LCD_DispEnCh(20,180,"NORFLASH 测试失败,请检查NORFLASH模块与开发板是否正常连接",RED);	
			#endif		
      LED3_TOGGLE;
      NOR_Delay(0x0FFFFF);
    }
  }
}
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
