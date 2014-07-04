/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_ili9341_lcd.h
*	版    本 : V1.0
*	说    明 : 驱动芯片的访问地址为:  0x60000000
*				
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2011-10-31   WildFire Team  ST固件库版本 V3.5.0版本。
*       v1.1    2011-11-07   WildFire Team  修正竖屏显示，通过直接将X,Y都设置为递减，加速GUI显示。
*
*
*	Copyright (C), 2012-2013, WildFire Team
*
*********************************************************************************************************
*/

#include "stm32f10x.h"


/* 定义LCD驱动器的访问地址 */
#define ILI9341_REG		*(__IO uint16_t *)(0x6C000000)
#define ILI9341_RAM		*(__IO uint16_t *)(0x6D000000)

//#define ILI9341_REG		*(__IO uint16_t *)(0x60000000)
//#define ILI9341_RAM		*(__IO uint16_t *)(0x60020000)

void LCD9341_SetCursor(uint16_t _usX1 , uint16_t _usY1);
void LCD9341_SetPoint(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usColor);

void LCD9341_Clear(uint16_t _usColor);
void LCD9341_SetDispWin(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2);
void LCD9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor);
void LCD9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor);
void LCD9341_FillRect(uint16_t _usX1 , uint16_t _usY1 ,  uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);

uint16_t LCD9341_GetPoint(uint16_t _usX1 , uint16_t _usY1);

void bsp_InitLCD(void);


