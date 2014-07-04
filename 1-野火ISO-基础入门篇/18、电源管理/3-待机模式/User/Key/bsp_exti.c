/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_exti.h"
#include "bsp_led.h"
#include "bsp_usart1.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置P[A|B|C|D|E]13为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* 配置P[A|B|C|D|E]0为中断源 */
	
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

	
}

 /**
  * @brief  配置 PC13 中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PC13) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
												
	/* config the NVIC(PC13) */
	NVIC_Configuration();

	/* EXTI line gpio config(PC13) */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	/* EXTI line(PC13) mode config */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); 
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
	
	/* EXTI line gpio config(PA0) */	
//	不需要配置PA0引脚
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 					// 下拉输入	
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	/* EXTI line(PA0) mode config */
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); 
//	
//  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿中断
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure); 
	
	
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}



 /**
  * @brief  用于检测按键是否被长时间按下
  * @param  无
  * @retval 1 ：按键被长时间按下  0 ：按键没有被长时间按下
  */
uint8_t PWR_Check_Standby(void)
{			
	uint8_t downCnt =0;																				//记录按下的次数
	uint8_t upCnt =0;																					//记录松开的次数			

	while(1)																										//死循环，由return结束
	{	
		Delay(0xFFFF);																					//延迟一段时间再检测

		if(GPIO_ReadInputDataBit (GPIOC,GPIO_Pin_13) == RESET)	//检测到按下按键
		{
			LED1(ON);LED2(ON);LED3(ON);												//点亮所有LED灯

			downCnt++;																					//记录按下次数
			upCnt=0;																						//清除按键释放记录

			if(downCnt>=100)																		//按下时间足够
			{
				LED1(OFF);LED2(OFF);LED3(OFF);	  	 
				return 1; 																				//检测到按键被时间长按下
			}
		}
		else 
		{
			upCnt++; 																						//记录释放次数
			if(upCnt>5)																					//连续检测到释放超过5次
			{
				LED1(OFF);LED2(OFF);LED3(OFF);									//关闭所有LED灯	
				return 0;																				//按下时间太短，不是按键长按操作
			}
		}//	if(GPIO_ReadInputDataBit 

	}//while

}
/*********************************************END OF FILE**********************/
