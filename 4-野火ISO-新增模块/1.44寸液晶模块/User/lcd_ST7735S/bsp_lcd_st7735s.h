/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_LCD_ST7735S.h
 * @brief      LCD 函数库
 * @author     野火科技
 * @version    
 * @date       2013-09-07
 */

#ifndef _BSP_LCD_ST7735S_H_
#define _BSP_LCD_ST7735S_H_

#include "stm32f10x.h"
#include "FIRE_lcd.h"


#define ST7735S_DELAY()         Lcd_Delay(10);                //do{asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");}while(0)
#define ST7735S_DELAY_MS(ms)    Lcd_Delay(ms*1000)


//用户配置
#define ST7735S_DIR_DEFAULT     0       //定义默认方向 (0~3)

#if (USE_LCD == LCD_ST7735S)

/*********************** API接口 ******************************/
//提供API接口给LCD调用

#define LCD_H                   ST7735S_get_h()                 //高
#define LCD_W                   ST7735S_get_w()                 //宽

#define LCD_INIT()              LCD_ST7735S_init()              //初始化
#define LCD_PTLON(site,size)    LCD_ST7735S_ptlon(site,size)    //开窗
#define LCD_RAMWR()             LCD_WR_CMD(0x2C)                //写模式
#define LCD_WR_DATA(data)       do{LCD_ST7735S_WR_8DATA((uint8_t)((data)>>8) );LCD_ST7735S_WR_8DATA( (uint8_t)(data) );}while(0)       //写数据
#define LCD_WR_CMD(cmd)         LCD_ST7735S_WR_8CMD(cmd)        //命令
#define LCD_SET_DIR(opt)        LCD_ST7735S_dir(opt)            //方向

#define LCD_DIR                 ST7735S_get_dir()               //获取方向


//通过函数调用，速度太慢了，改成宏定义调用
//#define LCD_WR_8DATA(data)      LCD_ST7735S_wr_data(data)       //写数据
//#define LCD_WR_8CMD(cmd)        LCD_ST7735S_wr_cmd(cmd)         //命令

#endif  //(USE_LCD == LCD_ST7735S)

/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)									{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)									{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)							{p->ODR ^=i;}			//输出反转状态

#define digitalOut8HData(p,data)		   	{p->ODR = ((data<<8) | (p->ODR & 0xff));}	//输出IO的高8位,低8位不变

/* 定义控制IO的宏 */
#define LCD_ST7735S_WR_H			digitalHi(GPIOC,GPIO_Pin_5)
#define LCD_ST7735S_WR_L			digitalLo(GPIOC,GPIO_Pin_5)

#define LCD_ST7735S_RD_H			digitalHi(GPIOA,GPIO_Pin_1)
#define LCD_ST7735S_RD_L			digitalLo(GPIOA,GPIO_Pin_1)

#define LCD_ST7735S_CS_H			digitalHi(GPIOA,GPIO_Pin_3)
#define LCD_ST7735S_CS_L			digitalLo(GPIOA,GPIO_Pin_3)

#define LCD_ST7735S_RS_H			digitalHi(GPIOA,GPIO_Pin_0)
#define LCD_ST7735S_RS_L			digitalLo(GPIOA,GPIO_Pin_0)

#define LCD_ST7735S_RST_H			digitalHi(GPIOA,GPIO_Pin_2)
#define LCD_ST7735S_RST_L			digitalLo(GPIOA,GPIO_Pin_2)

#define LCD_ST7735S_P0(data)	digitalOut8HData(GPIOB,data)


#define LCD_ST7735S_WR_8CMD(cmd)    do\
                            {\
                                LCD_ST7735S_RD_H;\
                                LCD_ST7735S_RS_L;\
                                LCD_ST7735S_CS_L;\
                                LCD_ST7735S_P0(cmd);\
                                LCD_ST7735S_WR_L;\
                                LCD_ST7735S_WR_H;\
                                LCD_ST7735S_CS_H;\
                            }while(0)   //LCD_WR=0;LCD_WR=1;产生一个上升沿

														
#define LCD_ST7735S_WR_8DATA(data)  do\
                            {\
                                LCD_ST7735S_RD_H;\
                                LCD_ST7735S_RS_H;\
                                LCD_ST7735S_CS_L;\
                                LCD_ST7735S_P0(data);\
                                LCD_ST7735S_WR_L;\
                                LCD_ST7735S_WR_H;\
                                LCD_ST7735S_CS_H;\
                            }while(0)   //LCD_WR=0;在这里写入数据到RAM

														
														
#define LCD_ST7735S_WR_DATA(data)       do{LCD_ST7735S_WR_8DATA((uint8_t)((data)>>8) );LCD_ST7735S_WR_8DATA( (uint8_t)(data) );}while(0)       //写数据
														

void LCD_ST7735S_init(void);
void LCD_ST7735S_dir(uint8_t option);
void LCD_ST7735S_ptlon(Site_t site, Size_t size);
uint16_t ST7735S_get_h(void);
uint16_t ST7735S_get_w(void);
uint8_t  ST7735S_get_dir(void);


#endif  //_BSP_LCD_ST7735S_H_




