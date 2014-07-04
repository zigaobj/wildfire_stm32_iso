/*
*********************************************************************************************************
*	                                  
*	模块名称 : 任务建立及其相应的任务函数
*	文件名称 : app.c
*	版    本 : V1.0
*	说    明 : uCOS-II任务。
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2012-10-31   WildFire Team  ST固件库版本 V3.5.0版本。
*
*	Copyright (C), 2012-2013, WildFire Team
*		
*********************************************************************************************************
*/

#include <includes.h>	/* 该文件包含了所有必需的.h文件 */
#include "GUI.h"

/*
**************************************************************************
*                  模块内全局变量
**************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskGUIStk[APP_TASK_GUI_STK_SIZE];	
static OS_STK AppTaskCOMStk[APP_TASK_COM_STK_SIZE];	
static OS_STK AppTaskUpdateStk[APP_TASK_UPDATE_STK_SIZE];	
			
/*
****************************************************************************
*                                      函数声明
****************************************************************************
*/

static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
static void AppTaskUserIF(void *p_arg);
static void AppTaskGUI(void *p_arg);
static void AppTaskCom(void *p_arg);
extern void MainTask(void);

/*
***************************************************************************
*                       函数定义
****************************************************************************
*/

/* 定义一个邮箱，    这只是一个邮箱指针，  OSMboxCreate函数会创建邮箱必需的资源 */
OS_EVENT *AppUserIFMbox;
OS_EVENT *AppMsgMbox;

/* 定义一个信号量，  这只是一个信号量指针，OSSEMCreate函数会创建邮箱必需的资源 */
 
OS_EVENT *Semp;
OS_EVENT *AppTouchSemp;

/*创建一个内存块*/
/*创建一个内存分区 2个内存块 每个内存块2050个字节*/
OS_MEM   *CommMem; 
/*
****************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
****************************************************************************
*/
int main(void)
{
	INT8U  err;

	/* 初始化"uC/OS-II"内核 */
	OSInit();
  
	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreateExt(AppTaskStart,	/* 启动任务函数指针 */
                    (void *)0,		/* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1], /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	/* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	/* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0],/* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE, /* 任务栈大小 */
                    (void *)0,	/* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                       （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
					/*  定义如下：
						OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
						OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
						OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
					*/                  

	/* 指定任务的名称，用于调试。这个函数是可选的 */
	OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);
	/*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/ 
	OSTimeSet(0);			 
	/* 启动多任务系统，控制权交给uC/OS-II */
	OSStart();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{		
   (void)p_arg;   /* 仅用于避免编译器告警，编译器不会产生任何目标代码 */
	 
	/* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
	BSP_Init();
	/* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率
	注意：此函数会延迟100ms再返回 */
	#if (OS_TASK_STAT_EN > 0)
		OSStatInit();
	#endif			   
	/* 创建应用程序的任务 */
	AppTaskCreate();
	

	/* 任务主体，必须是一个死循环 */
	while (1)     
	{   
		OSTimeDlyHMSM(0, 0, 1,0);										
	}      
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUIUpdate
*	功能说明: 本函数主要用于		
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
static void AppTaskGUIUpdate(void *p_arg)
{
	
	(void)p_arg;
		  
	while(1)
	{
		bsp_LedToggle(0);				
		OSTimeDlyHMSM(0, 0, 0, 500); 						  	 	       											  
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: 
*			  			  
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
    优 先 级：OS_LOWEST_PRIO - 3
*********************************************************************************************************
*/

static void AppTaskGUI(void *p_arg)
{
    (void)p_arg;		/* 避免编译器告警 */
		
	while (1) 
	{
		MainTask();				 
	}
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 此函数主要得到触摸和JoyStick的键值以便更新GUI的鼠标位置。
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
static void AppTaskUserIF(void *p_arg)
{
   (void)p_arg;	             /* 避免编译器报警 */
  
	while (1) 
	{       		
		OSTimeDlyHMSM(0, 0, 1, 5);   
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 		
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：16
*********************************************************************************************************
*/
uint8_t flag=0;
static void AppTaskCom(void *p_arg)
{
//	 INT8U err;	
	(void)p_arg;
		  

	while(1){
	
	/* 外部中断的方式容易使的鼠标跳动 暂时不考虑 */
	#if 0
	   if(flag == 1)
	   {   
		   OSSemPend(AppTouchSemp, 0, &err);
		   while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == Bit_RESET)
		   {
		   	 bsp_LedToggle(3);
	         GUI_TOUCH_Exec();
			 OSTimeDlyHMSM(0, 0, 0, 5); 
			 	 	
		   }
		   flag = 0;
		   EXTI->IMR|= (1<<6);
		   GUI_TOUCH_Exec();
		   OSTimeDlyHMSM(0, 0, 0, 20);  		   
		}
	#endif

	   GUI_TOUCH_Exec();	
	   OSTimeDlyHMSM(0, 0, 0, 20); 						  	 	       											  
   } 
   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	INT8U      err;


	/* 创建一个邮箱(MBOX), 用于Kbd任务和UserIF任务之间的通信 */
	/* 创建一个邮箱(MBOX), RTC中断和GUI之间通讯  */
	AppUserIFMbox = OSMboxCreate((void *)0);
	AppMsgMbox = OSMboxCreate((void *)0); 
	Semp      =	OSSemCreate(1);
	AppTouchSemp = OSSemCreate(0);

	
	OSTaskCreateExt(AppTaskUserIF,
                    (void *)0,
                    (OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE - 1],
                    APP_TASK_USER_IF_PRIO,
                    APP_TASK_USER_IF_PRIO,
                    (OS_STK *)&AppTaskUserIFStk[0],
                    APP_TASK_USER_IF_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_USER_IF_PRIO, APP_TASK_USER_IF_NAME, &err);

	OSTaskCreateExt(AppTaskGUI,
                    (void *)0,
                    (OS_STK *)&AppTaskGUIStk[APP_TASK_GUI_STK_SIZE - 1],
                    APP_TASK_GUI_PRIO,
                    APP_TASK_GUI_PRIO,
                    (OS_STK *)&AppTaskGUIStk[0],
                    APP_TASK_GUI_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_GUI_PRIO, APP_TASK_GUI_NAME, &err);

	OSTaskCreateExt(AppTaskCom,
                    (void *)0,
                    (OS_STK *)&AppTaskCOMStk[APP_TASK_COM_STK_SIZE-1],
                    APP_TASK_COM_PRIO,
                    APP_TASK_COM_PRIO,
                    (OS_STK *)&AppTaskCOMStk[0],
                    APP_TASK_COM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_COM_PRIO, APP_TASK_COM_NAME, &err);


	OSTaskCreateExt(AppTaskGUIUpdate,
                    (void *)0,
                    (OS_STK *)&AppTaskUpdateStk[APP_TASK_UPDATE_STK_SIZE-1],
                    APP_TASK_UPDATE_PRIO,
                    APP_TASK_UPDATE_PRIO,
                    (OS_STK *)&AppTaskUpdateStk[0],
                    APP_TASK_UPDATE_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskNameSet(APP_TASK_UPDATE_PRIO, APP_TASK_UPDATE_NAME, &err);
}

/*
*********************************************************************************************************
*                                          App_TaskCreateHook()
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    //OSProbe_TaskCreateHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                           App_TaskDelHook()
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                             App_TaskIdleHook()
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


/*
*********************************************************************************************************
*                                            TASK RETURN HOOK (APPLICATION)
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/

#if OS_VERSION >= 289
void  App_TaskReturnHook (OS_TCB  *ptcb)
{
    (void)ptcb;
}
#endif


/*
*********************************************************************************************************
*                                   App_TaskStatHook()
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Returns    : none
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                           App_TaskSwHook()
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    //OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           App_TCBInitHook()
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                               App_TimeTickHook()
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    // OSProbe_TickHook();
#endif
}
#endif

#endif                                                                  /* End of OS_APP_HOOKS_EN                                   */
