/*
 * $Id: Os_cfg.h,v 1.2 2002/02/07 10:25:41 linfusheng Exp $
 */

#define OS_MAX_EVENTS            20    /* Max. number of event control blocks in your application ...  */
                                       /* ... MUST be >= 2                                             */
#define OS_MAX_MEM_PART          10    /* Max. number of memory partitions ...                         */
                                       /* ... MUST be >= 2                                             */
#define OS_MAX_QS                 5    /* Max. number of queue control blocks in your application ...  */
                                       /* ... MUST be >= 2                                             */
#define OS_MAX_TASKS             32    /* Max. number of tasks in your application ...                 */
                                       /* ... MUST be >= 2                                             */

#define OS_LOWEST_PRIO           63    /* Defines the lowest priority that can be assigned ...         */
                                       /* ... MUST NEVER be higher than 63!                            */

#define OS_TASK_IDLE_STK_SIZE   1024    /* Idle task stack size (# of 16-bit wide entries)              */

#define OS_TASK_STAT_EN           1    /* Enable (1) or Disable(0) the statistics task                 */
#define OS_TASK_STAT_STK_SIZE   1024    /* Statistics task stack size (# of 16-bit wide entries)        */

#define OS_CPU_HOOKS_EN           0    /* uC/OS-II hooks are NOT found in the processor port files     */
#define OS_MBOX_EN                1    /* Include code for MAILBOXES                                   */
#define OS_MEM_EN                 1    /* Include code for MEMORY MANAGER (fixed sized memory blocks)  */
#define OS_Q_EN                   1    /* Include code for QUEUES                                      */
#define OS_SEM_EN                 1    /* Include code for SEMAPHORES                                  */
#define OS_TASK_CHANGE_PRIO_EN    1    /* Include code for OSTaskChangePrio()                          */
#define OS_TASK_CREATE_EN         1    /* Include code for OSTaskCreate()                              */
#define OS_TASK_CREATE_EXT_EN     1    /* Include code for OSTaskCreateExt()                           */
#define OS_TASK_DEL_EN            1    /* Include code for OSTaskDel()                                 */
#define OS_TASK_SUSPEND_EN        1    /* Include code for OSTaskSuspend() and OSTaskResume()          */

#define OS_TICKS_PER_SEC        200    /* Set the number of ticks in one second                        */
