/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

#ifndef  __BSP_H__
#define  __BSP_H__

#include  <stm32f10x.h>


#include "bsp_int.h"
#include "bsp_led.h"
#include "bsp_touch.h"
#include "bsp_ili9341_lcd.h"



void BSP_Init(void);
void BSP_IntDisAll(void);
uint32_t BSP_CPU_ClkFreq(void);

#endif
