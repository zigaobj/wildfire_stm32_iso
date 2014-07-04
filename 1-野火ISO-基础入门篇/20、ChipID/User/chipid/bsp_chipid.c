/**
  ******************************************************************************
  * @file    bsp_chipid.c
  * @author  
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   Get_ChipID bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_chipid.h"

uint32_t ChipUniqueID[3];

/**
  * @brief  Get_ChipID
  * @param  无
  * @retval 无
  */
void Get_ChipID(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7F0); // 高字节
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7E8); // 低字节
}
/*********************************************END OF FILE**********************/
