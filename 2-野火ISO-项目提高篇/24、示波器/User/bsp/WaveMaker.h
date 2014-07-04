#ifndef __WAVEMAKER_H
#define __WAVEMAKER_H

#include "stm32f10x.h"

/*
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)
typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t SWTRIGR;
  __IO uint32_t DHR12R1;
  __IO uint32_t DHR12L1;
  __IO uint32_t DHR8R1;
  __IO uint32_t DHR12R2;
  __IO uint32_t DHR12L2;
  __IO uint32_t DHR8R2;
  __IO uint32_t DHR12RD;
  __IO uint32_t DHR12LD;
  __IO uint32_t DHR8RD;
  __IO uint32_t DOR1;
  __IO uint32_t DOR2;
} DAC_TypeDef;


#define DAC_DHR12RD_Address      0x40007420
*/
#define DAC_DHR8R1_Address      0x40007410
#define DAC_DHR12R1_Address		0x40007408

/* 波形类型 */
enum
{
	WT_SIN = 0,		/* 正弦波 */
	WT_RECT = 1		/* 矩形波 */
};
void InitNoise(void);
void InitTriangle(void);
void InitWaveMaker(void);
void SetupWaveType(uint8_t _type);

#endif
