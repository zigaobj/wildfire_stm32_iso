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

#define  APP_TEST_MAX   20

/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  struct  test {
    void  (*Tx)(CPU_INT08U  ix);
    void  (*Rx)(CPU_INT08U  ix);
} APP_TEST;

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

static            OS_TCB       AppTaskStartTCB;
static            OS_TCB       AppTaskRxTCB;
static            OS_TCB       AppTaskTxTCB;

static            OS_SEM       AppSem;
static            OS_Q         AppQ;
static            OS_FLAG_GRP  AppFlagGrp;
static            OS_MUTEX     AppMutex;


static            CPU_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];
static            CPU_STK      AppTaskRxStk[APP_TASK_RX_STK_SIZE];
static            CPU_STK      AppTaskTxStk[APP_TASK_TX_STK_SIZE];


static            CPU_INT08U   AppTestSel;
static  volatile  CPU_INT32U   AppTestTime_uS;
static            CPU_TS       AppTS_Delta[APP_TEST_MAX];
static            CPU_TS       AppTS_Start[APP_TEST_MAX];
static            CPU_TS       AppTS_End[APP_TEST_MAX];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         AppTaskStart       (void *p_arg);
static  void         AppTaskRx          (void *p_arg);
static  void         AppTaskTx          (void *p_arg);

static  void         AppTaskCreate      (void);
static  void         AppObjCreate       (void);


static  void         AppTestTx_Sem1     (CPU_INT08U  ix);
static  void         AppTestRx_Sem1     (CPU_INT08U  ix);
static  void         AppTestTx_Sem2     (CPU_INT08U  ix);
static  void         AppTestRx_Sem2     (CPU_INT08U  ix);

static  void         AppTestTx_TaskSem1 (CPU_INT08U  ix);
static  void         AppTestRx_TaskSem1 (CPU_INT08U  ix);
static  void         AppTestTx_TaskSem2 (CPU_INT08U  ix);
static  void         AppTestRx_TaskSem2 (CPU_INT08U  ix);

static  void         AppTestTx_Q1       (CPU_INT08U  ix);
static  void         AppTestRx_Q1       (CPU_INT08U  ix);
static  void         AppTestTx_Q2       (CPU_INT08U  ix);
static  void         AppTestRx_Q2       (CPU_INT08U  ix);

static  void         AppTestTx_TaskQ1   (CPU_INT08U  ix);
static  void         AppTestRx_TaskQ1   (CPU_INT08U  ix);
static  void         AppTestTx_TaskQ2   (CPU_INT08U  ix);
static  void         AppTestRx_TaskQ2   (CPU_INT08U  ix);

static  void         AppTestTx_Mutex1   (CPU_INT08U  ix);
static  void         AppTestRx_Mutex1   (CPU_INT08U  ix);

static  void         AppTestTx_Flag1    (CPU_INT08U  ix);
static  void         AppTestRx_Flag1    (CPU_INT08U  ix);
static  void         AppTestTx_Flag2    (CPU_INT08U  ix);
static  void         AppTestRx_Flag2    (CPU_INT08U  ix);


/*
*********************************************************************************************************
*                                             TEST TABLE
*********************************************************************************************************
*/

static  APP_TEST  AppTestTbl[] = {
    {AppTestTx_Sem1,     AppTestRx_Sem1    },     //  0
    {AppTestTx_Sem2,     AppTestRx_Sem2    },     //  1
    {AppTestTx_TaskSem1, AppTestRx_TaskSem1},     //  2
    {AppTestTx_TaskSem2, AppTestRx_TaskSem2},     //  3

    {AppTestTx_Q1,       AppTestRx_Q1      },     //  4
    {AppTestTx_Q2,       AppTestRx_Q2      },     //  5

    {AppTestTx_TaskQ1,   AppTestRx_TaskQ1  },     //  6
    {AppTestTx_TaskQ2,   AppTestRx_TaskQ2  },     //  7

    {AppTestTx_Mutex1,   AppTestRx_Mutex1  },     //  8

    {AppTestTx_Flag1,    AppTestRx_Flag1   },     //  9
    {AppTestTx_Flag2,    AppTestRx_Flag2   },     // 10
    {0,                  0                 }
};

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

    BSP_Init();                                                 /* Initialize BSP functions                          */
    CPU_Init();                                                 /* Initialize the uC/CPU services                    */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                 */                                                                        
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;/* Determine nbr SysTick increments                  */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running         */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
    AppObjCreate();                                             /* Create kernel objects (semaphore, queue, etc.)    */
    AppTaskCreate();                                            /* Create application tasks                          */

    BSP_LED_Off(0);

    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.    */
        BSP_LED_Toggle(2);
        OSTimeDlyHMSM(0, 0, 0, 100,
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



    OSTaskCreate((OS_TCB     *)&AppTaskRxTCB,
                 (CPU_CHAR   *)"Rx Task",
                 (OS_TASK_PTR )AppTaskRx,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_RX_PRIO,
                 (CPU_STK    *)&AppTaskRxStk[0],
                 (CPU_STK_SIZE)APP_TASK_RX_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_RX_STK_SIZE,
                 (OS_MSG_QTY  )10,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);


    OSTaskCreate((OS_TCB     *)&AppTaskTxTCB,
                 (CPU_CHAR   *)"Tx Task",
                 (OS_TASK_PTR )AppTaskTx,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_TX_PRIO,
                 (CPU_STK    *)&AppTaskTxStk[0],
                 (CPU_STK_SIZE)APP_TASK_TX_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_TX_STK_SIZE,
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
    OS_ERR  err;



    OSSemCreate  ((OS_SEM      *)&AppSem,
                  (CPU_CHAR    *)"App Sem",
                  (OS_SEM_CTR   )0,
                  (OS_ERR      *)&err);

    OSFlagCreate ((OS_FLAG_GRP *)&AppFlagGrp,
                  (CPU_CHAR    *)"App Flag Group",
                  (OS_FLAGS     )0,
                  (OS_ERR      *)&err);

    OSQCreate    ((OS_Q        *)&AppQ,
                  (CPU_CHAR    *)"App Queue",
                  (OS_MSG_QTY   )20,
                  (OS_ERR      *)&err);

    OSMutexCreate((OS_MUTEX    *)&AppMutex,
                  (CPU_CHAR    *)"App Mutex",
                  (OS_ERR      *)&err);
}


/*
*********************************************************************************************************
*                                                TX TASK
*
* Description : This task sends signals or messages to the Rx Task
*
* Arguments   : p_arg   is the argument passed to 'AppTaskTx()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskTx (void *p_arg)
{
    OS_ERR       err;
    OS_MSG_SIZE  msg_size;
    CPU_TS       ts;
    APP_TEST    *p_test;



   (void)p_arg;

    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.    */
        BSP_LED_Toggle(3);
        p_test = (APP_TEST *)OSTaskQPend((OS_TICK      )0,
                                         (OS_OPT       )OS_OPT_PEND_BLOCKING,
                                         (OS_MSG_SIZE *)&msg_size,
                                         (CPU_TS      *)&ts,
                                         (OS_ERR      *)&err);

        (*p_test->Tx)((CPU_INT08U)msg_size);                    
    }
}


/*
*********************************************************************************************************
*                                                RX TASK
*
* Description : This task receives signals or messages from the Tx Task
*
* Arguments   : p_arg   is the argument passed to 'AppTaskRx()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskRx (void *p_arg)
{
    OS_ERR      err;
    CPU_INT08U  i;
    APP_TEST   *p_test;
    CPU_INT32U  clk_freq_mhz;
    


   (void)p_arg;

    i            = 0;
    p_test       = &AppTestTbl[0];
    AppTestSel   = 0;
    clk_freq_mhz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;
    if (clk_freq_mhz == 0) {
        clk_freq_mhz = 1;
    }
    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.           */
        BSP_LED_Toggle(1);
        OSTimeDlyHMSM(0, 0, 0, 50,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        if ((void *)p_test->Tx != (void *)0) {
            OSTaskQPost((OS_TCB    *)&AppTaskTxTCB,
                        (void      *)p_test,
                        (OS_MSG_SIZE)i,
                        (OS_OPT     )OS_OPT_POST_FIFO,
                        (OS_ERR    *)&err);
            (*(p_test->Rx))(i);
            i++;
            p_test++;
        } else {
            i      = 0;
            p_test = &AppTestTbl[0];
        }
        AppTestTime_uS = AppTS_Delta[AppTestSel] / clk_freq_mhz;
    }
}


/*
*********************************************************************************************************
*                                         SEMAPHORE TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_Sem1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSSemPost((OS_SEM *)&AppSem,
              (OS_OPT  )OS_OPT_POST_1,
              (OS_ERR *)&err);
}


void  AppTestRx_Sem1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    CPU_TS  ts;
    
    
    OSSemPend((OS_SEM *)&AppSem,
              (OS_TICK )0,
              (OS_OPT  )OS_OPT_PEND_BLOCKING,
              (CPU_TS *)&ts,
              (OS_ERR *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}




void  AppTestTx_Sem2 (CPU_INT08U  ix)
{
}


void  AppTestRx_Sem2 (CPU_INT08U  ix)
{
    OS_ERR  err;
    CPU_TS  ts;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSSemPost((OS_SEM *)&AppSem,
              (OS_OPT  )OS_OPT_POST_1,
              (OS_ERR *)&err);
    OSSemPend((OS_SEM *)&AppSem,
              (OS_TICK )0,
              (OS_OPT  )OS_OPT_PEND_BLOCKING,
              (CPU_TS *)&ts,
              (OS_ERR *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}

/*
*********************************************************************************************************
*                                        TASK SEMAPHORE TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_TaskSem1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSTaskSemPost((OS_TCB *)&AppTaskRxTCB,
                  (OS_OPT  )OS_OPT_POST_NONE,
                  (OS_ERR *)&err);
}


void  AppTestRx_TaskSem1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    CPU_TS  ts;
    
    
    OSTaskSemPend((OS_TICK )0,
                  (OS_OPT  )OS_OPT_PEND_BLOCKING,
                  (CPU_TS *)&ts,
                  (OS_ERR *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}



void  AppTestTx_TaskSem2 (CPU_INT08U  ix)
{
}


void  AppTestRx_TaskSem2 (CPU_INT08U  ix)
{
    OS_ERR  err;
    CPU_TS  ts;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSTaskSemPost((OS_TCB *)0,
                  (OS_OPT  )OS_OPT_POST_NONE,
                  (OS_ERR *)&err);
    OSTaskSemPend((OS_TICK )0,
                  (OS_OPT  )OS_OPT_PEND_BLOCKING,
                  (CPU_TS *)&ts,
                  (OS_ERR *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}

/*
*********************************************************************************************************
*                                        MESSAGE QUEUE TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_Q1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSQPost((OS_Q      *)&AppQ,
            (void      *)1,
            (OS_MSG_SIZE)0,
            (OS_OPT     )OS_OPT_POST_FIFO,
            (OS_ERR    *)&err);
}


void  AppTestRx_Q1 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    OS_MSG_SIZE  msg_size;
    
    
    (void)OSQPend((OS_Q        *)&AppQ,
                  (OS_TICK      )0,
                  (OS_OPT       )OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE *)&msg_size,
                  (CPU_TS      *)&ts,
                  (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}



void  AppTestTx_Q2 (CPU_INT08U  ix)
{
}


void  AppTestRx_Q2 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    OS_MSG_SIZE  msg_size;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSQPost((OS_Q      *)&AppQ,
            (void      *)1,
            (OS_MSG_SIZE)0,
            (OS_OPT     )OS_OPT_POST_FIFO,
            (OS_ERR    *)&err);
    (void)OSQPend((OS_Q        *)&AppQ,
                  (OS_TICK      )0,
                  (OS_OPT       )OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE *)&msg_size,
                  (CPU_TS      *)&ts,
                  (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}

/*
*********************************************************************************************************
*                                      TASK MESSAGE QUEUE TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_TaskQ1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSTaskQPost((OS_TCB    *)&AppTaskRxTCB,
                (void      *)1,
                (OS_MSG_SIZE)0,
                (OS_OPT     )OS_OPT_POST_FIFO,
                (OS_ERR    *)&err);
}


void  AppTestRx_TaskQ1 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    OS_MSG_SIZE  msg_size;
    
    
    (void)OSTaskQPend((OS_TICK      )0,
                      (OS_OPT       )OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE *)&msg_size,
                      (CPU_TS      *)&ts,
                      (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}



void  AppTestTx_TaskQ2 (CPU_INT08U  ix)
{
}


void  AppTestRx_TaskQ2 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    OS_MSG_SIZE  msg_size;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSTaskQPost((OS_TCB    *)0,
                (void      *)1,
                (OS_MSG_SIZE)0,
                (OS_OPT     )OS_OPT_POST_FIFO,
                (OS_ERR    *)&err);
    (void)OSTaskQPend((OS_TICK      )0,
                      (OS_OPT       )OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE *)&msg_size,
                      (CPU_TS      *)&ts,
                      (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}


/*
*********************************************************************************************************
*                                   MUTUAL EXCLUSION SEMAPHORE TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_Mutex1 (CPU_INT08U  ix)
{
}


void  AppTestRx_Mutex1 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSMutexPend((OS_MUTEX *)&AppMutex,
                (OS_TICK   )0,
                (OS_OPT    )OS_OPT_PEND_BLOCKING,
                (CPU_TS   *)&ts,
                (OS_ERR   *)&err);
    OSMutexPost((OS_MUTEX *)&AppMutex,
                (OS_OPT    )OS_OPT_POST_NONE,
                (OS_ERR    *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}


/*
*********************************************************************************************************
*                                          EVENT FLAG TEST(S)
*********************************************************************************************************
*/

void  AppTestTx_Flag1 (CPU_INT08U  ix)
{
    OS_ERR  err;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSFlagPost((OS_FLAG_GRP *)&AppFlagGrp, 
               (OS_FLAGS     )0xFF, 
               (OS_OPT       )OS_OPT_POST_FLAG_SET,
               (OS_ERR      *)&err);
}


void  AppTestRx_Flag1 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    
    
    OSFlagPend((OS_FLAG_GRP *)&AppFlagGrp,
               (OS_FLAGS     )0xFF,
               (OS_TICK      )0,
               (OS_OPT       )(OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME + OS_OPT_PEND_BLOCKING),
               (CPU_TS      *)&ts,
               (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}



void  AppTestTx_Flag2 (CPU_INT08U  ix)
{
}


void  AppTestRx_Flag2 (CPU_INT08U  ix)
{
    OS_ERR       err;
    CPU_TS       ts;
    
    
    AppTS_Start[ix] = OS_TS_GET();
    OSFlagPost((OS_FLAG_GRP *)&AppFlagGrp, 
               (OS_FLAGS     )0xFF, 
               (OS_OPT       )OS_OPT_POST_FLAG_SET,
               (OS_ERR      *)&err);
    OSFlagPend((OS_FLAG_GRP *)&AppFlagGrp,
               (OS_FLAGS     )0xFF,
               (OS_TICK      )0,
               (OS_OPT       )(OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME + OS_OPT_PEND_BLOCKING),
               (CPU_TS      *)&ts,
               (OS_ERR      *)&err);
    AppTS_End[ix]   = OS_TS_GET();
    AppTS_Delta[ix] = AppTS_End[ix] - AppTS_Start[ix];
}
