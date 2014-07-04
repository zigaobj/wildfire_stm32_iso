/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_KEY.c
 * @brief      KEY驱动函数实现
 * @author     野火科技
 * @version    v5.0
 * @date       2013-07-10
 */


/*
 * 包含头文件
 */
#include "FIRE_key.h"

#define DELAY_MS(x)	Key_Delay(100*x)


//按照 上、下、左、右、ok 这样的键顺序排列
const KEY_PTx_TypeDef KEY_PTxn[KEY_MAX]={{KEY_UP_PORT,KEY_UP_PIN},{KEY_DOWN_PORT,KEY_DOWN_PIN},{KEY_LEFT_PORT,KEY_LEFT_PIN},{KEY_RIGHT_PORT,KEY_RIGHT_PIN},{KEY_SELECT_PORT,KEY_SELECT_PIN}};


/* 延时消抖用到的延时函数 */
static void Key_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}	

/*!
 *  @brief      初始化key端口
 *  @param      无
 *  @since      v5.0
 *  Sample usage:       KEY_init ();    //初始化 
 */
void    key_init(void)
{
	uint8_t key = KEY_MAX;
	
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOB和GPIOF的外设时钟*/
	RCC_APB2PeriphClockCmd( KEY_RIGHT_CLK|KEY_LEFT_CLK|KEY_DOWN_CLK|KEY_UP_CLK|KEY_SELECT_CLK, ENABLE); 

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	
  
	//初始化全部 按键
	while(key--)
	{
		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = KEY_PTxn[key].pin ;	
		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(KEY_PTxn[key].port , &GPIO_InitStructure);
			
	}

  
}

/*!
 *  @brief      获取key状态（不带延时消抖）
 *  @param      KEY_e           KEY编号
 *  @return     KEY_STATUS_e    KEY状态（KEY_DOWN、KEY_DOWN）
 *  @since      v5.0
 *  Sample usage:
                    if(key_get(KEY_U) ==  KEY_DOWN)
                    {
                        printf("\n按键按下")
                    }
 */
KEY_STATUS_e key_get(KEY_e key)
{
    if(GPIO_ReadInputDataBit(KEY_PTxn[key].port ,KEY_PTxn[key].pin ) == Bit_RESET)
    {
        return KEY_DOWN;
    }
    return KEY_UP;
}


/*!
 *  @brief      检测key状态（带延时消抖）
 *  @param      KEY_e           KEY编号
 *  @return     KEY_STATUS_e    KEY状态（KEY_DOWN、KEY_DOWN）
 *  @since      v5.0
 *  Sample usage:
                    if(key_check(KEY_U) ==  KEY_DOWN)
                    {
                        printf("\n按键按下")
                    }
 */
KEY_STATUS_e key_check(KEY_e key)
{
    if(GPIO_ReadInputDataBit(KEY_PTxn[key].port ,KEY_PTxn[key].pin ) == Bit_RESET)
    {
        DELAY_MS(10);
        if(GPIO_ReadInputDataBit(KEY_PTxn[key].port ,KEY_PTxn[key].pin ) == Bit_RESET)
        {
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}


/*********************  如下代码是实现按键定时扫描，发送消息到FIFO  ********************/
/*
 * 定义按键消息FIFO状态
 */
typedef enum
{
    KEY_MSG_EMPTY,      //没有按键消息
    KEY_MSG_NORMAL,     //正常，有按键消息，但不满
    KEY_MSG_FULL,       //按键消息满
} key_msg_e;

/*
 * 定义按键消息FIFO相关的变量
 */
KEY_MSG_t           key_msg[KEY_MSG_FIFO_SIZE];             //按键消息FIFO
volatile uint8_t    key_msg_front = 0, key_msg_rear = 0;    //接收FIFO的指针
volatile uint8_t    key_msg_flag = KEY_MSG_EMPTY;           //按键消息FIFO状态


/*!
 *  @brief      发送按键消息到FIFO
 *  @param      KEY_MSG_t       按键消息
 *  @since      v5.0
 *  Sample usage:
                    KEY_MSG_t *keymsg;
                    keymsg.key      = KEY_U;
                    keymsg.status   = KEY_HOLD;
                    send_key_msg(keymsg);                   //发送
 */
void send_key_msg(KEY_MSG_t keymsg)
{
    uint8_t tmp;
    //保存在FIFO里
    if(key_msg_flag == KEY_MSG_FULL)
    {
        //满了直接不处理
        return ;
    }
    key_msg[key_msg_rear].key = keymsg.key;
    key_msg[key_msg_rear].status = keymsg.status;

    key_msg_rear++;

    if(key_msg_rear >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_rear = 0;                       //重头开始
    }

    tmp = key_msg_rear;
    if(tmp == key_msg_front)                   //追到屁股了，满了
    {
        key_msg_flag = KEY_MSG_FULL;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }
}


/*!
 *  @brief      从FIFO里获取按键消息
 *  @param      KEY_MSG_t       按键消息
 *  @return     是否获取按键消息（1为获取成功，0为没获取到按键消息）
 *  @since      v5.0
 *  Sample usage:
                    KEY_MSG_t keymsg;
                    if(get_key_msg(&keymsg) == 1)
                    {
                        printf("\n按下按键KEY%d,类型为%d（0为按下，1为弹起，2为长按）",keymsg.key,keymsg.status);
                    }
 */
uint8_t get_key_msg(KEY_MSG_t *keymsg)
{
    uint8_t tmp;

    if(key_msg_flag == KEY_MSG_EMPTY)               //按键消息FIFO为空，直接返回0
    {
        return 0;
    }

    keymsg->key = key_msg[key_msg_front].key;       //从FIFO队首中获取按键值
    keymsg->status = key_msg[key_msg_front].status; //从FIFO队首中获取按键类型

    key_msg_front++;                                //FIFO队首指针加1，指向下一个消息

    if(key_msg_front >= KEY_MSG_FIFO_SIZE)          //FIFO指针队首溢出则从0开始计数
    {
        key_msg_front = 0;                          //重头开始计数（循环利用数组）
    }

    tmp = key_msg_rear;
    if(key_msg_front == tmp)                        //比较队首和队尾是否一样，一样则表示FIFO已空了
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }

    return 1;
}

/*!
 *  @brief      定时检测key状态
 *  @since      v5.0
 *  @note       此函数需要放入 定时中断复位函数里，定时10ms执行一次
 */
void key_IRQHandler(void)
{

    KEY_e   keynum;
    static uint8_t keytime[KEY_MAX];                          //静态数组，保存各数组按下时间

    KEY_MSG_t keymsg;                                       //按键消息

    for(keynum = (KEY_e)0 ; keynum < KEY_MAX; keynum ++)    //每个按键轮询
    {
        if(key_get(keynum) == KEY_DOWN)                     //判断按键是否按下
        {
            keytime[keynum]++;                              //按下时间累加

            if(keytime[keynum] <= KEY_DOWN_TIME)            //判断时间是否没超过消抖确认按下时间
            {
                continue;                                   //没达到，则继续等待
            }
            else if(keytime[keynum] == KEY_DOWN_TIME + 1 )  //判断时间是否为消抖确认按下时间
            {
                //确认按键按下
                keymsg.key = keynum;
                keymsg.status = KEY_DOWN;
                send_key_msg(keymsg);                       //把按键值和按键类型发送消息到FIFO
            }
            else if(keytime[keynum] <= KEY_HOLD_TIME)       //是否没超过长按HOLD按键确认时间
            {
                continue;                                   //没超过，则继续等待
            }
            else if(keytime[keynum]  == KEY_HOLD_TIME + 1)  //是否为长按hold确认时间
            {
                //确认长按HOLD
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD;
                send_key_msg(keymsg);                       //发送
                keytime[keynum] = KEY_DOWN_TIME + 1;
            }
            else
            {
                keytime[keynum] = KEY_DOWN_TIME + 1;        //继续重复检测 hold 状态
            }
        }
        else
        {
            if(keytime[keynum] > KEY_DOWN_TIME)             //如果确认过按下按键
            {
                keymsg.key = keynum;
                keymsg.status = KEY_UP;
                send_key_msg(keymsg);                       //发送按键弹起消息
            }

            keytime[keynum] = 0;                            //时间累计清0
        }
    }
}



