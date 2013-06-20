/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                   Configuration for Intel 80x86 (Large)
*
* File : OS_CFG.H
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         uC/OS-II CONFIGURATION
*********************************************************************************************************
*/
#define Number361              0

#define MaxStkSize              400

#define OS_MAX_EVENTS             5            /* Max. number of event control blocks in your application ...  */
                                               /* ... MUST be >= 2                                             */
#define OS_MAX_MEM_PART           2            /* Max. number of memory partitions ...                         */
                                               /* ... MUST be >= 2                                             */
#define OS_MAX_QS                 2            /* Max. number of queue control blocks in your application ...  */
                                               /* ... MUST be >= 2                                             */
#define OS_MAX_TASKS             11            /* Max. number of tasks in your application ...                 */
                                               /* ... MUST be >= 2                                             */

#define OS_LOWEST_PRIO           12            /* Defines the lowest priority that can be assigned ...         */
                                               /* ... MUST NEVER be higher than 63!                            */

#define OS_TASK_IDLE_STK_SIZE    MaxStkSize    /* Idle task stack size (# of 16-bit wide entries)              */

#define OS_MANID_EN				  0            /*身份识别													   */
#define OS_TASK_STAT_EN           0            /* Enable (1) or Disable(0) the statistics task                 */
#define OS_TASK_STAT_STK_SIZE    MaxStkSize    /* Statistics task stack size (# of 16-bit wide entries)        */

#define OS_CPU_HOOKS_EN           1            /* uC/OS-II hooks are found in the processor port files         */
#define OS_MBOX_EN                0            /* Include code for MAILBOXES                                   */
#define OS_MEM_EN                 0            /* Include code for MEMORY MANAGER (fixed sized memory blocks)  */
#define OS_Q_EN                   0            /* Include code for QUEUES                                      */

#define OS_SEM_EN                 1            /* Include code for SEMAPHORES                                  */
#define OS_SEM_ACCEPT_EN          0            /*    Include code for OSSemAccept()                            */
#define OS_SEM_QUERY_EN           0            /*    Include code for OSSemQuery()                             */

#define OS_TASK_CHANGE_PRIO_EN    0            /* Include code for OSTaskChangePrio()                          */
#define OS_TASK_CREATE_EN         1            /* Include code for OSTaskCreate()                              */
#define OS_TASK_CREATE_EXT_EN     0            /* Include code for OSTaskCreateExt()                           */
#define OS_TASK_DEL_EN            1            /* Include code for OSTaskDel()                                 */
#define OS_TASK_SUSPEND_EN        0            /* Include code for OSTaskSuspend() and OSTaskResume()          */
#define OS_TASK_QUERY_EN	      0            /* Include code for OSTaskQuery()						       */
#define OS_TIMEDLYHMSM_EN	      0            /* Include code for OSTimeDlyHMSM()						       */
#define OS_TIMEDLYRESUME_EN	      0            /* Include code for OSTimeDlyResume()						   */
#define OS_TIMEGETSET_EN	      0            /* Include code for OSTimeGet() OSTimeSet()					   */
#define OS_SCHED_LOCK_EN          0			   /*     Include code for OSSchedLock() and OSSchedUnlock()       */


#define OS_TICKS_PER_SEC         100            /* Set the number of ticks in one second                        */

