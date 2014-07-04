/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_KEY.h
 * @brief      KEY驱动头文件
 * @author     野火科技
 * @version    v5.0
 * @date       2013-07-10
 */

#ifndef __FIRE_KEY_H__
#define __FIRE_KEY_H__

#include "stm32f10x.h"


//下面是定义按键的时间，单位为 ： 10ms（中断时间）
#define KEY_DOWN_TIME           1       //消抖确认按下时间
#define KEY_HOLD_TIME           50      //长按hold确认时间，最多253，否则需要修改 keytime 的类型
                                        //如果按键一直按下去，则每隔 KEY_HOLD_TIME - KEY_DOWN_TIME 时间会发送一个 KEY_HOLD 消息

//定义按键消息FIFO大小
#define KEY_MSG_FIFO_SIZE       20      //最多 255，否则需要修改key_msg_front/key_msg_rear类型

//按键端口的枚举
typedef enum
{
    KEY_U,  //上
    KEY_D,  //下

    KEY_L,  //左
    KEY_R,  //右

//  KEY_A,  //取消
    KEY_B,  //选择

//  KEY_START,  //开始
//  KEY_STOP,   //停止

    KEY_MAX,
} KEY_e;

/* 定义使用到的GPIO引脚和时钟 */

#define KEY_RIGHT_PORT	GPIOA
#define KEY_RIGHT_PIN		GPIO_Pin_8
#define KEY_RIGHT_CLK		RCC_APB2Periph_GPIOA

#define KEY_LEFT_PORT		GPIOB
#define KEY_LEFT_PIN		GPIO_Pin_5
#define KEY_LEFT_CLK		RCC_APB2Periph_GPIOB

#define KEY_UP_PORT			GPIOD
#define KEY_UP_PIN			GPIO_Pin_3
#define KEY_UP_CLK			RCC_APB2Periph_GPIOD

#define KEY_DOWN_PORT		GPIOC
#define KEY_DOWN_PIN		GPIO_Pin_6
#define KEY_DOWN_CLK		RCC_APB2Periph_GPIOC

#define KEY_SELECT_PORT	GPIOC
#define KEY_SELECT_PIN	GPIO_Pin_7
#define KEY_SELECT_CLK	RCC_APB2Periph_GPIOC

/*
 * 定义 KEY 编号对应的管脚
 */
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
}KEY_PTx_TypeDef;


//key状态宏定义
typedef enum
{
    KEY_DOWN  =   0,         //按键按下时对应电平
    KEY_UP    =   1,         //按键弹起时对应电平

    KEY_HOLD,

} KEY_STATUS_e;

//按键消息结构体
typedef struct
{
    KEY_e           key;
    KEY_STATUS_e    status;
} KEY_MSG_t;


void            key_init(void);            // KEY初始化函数(key 小于 KEY_MAX 时初始化 对应端口，否则初始化全部端口)
KEY_STATUS_e    key_check(KEY_e key);           //检测key状态（带延时消抖）


//定时扫描按键
uint8_t get_key_msg(KEY_MSG_t *keymsg);         //获取按键消息，返回1表示有按键消息，0为无按键消息
void    key_IRQHandler(void);                   //需要定时扫描的中断复位函数（定时时间为10ms）


#endif  //__FIRE_KEY_H__

