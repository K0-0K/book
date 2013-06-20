/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : uCOS_II.H
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

#define  OS_VERSION              200   /* Version of uC/OS-II (Vx.yy multiplied by 100)                */

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#define  OS_PRIO_SELF           0xFF   /* Indicate SELF priority                                       */

#if      OS_TASK_STAT_EN
#define  OS_N_SYS_TASKS            2                    /* Number of system tasks                      */
#else
#define  OS_N_SYS_TASKS            1
#endif

#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* Statistic task priority                     */
#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE      task priority                     */

#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of ready table                         */

#define  OS_TASK_IDLE_ID       65535   /* I.D. numbers for Idle and Stat tasks                         */
#define  OS_TASK_STAT_ID       65534

                                       /* TASK STATUS (Bit definition for OSTCBStat)                   */
#define  OS_STAT_RDY            0x00   /* Ready to run                                                 */
#define  OS_STAT_SEM            0x01   /* Pending on semaphore                                         */
#define  OS_STAT_MBOX           0x02   /* Pending on mailbox                                           */
#define  OS_STAT_Q              0x04   /* Pending on queue                                             */
#define  OS_STAT_SUSPEND        0x08   /* Task is suspended                                            */

#define  OS_EVENT_TYPE_MBOX        1
#define  OS_EVENT_TYPE_Q           2
#define  OS_EVENT_TYPE_SEM         3

                                       /* TASK OPTIONS (see OSTaskCreateExt())                         */
#define  OS_TASK_OPT_STK_CHK  0x0001   /* Enable stack checking for the task                           */
#define  OS_TASK_OPT_STK_CLR  0x0002   /* Clear the stack when the task is create                      */
#define  OS_TASK_OPT_SAVE_FP  0x0004   /* Save the contents of any floating-point registers            */


#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif

/*
*********************************************************************************************************
*                                              ERROR CODES
*********************************************************************************************************
*/

#define OS_NO_ERR                 0
#define OS_ERR_EVENT_TYPE         1
#define OS_ERR_PEND_ISR           2

#define OS_TIMEOUT               10
#define OS_TASK_NOT_EXIST        11

#define OS_MBOX_FULL             20

#define OS_Q_FULL                30

#define OS_PRIO_EXIST            40
#define OS_PRIO_ERR              41
#define OS_PRIO_INVALID          42

#define OS_SEM_OVF               50

#define OS_TASK_DEL_ERR          60
#define OS_TASK_DEL_IDLE         61
#define OS_TASK_DEL_REQ          62
#define OS_TASK_DEL_ISR          63

#define OS_NO_MORE_TCB           70

#define OS_TIME_NOT_DLY          80
#define OS_TIME_INVALID_MINUTES  81
#define OS_TIME_INVALID_SECONDS  82
#define OS_TIME_INVALID_MILLI    83
#define OS_TIME_ZERO_DLY         84

#define OS_TASK_SUSPEND_PRIO     90
#define OS_TASK_SUSPEND_IDLE     91

#define OS_TASK_RESUME_PRIO     100
#define OS_TASK_NOT_SUSPENDED   101

#define OS_MEM_INVALID_PART     110
#define OS_MEM_INVALID_BLKS     111
#define OS_MEM_INVALID_SIZE     112
#define OS_MEM_NO_FREE_BLKS     113
#define OS_MEM_FULL             114

#define OS_TASK_OPT_ERR         130

/*$PAGE*/
/*
*********************************************************************************************************
*                                          EVENT CONTROL BLOCK
*********************************************************************************************************
*/

#if (OS_MAX_EVENTS >= 2)
typedef struct {
    void   *OSEventPtr;                    /* Pointer to message or queue structure                    */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
    INT16U  OSEventCnt;                    /* Count of used when event is a semaphore                  */
    INT8U   OSEventType;                   /* OS_EVENT_TYPE_MBOX, OS_EVENT_TYPE_Q or OS_EVENT_TYPE_SEM */
    INT8U   OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
} OS_EVENT;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          MESSAGE MAILBOX DATA
*********************************************************************************************************
*/

#if OS_MBOX_EN
typedef struct {
    void   *OSMsg;                         /* Pointer to message in mailbox                            */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* List of tasks waiting for event to occur                 */
    INT8U   OSEventGrp;                    /* Group corresponding to tasks waiting for event to occur  */
} OS_MBOX_DATA;
#endif

/*
*********************************************************************************************************
*                                     MEMORY PARTITION DATA STRUCTURES
*********************************************************************************************************
*/

#if OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
typedef struct {                       /* MEMORY CONTROL BLOCK                                         */
    void   *OSMemAddr;                 /* Pointer to beginning of memory partition                     */
    void   *OSMemFreeList;             /* Pointer to list of free memory blocks                        */
    INT32U  OSMemBlkSize;              /* Size (in bytes) of each block of memory                      */
    INT32U  OSMemNBlks;                /* Total number of blocks in this partition                     */
    INT32U  OSMemNFree;                /* Number of memory blocks remaining in this partition          */
} OS_MEM;


typedef struct {
    void   *OSAddr;                    /* Pointer to the beginning address of the memory partition     */
    void   *OSFreeList;                /* Pointer to the beginning of the free list of memory blocks   */
    INT32U  OSBlkSize;                 /* Size (in bytes) of each memory block                         */
    INT32U  OSNBlks;                   /* Total number of blocks in the partition                      */
    INT32U  OSNFree;                   /* Number of memory blocks free                                 */
    INT32U  OSNUsed;                   /* Number of memory blocks used                                 */
} OS_MEM_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          MESSAGE QUEUE DATA
*********************************************************************************************************
*/

#if OS_Q_EN
typedef struct {
    void   *OSMsg;                          /* Pointer to next message to be extracted from queue      */
    INT16U  OSNMsgs;                        /* Number of messages in message queue                     */
    INT16U  OSQSize;                        /* Size of message queue                                   */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
} OS_Q_DATA;
#endif

/*
*********************************************************************************************************
*                                           SEMAPHORE DATA
*********************************************************************************************************
*/

#if OS_SEM_EN
typedef struct {
    INT16U  OSCnt;                          /* Semaphore count                                         */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* List of tasks waiting for event to occur                */
    INT8U   OSEventGrp;                     /* Group corresponding to tasks waiting for event to occur */
} OS_SEM_DATA;
#endif

/*
*********************************************************************************************************
*                                            TASK STACK DATA
*********************************************************************************************************
*/

#if OS_TASK_CREATE_EXT_EN
typedef struct {
    INT32U  OSFree;                    /* Number of free bytes on the stack                            */
    INT32U  OSUsed;                    /* Number of bytes used on the stack                            */
} OS_STK_DATA;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK        *OSTCBStkPtr;        /* Pointer to current top of stack                              */

#if OS_TASK_CREATE_EXT_EN    
    void          *OSTCBExtPtr;        /* Pointer to user definable data for TCB extension             */
    OS_STK        *OSTCBStkBottom;     /* Pointer to bottom of stack                                   */
    INT32U         OSTCBStkSize;       /* Size of task stack (in bytes)                                */
    INT16U         OSTCBOpt;           /* Task options as passed by OSTaskCreateExt()                  */
    INT16U         OSTCBId;            /* Task ID (0..65535)                                           */
#endif

    struct os_tcb *OSTCBNext;          /* Pointer to next     TCB in the TCB list                      */
    struct os_tcb *OSTCBPrev;          /* Pointer to previous TCB in the TCB list                      */

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
    OS_EVENT      *OSTCBEventPtr;      /* Pointer to event control block                               */
#endif

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN
    void          *OSTCBMsg;           /* Message received from OSMboxPost() or OSQPost()              */
#endif    

    INT16U         OSTCBDly;           /* Nbr ticks to delay task or, timeout waiting for event        */
    INT8U          OSTCBStat;          /* Task status                                                  */
    INT8U          OSTCBPrio;          /* Task priority (0 == highest, 63 == lowest)                   */

    INT8U          OSTCBX;             /* Bit position in group  corresponding to task priority (0..7) */
    INT8U          OSTCBY;             /* Index into ready table corresponding to task priority        */
    INT8U          OSTCBBitX;          /* Bit mask to access bit position in ready table               */
    INT8U          OSTCBBitY;          /* Bit mask to access bit position in ready group               */
    
#if OS_TASK_DEL_EN    
    BOOLEAN        OSTCBDelReq;        /* Indicates whether a task needs to delete itself              */
#endif
} OS_TCB;

/*$PAGE*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
                                       
OS_EXT  INT32U       OSCtxSwCtr;               /* Counter of number of context switches                */

#if     (OS_MAX_EVENTS >= 2)
OS_EXT  OS_EVENT    *OSEventFreeList;          /* Pointer to list of free EVENT control blocks         */
OS_EXT  OS_EVENT     OSEventTbl[OS_MAX_EVENTS];/* Table of EVENT control blocks                        */
#endif

OS_EXT  INT32U       OSIdleCtr;                /* Idle counter                                         */

#if     OS_TASK_STAT_EN
OS_EXT  INT8S        OSCPUUsage;               /* Percentage of CPU used                               */
OS_EXT  INT32U       OSIdleCtrMax;             /* Maximum value that idle counter can take in 1 sec.   */
OS_EXT  INT32U       OSIdleCtrRun;             /* Value reached by idle counter at run time in 1 sec.  */
OS_EXT  BOOLEAN      OSStatRdy;                /* Flag indicating that the statistic task is ready     */
#endif

OS_EXT  INT8U        OSIntNesting;             /* Interrupt nesting level                              */

OS_EXT  INT8U        OSLockNesting;            /* Multitasking lock nesting level                      */

OS_EXT  INT8U        OSPrioCur;                /* Priority of current task                             */
OS_EXT  INT8U        OSPrioHighRdy;            /* Priority of highest priority task                    */

OS_EXT  INT8U        OSRdyGrp;                        /* Ready list group                              */
OS_EXT  INT8U        OSRdyTbl[OS_RDY_TBL_SIZE];       /* Table of tasks which are ready to run         */

OS_EXT  BOOLEAN      OSRunning;                       /* Flag indicating that kernel is running        */

#if     OS_TASK_CREATE_EN  || OS_TASK_CREATE_EXT_EN || OS_TASK_DEL_EN
OS_EXT  INT8U        OSTaskCtr;                       /* Number of tasks created                       */
#endif

OS_EXT  OS_TCB      *OSTCBCur;                        /* Pointer to currently running TCB              */
OS_EXT  OS_TCB      *OSTCBFreeList;                   /* Pointer to list of free TCBs                  */
OS_EXT  OS_TCB      *OSTCBHighRdy;                    /* Pointer to highest priority TCB ready to run  */
OS_EXT  OS_TCB      *OSTCBList;                       /* Pointer to doubly linked list of TCBs         */
OS_EXT  OS_TCB      *OSTCBPrioTbl[OS_LOWEST_PRIO + 1];/* Table of pointers to created TCBs             */

OS_EXT  INT32U       OSTime;                   /* Current value of system time (in ticks)              */


extern  INT8U const  OSMapTbl[];               /* Priority->Bit Mask lookup table                      */
extern  INT8U const  OSUnMapTbl[];             /* Priority->Index    lookup table                      */

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                     (Target Independant Functions)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        MESSAGE MAILBOX MANAGEMENT
*********************************************************************************************************
*/
#if         OS_MBOX_EN
void       *OSMboxAccept(OS_EVENT *pevent);
OS_EVENT   *OSMboxCreate(void *msg);
void       *OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSMboxPost(OS_EVENT *pevent, void *msg);
INT8U       OSMboxQuery(OS_EVENT *pevent, OS_MBOX_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                           MEMORY MANAGEMENT
*********************************************************************************************************
*/
#if         OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
OS_MEM     *OSMemCreate(void *addr, INT32U nblks, INT32U blksize, INT8U *err);
void       *OSMemGet(OS_MEM *pmem, INT8U *err);
INT8U       OSMemPut(OS_MEM *pmem, void *pblk);
INT8U       OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                         MESSAGE QUEUE MANAGEMENT
*********************************************************************************************************
*/
#if         OS_Q_EN && (OS_MAX_QS >= 2)
void       *OSQAccept(OS_EVENT *pevent);
OS_EVENT   *OSQCreate(void **start, INT16U size);
INT8U       OSQFlush(OS_EVENT *pevent);
void       *OSQPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSQPost(OS_EVENT *pevent, void *msg);
INT8U       OSQPostFront(OS_EVENT *pevent, void *msg);
INT8U       OSQQuery(OS_EVENT *pevent, OS_Q_DATA *pdata);
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                          SEMAPHORE MANAGEMENT
*********************************************************************************************************
*/
#if         OS_SEM_EN
INT16U      OSSemAccept(OS_EVENT *pevent);
OS_EVENT   *OSSemCreate(INT16U value);
void        OSSemPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSSemPost(OS_EVENT *pevent);
INT8U       OSSemQuery(OS_EVENT *pevent, OS_SEM_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                            TASK MANAGEMENT
*********************************************************************************************************
*/
#if         OS_TASK_CHANGE_PRIO_EN
INT8U       OSTaskChangePrio(INT8U oldprio, INT8U newprio);
#endif

INT8U       OSTaskCreate(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio);

#if         OS_TASK_CREATE_EXT_EN
INT8U       OSTaskCreateExt(void  (*task)(void *pd), 
                            void   *pdata, 
                            OS_STK *ptos, 
                            INT8U   prio,
                            INT16U  id,
                            OS_STK *pbos,
                            INT32U  stk_size,
                            void   *pext,
                            INT16U  opt);
#endif                            

#if         OS_TASK_DEL_EN
INT8U       OSTaskDel(INT8U prio);
INT8U       OSTaskDelReq(INT8U prio);
#endif

#if         OS_TASK_SUSPEND_EN
INT8U       OSTaskResume(INT8U prio);
INT8U       OSTaskSuspend(INT8U prio);
#endif

#if         OS_TASK_CREATE_EXT_EN
INT8U       OSTaskStkChk(INT8U prio, OS_STK_DATA *pdata);
#endif

INT8U       OSTaskQuery(INT8U prio, OS_TCB *pdata);

/*
*********************************************************************************************************
*                                            TIME MANAGEMENT
*********************************************************************************************************
*/
void        OSTimeDly(INT16U ticks);
INT8U       OSTimeDlyHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U milli);
INT8U       OSTimeDlyResume(INT8U prio);
INT32U      OSTimeGet(void);
void        OSTimeSet(INT32U ticks);
void        OSTimeTick(void);

/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

void        OSInit(void);

void        OSIntEnter(void);
void        OSIntExit(void);

void        OSSchedLock(void);
void        OSSchedUnlock(void);

void        OSStart(void);

void        OSStatInit(void);

INT16U      OSVersion(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                      INTERNAL FUNCTION PROTOTYPES
*                            (Your application MUST NOT call these functions)
*********************************************************************************************************
*/

#if         OS_MBOX_EN || OS_Q_EN || OS_SEM_EN
void        OSEventTaskRdy(OS_EVENT *pevent, void *msg, INT8U msk);
void        OSEventTaskWait(OS_EVENT *pevent);
void        OSEventTO(OS_EVENT *pevent);
void        OSEventWaitListInit(OS_EVENT *pevent);
#endif

#if         OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
void        OSMemInit(void);
#endif

#if         OS_Q_EN
void        OSQInit(void);
#endif

void        OSSched(void);

void        OSTaskIdle(void *data);

#if         OS_TASK_STAT_EN
void        OSTaskStat(void *data);
#endif

INT8U       OSTCBInit(INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT16U stk_size, void *pext, INT16U opt);

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                      (Target Specific Functions)
*********************************************************************************************************
*/

void        OSCtxSw(void);

void        OSIntCtxSw(void);

void        OSStartHighRdy(void);

void        OSTaskCreateHook(OS_TCB *ptcb);
void        OSTaskDelHook(OS_TCB *ptcb);
void        OSTaskStatHook(void);
void       *OSTaskStkInit(void (*task)(void *pd), void *pdata, void *ptos, INT16U opt);
void        OSTaskSwHook(void);

void        OSTickISR(void);

void        OSTimeTickHook(void);
