#include "app.h"

extern	OS_TCB	StartUp_TCB;					

static  OS_TCB	LED1_TCB;		   				
static	CPU_STK	LED1_Stk[TASK_LED1_STK_SIZE];	   

static  OS_TCB	LED2_TCB;		   				
static	CPU_STK	LED2_Stk[TASK_LED2_STK_SIZE];	   	

static  OS_TCB	LED3_TCB;		   					
static	CPU_STK	LED3_Stk[TASK_LED3_STK_SIZE];	   

/* 
 * 函数名：Task_Start
 * 描述  : 启动任务，
 *			   优先级为3，
 *		     创建LED1、LED2和LED3的任务
 * 输入  ：无
 * 输出  : 无
 */
void Task_Start(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;	

	//创建任务LED1	
	OSTaskCreate((OS_TCB     *)&LED1_TCB,					            // 任务控制块指针          
               (CPU_CHAR   *)"LED1",						            // 任务名称
               (OS_TASK_PTR )Task_LED1,					            // 任务代码指针
               (void       *)0,							                // 传递给任务的参数parg
               (OS_PRIO     )TASK_LED1_PRIO,				        // 任务优先级
               (CPU_STK    *)&LED1_Stk[0],				          // 任务堆栈基地址
               (CPU_STK_SIZE)TASK_LED1_STK_SIZE/10,		      // 堆栈剩余警戒线
               (CPU_STK_SIZE)TASK_LED1_STK_SIZE,			      // 堆栈大小
               (OS_MSG_QTY  )0,							                // 可接收的最大消息队列数
               (OS_TICK     )0,							                // 时间片轮转时间
               (void       *)0,							                // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        // 任务选项
               (OS_ERR     *)&err);						              // 返回值

	//创建任务LED2	
	OSTaskCreate((OS_TCB     *)&LED2_TCB,					     
               (CPU_CHAR   *)"LED2",						
               (OS_TASK_PTR )Task_LED2,					
               (void       *)0,							
               (OS_PRIO     )TASK_LED2_PRIO,				
               (CPU_STK    *)&LED2_Stk[0],				
               (CPU_STK_SIZE)TASK_LED2_STK_SIZE/10,		
               (CPU_STK_SIZE)TASK_LED2_STK_SIZE,			
               (OS_MSG_QTY  )0,							
               (OS_TICK     )0,							
               (void       *)0,							
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	 
               (OS_ERR     *)&err);					

	//创建任务LED3	
	OSTaskCreate((OS_TCB     *)&LED3_TCB,					  
               (CPU_CHAR   *)"LED3",						
               (OS_TASK_PTR )Task_LED3,				
               (void       *)0,							
               (OS_PRIO     )TASK_LED3_PRIO,				
               (CPU_STK    *)&LED3_Stk[0],				
               (CPU_STK_SIZE)TASK_LED3_STK_SIZE/10,		
               (CPU_STK_SIZE)TASK_LED3_STK_SIZE,			
               (OS_MSG_QTY  )0,						
               (OS_TICK     )0,							
               (void       *)0,							
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	 
               (OS_ERR     *)&err);	
               
  //任务删除自己	
	OSTaskDel(&StartUp_TCB,&err);							 
}


/* 
 * 函数名：Task_LED1
 * 描述  : LED任务1，
 *		     优先级为4，
 *		     以100ms的间隔点亮、关闭LED1
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED1(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;                	

  while (1)
  {
    LED1( ON );
    OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);	//延时阻塞100ms
    LED1( OFF);     
    OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

/* 
 * 函数名：Task_LED2
 * 描述  : 	LED任务2，
 *			    优先级为5，
 *		      以200ms的间隔点亮、关闭LED2
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED2(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;                	

  while (1)
  {
    LED2( ON );
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	 //延时阻塞200ms
    LED2( OFF);
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

/* 
 * 函数名：Task_LED3
 * 描述  : LED任务3，
 *			   优先级为6，
 *		     以300ms的间隔点亮、关闭LED3
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED3(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;      

  while (1)
  {
    LED3( ON );
    OSTimeDlyHMSM(0, 0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);	 //延时阻塞300ms
    LED3( OFF);
    OSTimeDlyHMSM(0, 0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

/* -------------------------------------end of file------------------------------------ */
