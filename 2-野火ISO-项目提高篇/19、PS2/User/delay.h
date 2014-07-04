/*
*********************************************************************************************************
*	                                  
*	模块名称 : 延时函数	  (采用正点原子开发板)
*	文件名称 : bsp_delay.h
*	版    本 : V1.0
*	说    明 : 延时例程
*              主要是实现毫秒和微秒的延时
*	修改记录 : 这里的修改记录也是指的原子开发板的修改记录
*			   
*	V1.2修改说明
*			   修正了中断中调用出现死循环的错误
*		 	   防止延时不准确,采用do while结构!
*			
*	V1.3修改说明
*			   增加了对UCOSII延时的支持.
*			   如果使用ucosII,delay_init会自动设置SYSTICK的值,使之与ucos的TICKS_PER_SEC对应.
*			   delay_ms和delay_us也进行了针对ucos的改造.
*			   delay_us可以在ucos下使用,而且准确度很高,更重要的是没有占用额外的定时器.
*			   delay_ms在ucos下,可以当成OSTimeDly来用,在未启动ucos时,它采用delay_us实现,从而准确延时
*			   可以用来初始化外设,在启动了ucos之后delay_ms根据延时的长短,选择OSTimeDly实现或者delay_us实现.
*			
*	V1.4修改说明 20110929
*			   修改了使用ucos,但是ucos未启动的时候,delay_ms中中断无法响应的bug.
*			   V1.5修改说明 20120902
*			   在delay_us加入ucos上锁，防止由于ucos打断delay_us的执行，可能导致的延时不准。
*
*   版本号     日期       作者      说明
*   v1.0     2012-10-26  Eric2013  ST固件库版本 V3.5.0版本。		
*   BLOG: http://blog.sina.com.cn/u/2565749395
*********************************************************************************************************
*/

#ifndef __DELAY_H
#define __DELAY_H 			   

#include "stm32f10x.h"
	 
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























