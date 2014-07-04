/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   os 单任务 测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "includes.h"


OS_TCB	StartUp_TCB;		   					          //定义任务控制块
CPU_STK	StartUp_Stk[STARTUP_TASK_STK_SIZE];		//定义任务堆栈

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  OS_ERR err;	

  /* 板级初始化 */	
	BSP_Init();

  /* 初始化"uC/OS-III"内核 */  
	OSInit(&err);		                                        
  
	/*创建任务*/
	OSTaskCreate((OS_TCB     *)&StartUp_TCB,                // 任务控制块指针          
               (CPU_CHAR   *)"StartUp",		                // 任务名称
               (OS_TASK_PTR )Task_Start, 	                // 任务代码指针
               (void       *)0,			                      // 传递给任务的参数parg
               (OS_PRIO     )STARTUP_TASK_PRIO,			      // 任务优先级
               (CPU_STK    *)&StartUp_Stk[0],	            // 任务堆栈基地址
               (CPU_STK_SIZE)STARTUP_TASK_STK_SIZE/10,	  // 堆栈剩余警戒线
               (CPU_STK_SIZE)STARTUP_TASK_STK_SIZE,			  // 堆栈大小
               (OS_MSG_QTY  )0,			                      // 可接收的最大消息队列数
               (OS_TICK     )0,			                      // 时间片轮转时间
               (void       *)0,			                      // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	      // 任务选项
               (OS_ERR     *)&err);		                    // 返回值
	  
  /* 启动多任务系统，控制权交给uC/OS-II */
  OSStart(&err);                                       
}

/*********************************************END OF FILE**********************/

