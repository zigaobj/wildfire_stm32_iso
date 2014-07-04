/*
*********************************************************************************************************
*	                                  
*	模块名称 : 延时函数	
*	文件名称 : bsp_delay.c
*	版    本 : V1.0
*	说    明 : 延时例程
*
*********************************************************************************************************
*/
#include "delay.h"	
 

static u8  fac_us=0;  /* us延时倍乘数 */			   
static u16 fac_ms=0;  /* ms延时倍乘数,在ucos下,代表每个节拍的ms数 */

			   
/*
*********************************************************************************************************
*	函 数 名: delay_init
*	功能说明: 初始化延迟函数
*             当使用ucos的时候,此函数会初始化ucos的时钟节拍
*             SYSTICK的时钟固定为HCLK时钟的1/8 
*	形    参：SYSCLK: 系统时钟			  
*	返 回 值: 无 
*********************************************************************************************************
*/
void delay_init(u8 SYSCLK)
{
 	SysTick->CTRL&=~(1<<2);	                  /* SYSTICK使用外部时钟源 */	 
	fac_us=9;		                  /* 不论是否使用ucos,fac_us都需要使用 */
	fac_ms=(u16)fac_us*1000;                  /* 非ucos下,代表每个ms需要的systick时钟数 */   
}								    


/* 微秒延时 */		    								   
void delay_us(u32 nus)
{		
	u32 temp;
	SysTick->LOAD=nus*fac_us;           /* 时间加载   */	  		 
	SysTick->VAL=0x00;                  /* 清空计数器 */
	SysTick->CTRL=0x01 ;                /* 开始倒数   */	 
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));/* 等待时间到达 */   
	SysTick->CTRL=0x00;                 /* 关闭计数器   */
	SysTick->VAL =0X00;                 /* 清空计数器   */	 
}
/* 毫秒延时 */
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;     /* 时间加载(SysTick->LOAD为24bit) */
	SysTick->VAL =0x00;                /* 清空计数器					 */
	SysTick->CTRL=0x01 ;               /* 开始倒数  					 */
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));/* 等待时间到达 */   			    
	SysTick->CTRL=0x00;                 /* 关闭计数器	*/
	SysTick->VAL =0X00;                 /* 清空计数器   */	  	    
} 
