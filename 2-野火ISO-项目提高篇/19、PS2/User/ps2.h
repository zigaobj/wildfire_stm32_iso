/*
*********************************************************************************************************
*	                                  
*	模块名称 : 延时函数	  (采用正点原子开发板)
*	文件名称 : bsp_ps2.h
*	版    本 : V1.0
*	说    明 : 鼠标驱动           
*	修改记录 : 这里的修改记录也是指的原子开发板的修改记录
*			   
*
*   版本号     日期       作者      说明
*   v1.0     2012-10-26  Eric2013  ST固件库版本 V3.5.0版本。		
*   BLOG: http://blog.sina.com.cn/u/2565749395
*********************************************************************************************************
*/
#ifndef __PS2_H
#define __PS2_H	 	   
#include "sys.h"	
#include "stm32f10x.h"
//物理接口定义
//PS2输入 		  				    
#define PS2_SCL PCin(4)	//PC11
#define PS2_SDA PCin(5)	//PC10
//PS2输出
#define PS2_SCL_OUT PCout(4)	//PC11
#define PS2_SDA_OUT PCout(5)	//PC10

//设置PS2_SCL输入输出状态.		  
#define PS2_SET_SCL_IN()  {GPIOC->CRL&=0XFFF0FFFF;GPIOC->CRL|=0X00080000;}
#define PS2_SET_SCL_OUT() {GPIOC->CRL&=0XFFF0FFFF;GPIOC->CRL|=0X00030000;}	  
//设置PS2_SDA输入输出状态.		  
#define PS2_SET_SDA_IN()  {GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=0X00800000;}
#define PS2_SET_SDA_OUT() {GPIOC->CRL&=0XFF0FFFFF;GPIOC->CRL|=0X00300000;} 

#define MOUSE    0X20 //鼠标模式
#define KEYBOARD 0X10 //键盘模式
#define CMDMODE  0X00 //发送命令
//PS2_Status当前状态标志
//[5:4]:当前工作的模式;[7]:接收到一次数据
//[6]:校验错误;[3:0]:收到的数据长度;	 
extern u8 PS2_Status;       //定义为命令模式
extern u8 PS2_DATA_BUF[16]; //ps2数据缓存区                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
extern u8 MOUSE_ID;

void PS2_Init(void);
u8 PS2_Send_Cmd(u8 cmd);
void PS2_Set_Int(u8 en);
u8 PS2_Get_Byte(void);
void PS2_En_Data_Report(void);  
void PS2_Dis_Data_Report(void);		  				    
#endif





















