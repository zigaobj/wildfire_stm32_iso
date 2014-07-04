#ifndef	_APP_H_
#define	_APP_H_

#include "bsp.h" 
#include "os.h"  		
#include "os_type.h"

/*******************设置任务优先级*******************/
#define STARTUP_TASK_PRIO       3
#define	TASK_LED1_PRIO			    4	   
#define	TASK_LED2_PRIO			    5
#define	TASK_LED3_PRIO			    6 

/************设置栈大小（单位为 OS_STK ）************/
#define STARTUP_TASK_STK_SIZE   80 
#define	TASK_LED1_STK_SIZE		  80  
#define	TASK_LED2_STK_SIZE		  80
#define	TASK_LED3_STK_SIZE		  80

/**************** 用户任务声明 *******************/
void Task_Start(void *p_arg);
void Task_LED1(void *p_arg);
void Task_LED2(void *p_arg);
void Task_LED3(void *p_arg);

#endif	//_APP_H_
