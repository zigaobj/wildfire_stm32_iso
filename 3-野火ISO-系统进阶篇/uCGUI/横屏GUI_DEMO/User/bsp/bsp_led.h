/*
*********************************************************************************************************
*	                                  
*	模块名称 : LED指示灯驱动模块    
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2011-11-01   WildFire Team  ST固件库版本 V3.5.0版本。
*
*	Copyright (C), 2012-2013, WildFire Team
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "bsp_sys.h"

#define LED1  PCout(3)
#define LED2  PCout(4)
#define LED3  PCout(5)


/* 供外部调用的函数声明 */
void bsp_InitLed(void);
void bsp_LedOn(uint8_t _no);
void bsp_LedOff(uint8_t _no);
void bsp_LedToggle(uint8_t _no);

#endif


