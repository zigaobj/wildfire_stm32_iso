/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JJL
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB          AppTaskStartTCB; 
static  OS_TCB          AppTaskTempSensorTCB; 

static  BSP_STLM75_CFG  AppLM75_Cfg;
static  CPU_INT16S      AppTempSensor;           /* Temperature at the sensor                          */
static  CPU_BOOLEAN     AppTempSensorOverTemp;

static  CPU_INT16S      AppTempSensorDeg;
static  CPU_BOOLEAN     AppDegF_DegC_Sel;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK         AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK         AppTaskTempSensorStk[APP_TASK_TEMP_SENSOR_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate     (void);
static  void  AppTaskStart      (void *p_arg);

static  void  AppTaskTempSensor (void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSSchedRoundRobinCfg((CPU_BOOLEAN)DEF_TRUE, 
                         (OS_TICK    )10,
                         (OS_ERR    *)&err);
    
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    
    

   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                                 */

    CPU_Init();

    cpu_clk_freq = BSP_CPU_ClkFreq();
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;/* Determine nbr SysTick increments                         */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running                */
#endif

    CPU_IntDisMeasMaxCurReset();
    
    AppTaskCreate();                                            /* Create application tasks                                 */

    BSP_LED_Off(0);
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */
        BSP_LED_Toggle(1);
        OSTimeDlyHMSM(0, 0, 0, 250, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  err;
    

    
    OSTaskCreate((OS_TCB     *)&AppTaskTempSensorTCB, 
                 (CPU_CHAR   *)"App Temp Sensor Start",
                 (OS_TASK_PTR )AppTaskTempSensor, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_TEMP_SENSOR_PRIO,
                 (CPU_STK    *)&AppTaskTempSensorStk[0],
                 (CPU_STK_SIZE)APP_TASK_TEMP_SENSOR_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_TEMP_SENSOR_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}


/*
*********************************************************************************************************
*                                          AppTaskTempSensor()
*
* Description : Monitors the STLM75 Sensor.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskTempSensor()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a Task.
*
* Note(s)     : none.
*********************************************************************************************************
*/


static  void  AppTaskTempSensor  (void *p_arg)
{
    OS_ERR      err;
    

    BSP_STLM75_Init();                                                    /* Initlize the STLM75 Sensor              */
                                                                          
    AppLM75_Cfg.FaultLevel    = (CPU_INT08U )BSP_STLM75_FAULT_LEVEL_1;    /* Configure The sensor                    */
    AppLM75_Cfg.HystTemp      = (CPU_INT16S ) 1;
    AppLM75_Cfg.IntPol        = (CPU_BOOLEAN)BSP_STLM75_INT_POL_HIGH;
    AppLM75_Cfg.Mode          = (CPU_BOOLEAN)BSP_STLM75_MODE_INTERRUPT;
    AppLM75_Cfg.OverLimitTemp = (CPU_INT16S )88;

    while (DEF_TRUE) {                                                    /*  Sense the temperature every 100 ms     */
        BSP_LED_Toggle(3);
        BSP_STLM75_TempGet(BSP_STLM75_TEMP_UNIT_FAHRENHEIT, 
                           &AppTempSensor);
        if (AppDegF_DegC_Sel == 0) {
            AppTempSensorDeg = AppTempSensor;
        } else {
            AppTempSensorDeg = AppTempSensor * 5 / 8 - 32;
        }
        (void)&AppTempSensorDeg;
        BSP_STLM75_CfgSet(&AppLM75_Cfg);
        AppTempSensorOverTemp = BSP_StatusRd(1);
        (void)&AppTempSensorOverTemp;
        OSTimeDlyHMSM(0, 0, 0, 500,
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}
        
