;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           Generic ARM Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.86
; By        : Jean J. Labrosse
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning                                           ; External references
    EXTERN  OSPrioCur											;本应用所用到的声明   
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSIntNesting
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook


    EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    EXPORT  OS_CPU_SR_Restore								    ;输出外部声明 
    EXPORT  OSStartHighRdy
    EXPORT  OSCtxSw
    EXPORT  OSIntCtxSw
    EXPORT  OS_CPU_PendSVHandler

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.      中断控制及状态寄存器ICSR的地址
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).PendSV优先级寄存器的地址
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).		 PendSV中断的优先级是255（最低）系统优先级寄存器
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.	 相应的位28置一，触发PendSV中断
																;

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;********************************************************************************************************

OS_CPU_SR_Save
    MRS     R0, PRIMASK                                         ;读取PRIMASK到R0，R0是返回值 Set prio int mask to mask all (except faults)
    CPSID   I												    ;PRIMASK=1，关中断（NMI和硬fault可以响应）
    BX      LR													;R14 返回

OS_CPU_SR_Restore
    MSR     PRIMASK, R0											;读取R0到PRIMASK中，R0为参数
    BX      LR													;返回

;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set OSRunning to TRUE;
;              d) Trigger PendSV exception;
;              e) Enable interrupts (tasks will run with interrupts enabled).
;    OSStartHighRdy()由OSStart()调用，用来启动最高优先级任务，当然任务必须在OSStart()前已被创建
;	 #1.PendSV中断的优先级应该为最低优先级，原因在<<ARM Cortex-M3权威指南>>的7.6节已有说明
;    #2.PSP设置为0，是告诉具体的任务切换程序（OS_CPU_PendSVHandler()），这是第一次任务切换。做过切换后PSP就不会为0了，后面会看到
;    #3.往中断控制及状态寄存器ICSR(0xE000ED04)第28位写1即可产生PendSV中断。这个<<ARM Cortex-M3权威指南>>8.4.5 其它异常的配置寄存器有说明。		
;********************************************************************************************************

OSStartHighRdy													;设置PendSV中断的优先级
    LDR     R0, =NVIC_SYSPRI14                                  ;Set the PendSV exception priority
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]											;*(uint8_t *)NVIC_SYSPRI14 = NVIC_PENDSV_PRI

    MOVS    R0, #0                                              ;初始化进程堆栈指针  Set the PSP to 0 for initial context switch call
    MSR     PSP, R0												;初始化PSP为0        初始化上下文切换调用

    LDR     R0, =OSRunning                                      ; OSRunning = TRUE
    MOVS    R1, #1												;设置OSRunning = TRUE
    STRB    R1, [R0]

    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET									;触发PendSV中断
    STR     R1, [R0]											;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET


    CPSIE   I                                                   ; Enable interrupts at processor level开启中断

OSStartHang
    B       OSStartHang                                         ; Should never get here 死循环 which（1）；


;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
; 当一个任务放弃cpu的使用权，就会调用OS_TASK_SW()宏，而OS_TASK_SW()就是OSCtxSw()。OSCtxSw()应该做任务切换。
; 但是在CM3中，所有任务切换都被放到PendSV的中断处理函数中去做了，因此OSCtxSw()只需简单的触发PendSV中断即可。
; OS_TASK_SW()是由OS_Sched()调用。
;********************************************************************************************************

OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]											;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET
    BX      LR													;返回

;********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;看到这里有些同学可能奇怪怎么OSCtxSw()和OSIntCtxSw()完全一样，事实上，这两个函数的意义是不一样的，OSCtxSw()
;做的是任务之间的切换，如任务A因为等待某个资源或是做延时切换到任务B，而OSIntCtxSw()则是中断退出时，由中断状
;态切换到另一个任务。由中断切换到任务时，CPU寄存器入栈的工作已经做完了，所以无需做第二次了（参考邵老师书的3.10节）。
;这里只不过由于CM3的特殊机制导致了在这两个函数中只要做触发PendSV中断即可，具体切换由PendSV中断来处理。
;********************************************************************************************************

OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;              前面已经说过真正的任务切换是在PendSV中断处理函数里做的，由于CM3在中断时会有一半的寄存器自动保存到任务堆栈里，
;              所以在PendSV中断处理函数中只需保存R4-R11并调节堆栈指针即可
;//* 描    述 : 在cm3内核下,真正的任务文本切换是靠本函数实现  CM3中，栈是由高地址向低地址增长的
;//                 |     ....        |
;//                 |-----------------|
;//                 |     ....        |
;//                 |-----------------|
;//                 |     ....        |
;//                 |-----------------|       |---- 任务切换时PSP
;//     Low Memory  |     ....        |       |
;//                 |-----------------|       |    |---------------|      |----------------|
;//        ^        |       R4        |  <----|----|--OSTCBStkPtr  |<-----|   (OS_TCB *)   |
;//        ^        |-----------------|            |---------------|      |----------------|
;//        ^        |       R5        |            |               |         OSTCBHighRdy
;//        |        |-----------------|            |---------------|
;//        |        |       R6        |            |               |
;//        |        |-----------------|            |---------------|
;//        |        |       R7        |            |               |
;//        |        |-----------------|            |---------------|
;//        |        |       R8        |                 Task's
;//        |        |-----------------|                 OS_TCB
;//        |        |       R9        |
;//        |        |-----------------|
;//        |        |      R10        |
;//      Stack      |-----------------|
;//      Growth     |      R11        |
;//       = 1       |-----------------|
;//        |        |    R0 = p_arg   |  <-------- 异常时的PSP (向下生长的满栈)
;//        |        |-----------------|
;//        |        |       R1        |
;//        |        |-----------------|
;//        |        |       R2        |
;//        |        |-----------------|
;//        |        |       R3        |
;//        |        |-----------------|
;//        |        |       R12       |
;//        |        |-----------------|
;//        |        |       LR        |
;//        |        |-----------------|
;//        |        |    PC = task    |
;//        |        |-----------------|
;//        |        |      xPSR       |
;//    High Memory  |-----------------|
;//   
;********************************************************************************************************

OS_CPU_PendSVHandler											;xPSR, PC, LR, R12, R0-R3已自动保存
    CPSID   I                                                   ;任务切换期间需要关中断 Prevent interruption during context switch
    MRS     R0, PSP                                             ;R0 = PSP PSP is process stack pointer 线程堆栈指针
    CBZ     R0, OS_CPU_PendSVHandler_nosave                     ;如果PSP==0跳转到OS_CPU_PendSVHandler_nosave去执行 在多任务的初始化时PSP被初始化为0  Skip register save the first time
																;若果PSP如果是0,标示任务没有运行过,那么不需要压栈

    SUBS    R0, R0, #0x20                                       ;R0 -= 0x20 保存R4-R11到任务堆栈 共32个字节		Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}										;压栈R4-R11, 其他8个寄存器是在异常时自动压栈的
    LDR     R1, =OSTCBCur                                       ;获取OSTCBCur->OSTCBStkPtr   				R1 = &OSTCBCur    OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]											;R1 = *R1 (R1 = OSTCBCur) 
    STR     R0, [R1]                                            ;*R1 = R0 (*OSTCBCur = SP) R0 is SP of process being switched out
																;将当前任务的堆栈保存到自己的任务控制块
																;OSTCBCur->OSTCBStkPtr = PSP
																;程序运行此位置,已经保存了当前任务的context了


                                                                ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave
    PUSH    {R14}                                               ; Save LR exc_return value 保存R14，因为后面要调用函数  
    LDR     R0, =OSTaskSwHook                                   ; OSTaskSwHook();		   R0 = &OSTaskSwHook 
    BLX     R0													; 调用OSTaskSwHook()
    POP     {R14}												; 恢复R14

    LDR     R0, =OSPrioCur                                      ; OSPrioCur = OSPrioHighRdy; R0 = &OSPrioCur
    LDR     R1, =OSPrioHighRdy									; R1 = &OSPrioHighRdy
    LDRB    R2, [R1]											; R2 = *R1 (R2 = OSPrioHighRdy)
    STRB    R2, [R0]											; *R0 = R2 (OSPrioCur = OSPrioHighRdy)

    LDR     R0, =OSTCBCur                                       ; OSTCBCur  = OSTCBHighRdy;;R0 = &OSTCBCur
    LDR     R1, =OSTCBHighRdy									; R1 = &OSTCBHighRdy
    LDR     R2, [R1]											; R2 = *R1 (R2 = OSTCBHighRdy)
    STR     R2, [R0]											; *R0 = R2 (OSTCBCur = OSTCBHighRdy) 此时 [R2] = 新任务的PSP

    LDR     R0, [R2]                                            ;;R0 = *R2 (R0 = OSTCBHighRdy), 此时R0是新任务的SP R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; 从任务堆栈SP恢复R4-R11 Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20										; 调整PSP  R0 += 0x20
    MSR     PSP, R0                                             ; Load PSP with new process SP  PSP = R0，    用新任务的SP加载PSP 
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack  确保LR位2为1，返回后使用进程堆栈PSP
    CPSIE   I													;开中断
    BX      LR                                                  ; Exception return will restore remaining context  中断返回

    END
