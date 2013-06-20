/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                             CORE FUNCTIONS
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : OS_CORE.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#define  OS_GLOBALS
#include "includes.h"
#endif

/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
                                       
static  INT8U        OSIntExitY;               /* Variable used by 'OSIntExit' to prevent using locals */

static  OS_STK       OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];       /* Idle       task stack              */

#if     OS_TASK_STAT_EN
static  OS_STK       OSTaskStatStk[OS_TASK_STAT_STK_SIZE];      /* Statistics task stack               */
#endif

static  OS_TCB       OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];   /* Table of TCBs                       */

/*$PAGE*/
/*
*********************************************************************************************************
*                              MAPPING TABLE TO MAP BIT POSITION TO BIT MASK
*
* Note: Index into table is desired bit position, 0..7
*       Indexed value corresponds to bit mask
*********************************************************************************************************
*/

INT8U const OSMapTbl[]   = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*********************************************************************************************************
*/

INT8U const OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/*$PAGE*/
/*
*********************************************************************************************************
*                             MAKE TASK READY TO RUN BASED ON EVENT OCCURING
*
* Description: This function is called by other uC/OS-II services and is used to ready a task that was
*              waiting for an event to occur.
*
* Arguments  : pevent    is a pointer to the event control block corresponding to the event.
*
*              msg       is a pointer to a message.  This pointer is used by message oriented services
*                        such as MAILBOXEs and QUEUEs.  The pointer is not used when called by other
*                        service functions.
*
*              msk       is a mask that is used to clear the status byte of the TCB.  For example,
*                        OSSemPost() will pass OS_STAT_SEM, OSMboxPost() will pass OS_STAT_MBOX etc.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if  (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
void  OSEventTaskRdy (OS_EVENT *pevent, void *msg, INT8U msk)
{
    OS_TCB *ptcb;
    INT8U   x;
    INT8U   y;
    INT8U   bitx;
    INT8U   bity;
    INT8U   prio;


    y    = OSUnMapTbl[pevent->OSEventGrp];            /* Find highest prio. task waiting for message   */
    bity = OSMapTbl[y];
    x    = OSUnMapTbl[pevent->OSEventTbl[y]];
    bitx = OSMapTbl[x];
    prio = (INT8U)((y << 3) + x);                     /* Find priority of task getting the msg         */
    if ((pevent->OSEventTbl[y] &= ~bitx) == 0) {      /* Remove this task from the waiting list        */
        pevent->OSEventGrp &= ~bity;
    }
    ptcb                 =  OSTCBPrioTbl[prio];       /* Point to this task's OS_TCB                   */
    ptcb->OSTCBDly       =  0;                        /* Prevent OSTimeTick() from readying task       */
    ptcb->OSTCBEventPtr  = (OS_EVENT *)0;             /* Unlink ECB from this task                     */
#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN
    ptcb->OSTCBMsg       = msg;                       /* Send message directly to waiting task         */
#else
    msg                  = msg;                       /* Prevent compiler warning if not used          */    
#endif    
    ptcb->OSTCBStat     &= ~msk;                      /* Clear bit associated with event type          */
    if (ptcb->OSTCBStat == OS_STAT_RDY) {             /* See if task is ready (could be susp'd)        */
        OSRdyGrp        |=  bity;                     /* Put task in the ready to run list             */
        OSRdyTbl[y]     |=  bitx;
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                   MAKE TASK WAIT FOR EVENT TO OCCUR
*
* Description: This function is called by other uC/OS-II services to suspend a task because an event has
*              not occurred.
*
* Arguments  : pevent   is a pointer to the event control block for which the task will be waiting for.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if  (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
void  OSEventTaskWait (OS_EVENT *pevent)
{
    OSTCBCur->OSTCBEventPtr = pevent;            /* Store pointer to event control block in TCB        */
    if ((OSRdyTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0) {      /* Task no longer ready      */
        OSRdyGrp &= ~OSTCBCur->OSTCBBitY;
    }
    pevent->OSEventTbl[OSTCBCur->OSTCBY] |= OSTCBCur->OSTCBBitX;          /* Put task in waiting list  */
    pevent->OSEventGrp                   |= OSTCBCur->OSTCBBitY;
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                              MAKE TASK READY TO RUN BASED ON EVENT TIMEOUT
*
* Description: This function is called by other uC/OS-II services to make a task ready to run because a
*              timeout occurred.
*
* Arguments  : pevent   is a pointer to the event control block which is readying a task.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if  (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
void  OSEventTO (OS_EVENT *pevent)
{
    if ((pevent->OSEventTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0) {
        pevent->OSEventGrp &= ~OSTCBCur->OSTCBBitY;
    }
    OSTCBCur->OSTCBStat     = OS_STAT_RDY;       /* Set status to ready                                */
    OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;     /* No longer waiting for event                        */
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                 INITIALIZE EVENT CONTROL BLOCK'S WAIT LIST
*
* Description: This function is called by other uC/OS-II services to initialize the event wait list.
*
* Arguments  : pevent    is a pointer to the event control block allocated to the event.
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/
#if  (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
void  OSEventWaitListInit (OS_EVENT *pevent)
{
    INT8U i;
    
    
    pevent->OSEventGrp = 0x00;                   /* No task waiting on event                           */
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++) {
        pevent->OSEventTbl[i] = 0x00;
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-II and MUST be called prior to
*              creating any uC/OS-II object and, prior to calling OSStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

void OSInit (void)
{
    INT16U i;


    OSTime        = 0L;                                    /* Clear the 32-bit system clock            */
    OSIntNesting  = 0;                                     /* Clear the interrupt nesting counter      */
    OSLockNesting = 0;                                     /* Clear the scheduling lock counter        */
#if OS_TASK_CREATE_EN  || OS_TASK_CREATE_EXT_EN || OS_TASK_DEL_EN
    OSTaskCtr     = 0;                                     /* Clear the number of tasks                */
#endif
    OSRunning     = FALSE;                                 /* Indicate that multitasking not started   */
    OSIdleCtr     = 0L;                                    /* Clear the 32-bit idle counter            */
#if OS_TASK_STAT_EN && OS_TASK_CREATE_EXT_EN
    OSIdleCtrRun  = 0L;
    OSIdleCtrMax  = 0L;
    OSStatRdy     = FALSE;                                 /* Statistic task is not ready              */
#endif
    OSCtxSwCtr    = 0;                                     /* Clear the context switch counter         */
    OSRdyGrp      = 0;                                     /* Clear the ready list                     */
    for (i = 0; i < OS_RDY_TBL_SIZE; i++) {
        OSRdyTbl[i] = 0;
    }
                
    OSPrioCur     = 0;
    OSPrioHighRdy = 0;                                           
    OSTCBHighRdy  = (OS_TCB *)0;                                 /* TCB Initialization                 */
    OSTCBCur      = (OS_TCB *)0;
    OSTCBList     = (OS_TCB *)0;
    for (i = 0; i < (OS_LOWEST_PRIO + 1); i++) {                 /* Clear the priority table           */
        OSTCBPrioTbl[i] = (OS_TCB *)0;
    }
    for (i = 0; i < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1); i++) {  /* Init. list of free TCBs            */
        OSTCBTbl[i].OSTCBNext = &OSTCBTbl[i + 1];
    }
    OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS - 1].OSTCBNext = (OS_TCB *)0;    /* Last OS_TCB             */
    OSTCBFreeList                                         = &OSTCBTbl[0];

#if OS_MAX_EVENTS >= 2
    for (i = 0; i < (OS_MAX_EVENTS - 1); i++) {            /* Init. list of free EVENT control blocks  */
        OSEventTbl[i].OSEventPtr = (OS_EVENT *)&OSEventTbl[i + 1];
    }
    OSEventTbl[OS_MAX_EVENTS - 1].OSEventPtr = (OS_EVENT *)0;
    OSEventFreeList                          = &OSEventTbl[0];    
#endif

#if OS_Q_EN && (OS_MAX_QS >= 2)
    OSQInit();                                             /* Initialize the message queue structures  */
#endif

#if OS_MEM_EN && OS_MAX_MEM_PART >= 2
    OSMemInit();                                           /* Initialize the memory manager            */
#endif    

#if OS_STK_GROWTH == 1
    #if OS_TASK_CREATE_EXT_EN
    OSTaskCreateExt(OSTaskIdle, 
                    (void *)0,                                 /* No arguments passed to OSTaskIdle()  */
                    &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Top-Of-Stack                     */
                    OS_IDLE_PRIO,                              /* Lowest priority level                */
                    OS_TASK_IDLE_ID,
                    &OSTaskIdleStk[0],                         /* Set Bottom-Of-Stack                  */
                    OS_TASK_IDLE_STK_SIZE, 
                    (void *)0,                                 /* No TCB extension                     */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #else
    OSTaskCreate(OSTaskIdle, (void *)0, &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], OS_IDLE_PRIO);
    #endif
#else
    #if OS_TASK_CREATE_EXT_EN
    OSTaskCreateExt(OSTaskIdle, 
                    (void *)0,                                 /* No arguments passed to OSTaskIdle()  */
                    &OSTaskIdleStk[0],                         /* Set Top-Of-Stack                     */
                    OS_IDLE_PRIO,                              /* Lowest priority level                */
                    OS_TASK_IDLE_ID,
                    &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE - 1], /* Set Bottom-Of-Stack                  */
                    OS_TASK_IDLE_STK_SIZE, 
                    (void *)0,                                 /* No TCB extension                     */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
    #else
    OSTaskCreate(OSTaskIdle, (void *)0, &OSTaskIdleStk[0], OS_IDLE_PRIO);
    #endif
#endif

#if OS_TASK_STAT_EN 
    #if OS_TASK_CREATE_EXT_EN
        #if OS_STK_GROWTH == 1
        OSTaskCreateExt(OSTaskStat, 
                        (void *)0,                                /* No args passed to OSTaskStat()    */
                        &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],/* Set Top-Of-Stack                  */
                        OS_STAT_PRIO,                             /* One higher than the idle task     */
                        OS_TASK_STAT_ID,
                        &OSTaskStatStk[0],                        /* Set Bottom-Of-Stack               */
                        OS_TASK_STAT_STK_SIZE, 
                        (void *)0,                                /* No TCB extension                  */
                        OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
        #else
        OSTaskCreateExt(OSTaskStat, 
                        (void *)0,                                /* No args passed to OSTaskStat()    */
                        &OSTaskStatStk[0],                        /* Set Top-Of-Stack                  */
                        OS_STAT_PRIO,                             /* One higher than the idle task     */
                        OS_TASK_STAT_ID,
                        &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],/* Set Bottom-Of-Stack               */
                        OS_TASK_STAT_STK_SIZE, 
                        (void *)0,                                /* No TCB extension                  */
                        OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear  */
        #endif
    #else
        #if OS_STK_GROWTH == 1
        OSTaskCreate(OSTaskStat, 
                     (void *)0,                                   /* No args passed to OSTaskStat()    */
                     &OSTaskStatStk[OS_TASK_STAT_STK_SIZE - 1],   /* Set Top-Of-Stack                  */
                     OS_STAT_PRIO);                               /* One higher than the idle task     */
        #else
        OSTaskCreate(OSTaskStat, 
                     (void *)0,                                   /* No args passed to OSTaskStat()    */
                     &OSTaskStatStk[0],                           /* Set Top-Of-Stack                  */
                     OS_STAT_PRIO);                               /* One higher than the idle task     */
        #endif
    #endif
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              ENTER ISR
*
* Description: This function is used to notify uC/OS-II that you are about to service an interrupt
*              service routine (ISR).  This allows uC/OS-II to keep track of interrupt nesting and thus
*              only perform rescheduling at the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) Your ISR can directly increment OSIntNesting without calling this function because 
*                 OSIntNesting has been declared 'global'.  You MUST, however, be sure that the increment
*                 is performed 'indivisibly' by your processor to ensure proper access to this critical
*                 resource.
*              2) You MUST still call OSIntExit() even though you increment OSIntNesting directly.
*              3) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*********************************************************************************************************
*/

void OSIntEnter (void)
{
    OS_ENTER_CRITICAL();
    OSIntNesting++;                              /* Increment ISR nesting level                        */
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               EXIT ISR
*
* Description: This function is used to notify uC/OS-II that you have completed serviving an ISR.  When 
*              the last nested ISR has completed, uC/OS-II will call the scheduler to determine whether
*              a new, high-priority task, is ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call
*                 to OSIntEnter() at the beginning of the ISR you MUST have a call to OSIntExit() at the
*                 end of the ISR.
*              2) Rescheduling is prevented when the scheduler is locked (see OSSchedLock())
*********************************************************************************************************
*/

void OSIntExit (void)
{
    OS_ENTER_CRITICAL();
    if ((--OSIntNesting | OSLockNesting) == 0) { /* Reschedule only if all ISRs completed & not locked */
        OSIntExitY    = OSUnMapTbl[OSRdyGrp];
        OSPrioHighRdy = (INT8U)((OSIntExitY << 3) + OSUnMapTbl[OSRdyTbl[OSIntExitY]]);
        if (OSPrioHighRdy != OSPrioCur) {        /* No context switch if current task is highest ready */
            OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy];
            OSCtxSwCtr++;                        /* Keep track of the number of context switches       */
            OSIntCtxSw();                        /* Perform interrupt level context switch             */
        }
    }
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              SCHEDULER
*
* Description: This function is called by other uC/OS-II services to determine whether a new, high
*              priority task has been made ready to run.  This function is invoked by TASK level code
*              and is not used to reschedule tasks from ISRs (see OSIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Rescheduling is prevented when the scheduler is locked (see OSSchedLock())
*********************************************************************************************************
*/

void OSSched (void)
{
    INT8U y;


    OS_ENTER_CRITICAL();
    if ((OSLockNesting | OSIntNesting) == 0) {   /* Task scheduling must be enabled and not ISR level  */
        y             = OSUnMapTbl[OSRdyGrp];    /* Get pointer to highest priority task ready to run  */
        OSPrioHighRdy = (INT8U)((y << 3) + OSUnMapTbl[OSRdyTbl[y]]);
        if (OSPrioHighRdy != OSPrioCur) {         /* No context switch if current task is highest ready */
            OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
            OSCtxSwCtr++;                        /* Increment context switch counter                   */
            OS_TASK_SW();                        /* Perform a context switch                           */
        }
    }
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling to take place.  This allows your application
*              to prevent context switches until you are ready to permit context switching.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every 
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

void OSSchedLock (void)
{
    if (OSRunning == TRUE) {                     /* Make sure multitasking is running                  */
        OS_ENTER_CRITICAL();
        OSLockNesting++;                         /* Increment lock nesting level                       */
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.  
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every 
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
*********************************************************************************************************
*/

void OSSchedUnlock (void)
{
    if (OSRunning == TRUE) {                           /* Make sure multitasking is running            */
        OS_ENTER_CRITICAL();
        if (OSLockNesting > 0) {                       /* Do not decrement if already 0                */
            OSLockNesting--;                           /* Decrement lock nesting level                 */
            if ((OSLockNesting | OSIntNesting) == 0) { /* See if scheduling re-enabled and not an ISR  */
                OS_EXIT_CRITICAL();
                OSSched();                             /* See if a higher priority task is ready       */
            } else {
                OS_EXIT_CRITICAL();
            }
        } else {
            OS_EXIT_CRITICAL();
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-II manages the
*              task that you have created.  Before you can call OSStart(), you MUST have called OSInit()
*              and you MUST have created at least one task.
*
* Arguments  : none
*
* Returns    : none
*
* Note       : OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Set OSRunning to TRUE.
*********************************************************************************************************
*/

void OSStart (void)
{
    INT8U y;
    INT8U x;


    if (OSRunning == FALSE) {
        y             = OSUnMapTbl[OSRdyGrp];        /* Find highest priority's task priority number   */
        x             = OSUnMapTbl[OSRdyTbl[y]];
        OSPrioHighRdy = (INT8U)((y << 3) + x);
        OSPrioCur     = OSPrioHighRdy;
        OSTCBHighRdy  = OSTCBPrioTbl[OSPrioHighRdy]; /* Point to highest priority task ready to run    */
        OSTCBCur      = OSTCBHighRdy;
        OSStartHighRdy();                            /* Execute target specific code to start task     */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        STATISTICS INITIALIZATION
*
* Description: This function is called by your application to establish CPU usage by first determining
*              how high a 32-bit counter would count to in 1 second if no other tasks were to execute
*              during that time.  CPU usage is then determined by a low priority task which keeps track
*              of this 32-bit counter every second but this time, with other tasks running.  CPU usage is
*              determined by:
*
*                                             OSIdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------)
*                                            OSIdleCtrMax
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN
void OSStatInit (void)
{
    OSTimeDly(2);                                /* Synchronize with clock tick                        */
    OS_ENTER_CRITICAL();
    OSIdleCtr    = 0L;                           /* Clear idle counter                                 */
    OS_EXIT_CRITICAL();
    OSTimeDly(OS_TICKS_PER_SEC);                 /* Determine MAX. idle counter value for 1 second     */
    OS_ENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCtr;                    /* Store maximum idle counter count in 1 second       */
    OSStatRdy    = TRUE;
    OS_EXIT_CRITICAL();
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                              IDLE TASK
*
* Description: This task is internal to uC/OS-II and executes whenever no other higher priority tasks
*              executes because they are waiting for event(s) to occur.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

void OSTaskIdle (void *pdata)
{
    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                            STATISTICS TASK
*
* Description: This task is internal to uC/OS-II and is used to compute some statistics about the
*              multitasking environment.  Specifically, OSTaskStat() computes the CPU usage.
*              CPU usage is determined by:
*
*                                          OSIdleCtr
*                 OSCPUUsage = 100 * (1 - ------------)     (units are in %)
*                                         OSIdleCtrMax
*
* Arguments  : pdata     this pointer is not used at this time.
*
* Returns    : none
*
* Notes      : 1) This task runs at a priority level higher than the idle task.  In fact, it runs at the
*                 next higher priority, OS_IDLE_PRIO-1.
*              2) You can disable this task by setting the configuration #define OS_TASK_STAT_EN to 0.
*              3) We delay for 5 seconds in the beginning to allow the system to reach steady state and
*                 have all other tasks created before we do statistics.  You MUST have at least a delay
*                 of 2 seconds to allow for the system to establish the maximum value for the idle 
*                 counter.
*********************************************************************************************************
*/

#if OS_TASK_STAT_EN
void OSTaskStat (void *pdata)
{
    INT32U run;
    INT8S  usage;
    
    
    pdata = pdata;                               /* Prevent compiler warning for not using 'pdata'     */
    while (OSStatRdy == FALSE) {
        OSTimeDly(2 * OS_TICKS_PER_SEC);             /* Wait until statistic task is ready                 */
    }
    for (;;) {
        OS_ENTER_CRITICAL();
        OSIdleCtrRun = OSIdleCtr;                /* Obtain the of the idle counter for the past second */
        run          = OSIdleCtr;
        OSIdleCtr    = 0L;                       /* Reset the idle counter for the next second         */
        OS_EXIT_CRITICAL();
        if (OSIdleCtrMax > 0L) {
            usage = (INT8S)(100L - 100L * run / OSIdleCtrMax);
            if (usage > 100) {
                OSCPUUsage = 100;
            } else if (usage < 0) {
                OSCPUUsage =   0;
            } else {
                OSCPUUsage = usage;
            }
        } else {
            OSCPUUsage = 0;
        }
        OSTaskStatHook();                        /* Invoke user definable hook                         */
        OSTimeDly(OS_TICKS_PER_SEC);             /* Accumulate OSIdleCtr for the next second           */
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*
* Description: This function is internal to uC/OS-II and is used to initialize a Task Control Block when
*              a task is created (see OSTaskCreate() and OSTaskCreateExt()).
*
* Arguments  : prio          is the priority of the task being created
*
*              ptos          is a pointer to the task's top-of-stack assuming that the CPU registers
*                            have been placed on the stack.  Note that the top-of-stack corresponds to a 
*                            'high' memory location is OS_STK_GROWTH is set to 1 and a 'low' memory
*                            location if OS_STK_GROWTH is set to 0.  Note that stack growth is CPU
*                            specific.
*
*              pbos          is a pointer to the bottom of stack.  A NULL pointer is passed if called by
*                            'OSTaskCreate()'.
*
*              id            is the task's ID (0..65535)
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'OSTaskCreate()'.
*
*              pext          is a pointer to a user supplied memory area that is used to extend the task
*                            control block.  This allows you to store the contents of floating-point
*                            registers, MMU registers or anything else you could find useful during a 
*                            context switch.  You can even assign a name to each task and store this name
*                            in this TCB extension.  A NULL pointer is passed if called by OSTaskCreate().
*
*              opt           options as passed to 'OSTaskCreateExt()' or, 
*                            0 if called from 'OSTaskCreate()'.
*
* Returns    : OS_NO_ERR         if the call was successful
*              OS_NO_MORE_TCB    if there are no more free TCBs to be allocated and thus, the task cannot
*                                be created.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

INT8U OSTCBInit (INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT16U stk_size, void *pext, INT16U opt)
{
    OS_TCB *ptcb;


    OS_ENTER_CRITICAL();
    ptcb = OSTCBFreeList;                                  /* Get a free TCB from the free TCB list    */
    if (ptcb != (OS_TCB *)0) {
        OSTCBFreeList        = ptcb->OSTCBNext;            /* Update pointer to free TCB list          */
        OS_EXIT_CRITICAL();
        ptcb->OSTCBStkPtr    = ptos;                       /* Load Stack pointer in TCB                */
        ptcb->OSTCBPrio      = (INT8U)prio;                /* Load task priority into TCB              */
        ptcb->OSTCBStat      = OS_STAT_RDY;                /* Task is ready to run                     */
        ptcb->OSTCBDly       = 0;                          /* Task is not delayed                      */

#if OS_TASK_CREATE_EXT_EN        
        ptcb->OSTCBExtPtr    = pext;                       /* Store pointer to TCB extension           */
        ptcb->OSTCBStkSize   = stk_size;                   /* Store stack size                         */
        ptcb->OSTCBStkBottom = pbos;                       /* Store pointer to bottom of stack         */
        ptcb->OSTCBOpt       = opt;                        /* Store task options                       */
        ptcb->OSTCBId        = id;                         /* Store task ID                            */
#else
        pext                 = pext;                       /* Prevent compiler warning if not used     */
        stk_size             = stk_size;
        pbos                 = pbos;
        opt                  = opt;
        id                   = id;
#endif

#if OS_TASK_DEL_EN        
        ptcb->OSTCBDelReq    = OS_NO_ERR;
#endif

        ptcb->OSTCBY         = prio >> 3;                  /* Pre-compute X, Y, BitX and BitY          */
        ptcb->OSTCBBitY      = OSMapTbl[ptcb->OSTCBY];
        ptcb->OSTCBX         = prio & 0x07;
        ptcb->OSTCBBitX      = OSMapTbl[ptcb->OSTCBX];

#if     OS_MBOX_EN || (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_SEM_EN
        ptcb->OSTCBEventPtr  = (OS_EVENT *)0;              /* Task is not pending on an event          */
#endif

#if     OS_MBOX_EN || (OS_Q_EN && (OS_MAX_QS >= 2))
        ptcb->OSTCBMsg       = (void *)0;                  /* No message received                      */
#endif

        OS_ENTER_CRITICAL();
        OSTCBPrioTbl[prio]   = ptcb;
        ptcb->OSTCBNext      = OSTCBList;                  /* Link into TCB chain                      */
        ptcb->OSTCBPrev      = (OS_TCB *)0;
        if (OSTCBList != (OS_TCB *)0) {
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList               = ptcb;
        OSRdyGrp               |= ptcb->OSTCBBitY;         /* Make task ready to run                   */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OS_EXIT_CRITICAL();
        return (OS_NO_ERR);
    } else {
        OS_EXIT_CRITICAL();
        return (OS_NO_MORE_TCB);
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-II the occurrence of a 'system tick' (also known
*              as a 'clock tick').  This function should be called by the ticker ISR but, can also be
*              called by a high priority task.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

void OSTimeTick (void)
{
    OS_TCB *ptcb;


    OSTimeTickHook();                                      /* Call user definable hook                 */
    ptcb = OSTCBList;                                      /* Point at first TCB in TCB list           */
    while (ptcb->OSTCBPrio != OS_IDLE_PRIO) {              /* Go through all TCBs in TCB list          */
        OS_ENTER_CRITICAL();
        if (ptcb->OSTCBDly != 0) {                         /* Delayed or waiting for event with TO     */
            if (--ptcb->OSTCBDly == 0) {                   /* Decrement nbr of ticks to end of delay   */
                if (!(ptcb->OSTCBStat & OS_STAT_SUSPEND)) {    /* Is task suspended?                   */
                    OSRdyGrp               |= ptcb->OSTCBBitY; /* No,  Make task Rdy to Run (timed out)*/
                    OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
                } else {                                       /* Yes, Leave 1 tick to prevent ...     */
                    ptcb->OSTCBDly = 1;                        /* ... loosing the task when the ...    */
                }                                              /* ... suspension is removed.           */
            }
        }
        ptcb = ptcb->OSTCBNext;                            /* Point at next TCB in TCB list            */
        OS_EXIT_CRITICAL();
    }
    OS_ENTER_CRITICAL();                                   /* Update the 32-bit tick counter           */
    OSTime++;
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             GET VERSION
*
* Description: This function is used to return the version number of uC/OS-II.  The returned value
*              corresponds to uC/OS-II's version number multiplied by 100.  In other words, version 2.00
*              would be returned as 200.
*
* Arguments  : none
*
* Returns    : the version number of uC/OS-II multiplied by 100.
*********************************************************************************************************
*/

INT16U OSVersion (void)
{
    return (OS_VERSION);
}
