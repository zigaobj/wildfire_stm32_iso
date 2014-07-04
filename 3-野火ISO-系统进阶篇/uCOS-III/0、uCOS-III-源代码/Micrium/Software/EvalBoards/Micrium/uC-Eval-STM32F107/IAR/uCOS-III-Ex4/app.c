/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
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
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

static    OS_TCB       AppTaskStart_TCB;
static    OS_TCB       AppTaskRPM_TCB;
static    OS_TCB       AppTaskUserIF_TCB;

static    CPU_STK      AppTaskStart_Stk[APP_TASK_START_STK_SIZE];
static    CPU_STK      AppTaskRPM_Stk[APP_TASK_RPM_STK_SIZE];
static    CPU_STK      AppTaskUserIF_Stk[APP_TASK_USER_IF_STK_SIZE];

static    CPU_INT32U   AppCPU_ClkFreq_Hz;

static    CPU_BOOLEAN  AppStatResetSw;

static    CPU_FP32     AppRPM;
static    CPU_INT32U   AppRPM_Stp;
static    CPU_FP32     AppRPM_Min;
static    CPU_FP32     AppRPM_Max;
static    CPU_FP32     AppRPM_Avg;
static    CPU_INT32U   AppRPM_RevCtr;
static    CPU_INT16U   AppRPM_TmrReload_Cnts;
static    CPU_TS       AppRPM_PrevTS;
static    CPU_TS       AppRPM_TaskExecTime_uS;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         AppTaskStart       (void *p_arg);
static  void         AppTaskRPM         (void *p_arg);
static  void         AppTaskUserIF      (void *p_arg);

static  void         AppTaskCreate      (void);
static  void         AppObjCreate       (void);

static  void         AppTmrInit         (CPU_INT32U  tmr_freq);
static  void         AppTmrISR_Handler  (void);

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

    OSTaskCreate((OS_TCB     *)&AppTaskStart_TCB,               /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStart_Stk[0],
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
* Arguments   : p_arg   is the argument passed to 'AppTaskStart_()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cnts;
    OS_ERR      err;
    
    

   (void)p_arg;

    BSP_Init();                                                       /* Initialize BSP functions                          */
    CPU_Init();                                                       /* Initialize the uC/CPU services                    */

    AppCPU_ClkFreq_Hz = BSP_CPU_ClkFreq();                            /* Determine SysTick reference freq.                 */                                                                        
    cnts          = AppCPU_ClkFreq_Hz / (CPU_INT32U)OSCfg_TickRate_Hz;/* Determine nbr SysTick increments                  */
    OS_CPU_SysTickInit(cnts);                                         /* Init uC/OS periodic time src (SysTick).           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                     /* Compute CPU capacity with no task running         */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    AppObjCreate();                                                   /* Create kernel objects (semaphore, queue, etc.)    */
    AppTaskCreate();                                                  /* Create application tasks                          */

    BSP_LED_Off(0);

    while (DEF_TRUE) {                                                /* Task body, always written as an infinite loop.    */
        BSP_LED_Toggle(2);
        OSTimeDlyHMSM(0, 0, 0, 500,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}


/*
*********************************************************************************************************
*                                        CREATE APPLICATION TASKS
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



    OSTaskCreate((OS_TCB     *)&AppTaskRPM_TCB,
                 (CPU_CHAR   *)"RPM Task",
                 (OS_TASK_PTR )AppTaskRPM,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_RPM_PRIO,
                 (CPU_STK    *)&AppTaskRPM_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_RPM_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_RPM_STK_SIZE,
                 (OS_MSG_QTY  )10,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);


    OSTaskCreate((OS_TCB     *)&AppTaskUserIF_TCB,
                 (CPU_CHAR   *)"UserIFlay Task",
                 (OS_TASK_PTR )AppTaskUserIF,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_USER_IF_PRIO,
                 (CPU_STK    *)&AppTaskUserIF_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_USER_IF_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY  )10,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}


/*
*********************************************************************************************************
*                                    CREATE APPLICATION KERNEL OBJECTS
*
* Description:  This function creates the application kernel objects such as semaphore, queue, etc.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
}


/*
*********************************************************************************************************
*                                              USER_IFLAY TASK
*
* Description : This task displays the value of RPM variables
*
* Arguments   : p_arg   is the argument passed to 'AppTaskUserIF_()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskUserIF (void *p_arg)
{
    OS_ERR       err;


   (void)p_arg;

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.    */
        BSP_LED_Toggle(3);
        OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        if (AppRPM_Stp > 1000u) {                               /* Compute the reload value for the RPM timer        */
            AppRPM_TmrReload_Cnts = (CPU_INT16U)(60000000uL / AppRPM_Stp);
        } else {
            AppRPM_TmrReload_Cnts = (CPU_INT16U)60000u;
        }
        TIM1->ARR = AppRPM_TmrReload_Cnts;
        if (AppStatResetSw != DEF_FALSE) {                      /* See if we need to reset the statistics            */
            OSStatReset(&err);
            AppStatResetSw = DEF_FALSE;
        }
    }
}


/*
*********************************************************************************************************
*                                                RPM TASK
*
* Description : This task receives input capture readings from a timer configured to capture the time
*               of when the rotating device completes a revolution.  This is done through a 32-bit input 
*               capture
*
* Arguments   : p_arg   is the argument passed to 'AppTaskRPM()' by 'OSTaskCreate()', not used.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskRPM (void *p_arg)
{
    OS_ERR       err;
    CPU_INT32U   cpu_clk_freq_mhz;
    CPU_INT32U   rpm_delta_ic;
    OS_MSG_SIZE  msg_size;
    CPU_TS       ts;
    CPU_TS       ts_start;
    CPU_TS       ts_end;
    


    (void)p_arg;

    AppRPM_PrevTS    = OS_TS_GET();
    AppTmrInit(200);

    cpu_clk_freq_mhz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;

    AppRPM_RevCtr    = 0u;
    AppRPM_Max       = (CPU_FP32)0.0;
    AppRPM_Min       = (CPU_FP32)99999999.9;
    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.    */
        rpm_delta_ic = (CPU_INT32U)OSTaskQPend((OS_TICK      )OSCfg_TickRate_Hz,
                                               (OS_OPT       )OS_OPT_PEND_BLOCKING,
                                               (OS_MSG_SIZE *)&msg_size,
                                               (CPU_TS      *)&ts,
                                               (OS_ERR      *)&err);
        ts_start = OS_TS_GET();
        if (err == OS_ERR_TIMEOUT) {                            /* Timeout indicates wheel is stopped                */
            AppRPM = (CPU_FP32)0;
        } else {
            AppRPM_RevCtr++;                                    /* Count the number of revolutions                   */
            
            if (rpm_delta_ic > 0u) {                            /* Compute RPM from delta input capture counts       */
                AppRPM = (CPU_FP32)60 * (CPU_FP32)AppCPU_ClkFreq_Hz / (CPU_FP32)rpm_delta_ic;
            } else {
                AppRPM = (CPU_FP32)0;
            }
        }
        if (AppRPM > AppRPM_Max) {                              /* Detect Peak RPM                                   */
            AppRPM_Max = AppRPM;
        }
        if (AppRPM < AppRPM_Min) {                              /* Detect Minimum RPM                                */
            AppRPM_Min = AppRPM;
        }
        AppRPM_Avg             = (CPU_FP32)0.0625 * AppRPM      /* Compute Average RPM                               */
                               + (CPU_FP32)0.9375 * AppRPM_Avg;
        ts_end                 = OS_TS_GET();
        AppRPM_TaskExecTime_uS = (ts_end - ts_start)            /* Compute execution time of the RPM task            */
                               / cpu_clk_freq_mhz;
        (void)&AppRPM_TaskExecTime_uS;
        BSP_LED_Toggle(1);
    }
}


/*
*********************************************************************************************************
*                                                AppTmrInit()
*
* Description   : Initialize timer #1 to generate periodic interrupts
*
* Argument(s)   : freq         Frequency in Hertz.
*
* Return(s)     : none
*
* Caller(s)     : Application
*
* Return(s)     : none.   
*********************************************************************************************************
*/

static  void  AppTmrInit  (CPU_INT32U  tmr_freq) 
{
    TIM_TimeBaseInitTypeDef   tmr_cfg;                          
    CPU_INT32U                tmr_clk_freq;
    RCC_ClocksTypeDef         rcc_clocks;
    CPU_INT32U                reg_val;
    CPU_INT32U                prescaler;
    

    
    prescaler    = 72;

    BSP_IntVectSet((CPU_INT08U   )BSP_INT_ID_TIM1_UP,          /* Set the Interrupt vector                              */
                   (CPU_FNCT_VOID)AppTmrISR_Handler);          
   
    BSP_PeriphEn(BSP_PERIPH_ID_TIM1);                           /* Enable clock for  Timer1                             */

    RCC_GetClocksFreq(&rcc_clocks);                             /* Get the Timer Clock frequency                        */
    tmr_clk_freq = (CPU_INT32U)rcc_clocks.PCLK2_Frequency;
    
    reg_val      = (RCC->CFGR >> 11) & DEF_BIT_FIELD(3, 0);
    
    if (DEF_BIT_IS_SET(reg_val, DEF_BIT_02)) {                 /* APB2 prescaler > 1  ...  ?                            */
        tmr_clk_freq *= 2;                                     /* ... tmr_clk_freq = 2 * APB2_freq                      */
    }
    
    tmr_cfg.TIM_Prescaler          = prescaler - 1;             
    tmr_cfg.TIM_Period             = (tmr_clk_freq / tmr_freq) /* Calculate the reload value                            */
                                   / prescaler;    
    tmr_cfg.TIM_ClockDivision      = TIM_CKD_DIV1;             /* Clock division = 1                                    */
    tmr_cfg.TIM_CounterMode        = TIM_CounterMode_Down;     /* Down Mode                                             */                                       
    tmr_cfg.TIM_RepetitionCounter  = 0;

    TIM_DeInit(TIM1);                                          /* De-Initialize the Timer 1                             */
    TIM_TimeBaseInit(TIM1, &tmr_cfg);                          /* Configure the Timer1                                  */

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);                /* Clear Pending Update Interrupt                        */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                 /* Enable Update Interrupt                               */

    BSP_IntEn(BSP_INT_ID_TIM1_UP);                             /* Enable the Timer Update in the interrupt controller   */
                  
    TIM_Cmd(TIM1, ENABLE);                                     /* Start the Timer1                                      */
}


/*
*********************************************************************************************************
*                                              AppTmrISR_Handler()
*
* Description   : Timer #1 Interrupt handler
*
* Argument(s)   : none.
*
* Return(s)     : none
*
* Caller(s)     : This is an ISR
*
* Return(s)     : none.   
*********************************************************************************************************
*/

static  void  AppTmrISR_Handler (void)
{
    OS_ERR  err;
    CPU_TS  ts;
    CPU_TS  delta_ts;
    
    
    
    ts            = OS_TS_GET();                              /* Determine when the interrupt occurred ASAP             */
    delta_ts      = ts - AppRPM_PrevTS;
    AppRPM_PrevTS = ts;
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);               /* Clear the Timer #1 Update interrupt                    */
    OSTaskQPost((OS_TCB    *)&AppTaskRPM_TCB,                 /* Post simulated timer 'input capture' to RPM task       */
                (void      *)delta_ts,
                (OS_MSG_SIZE)sizeof(CPU_TS),
                (OS_OPT     )OS_OPT_POST_FIFO,
                (OS_ERR    *)&err);
}
