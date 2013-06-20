/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                          SEMAPHORE MANAGEMENT
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : OS_SEM.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if OS_SEM_EN
/*
*********************************************************************************************************
*                                           ACCEPT SEMAPHORE
*
* Description: This function checks the semaphore to see if a resource is available or, if an event
*              occurred.  Unlike OSSemPend(), OSSemAccept() does not suspend the calling task if the
*              resource is not available or the event did not occur.
*
* Arguments  : pevent     is a pointer to the event control block
*
* Returns    : >  0       if the resource is available or the event did not occur the semaphore is
*                         decremented to obtain the resource.
*              == 0       if the resource is not available or the event did not occur or,
*                         you didn't pass a pointer to a semaphore
*********************************************************************************************************
*/

INT16U OSSemAccept (OS_EVENT *pevent)
{
    INT16U cnt;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return (0);
    }
    cnt = pevent->OSEventCnt;
    if (cnt > 0) {                                    /* See if resource is available                  */
        pevent->OSEventCnt--;                         /* Yes, decrement semaphore and notify caller    */
    }
    OS_EXIT_CRITICAL();
    return (cnt);                                     /* Return semaphore count                        */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : cnt           is the initial value for the semaphore.  If the value is 0, no resource is
*                            available (or no event has occurred).  You initialize the semaphore to a 
*                            non-zero value to specify how many resources are available (e.g. if you have
*                            10 resources, you would initialize the semaphore to 10).
*
* Returns    : != (void *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                            created semaphore
*              == (void *)0  if no event control blocks were available
*********************************************************************************************************
*/

OS_EVENT *OSSemCreate (INT16U cnt)
{
    OS_EVENT *pevent;


    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                              /* Get next free event control block        */
    if (OSEventFreeList != (OS_EVENT *)0) {                /* See if pool of free ECB pool was empty   */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {                         /* Get an event control block               */
        pevent->OSEventType = OS_EVENT_TYPE_SEM;
        pevent->OSEventCnt  = cnt;                         /* Set semaphore value                      */
        OSEventWaitListInit(pevent);
    }
    return (pevent);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired 
*                            semaphore.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for the resource up to the amount of time specified by this argument.  
*                            If you specify 0, however, your task will wait forever at the specified 
*                            semaphore or, until the resource becomes available (or the event occurs).
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR          The call was successful and your task owns the resource 
*                                               or, the event you are waiting for occurred.
*                            OS_TIMEOUT         The semaphore was not received within the specified 
*                                               timeout.
*                            OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a semaphore.
*                            OS_ERR_PEND_ISR    If you called this function from an ISR and the result
*                                               would lead to a suspension.
*
* Returns    : none
*********************************************************************************************************
*/

void OSSemPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {   /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        *err = OS_ERR_EVENT_TYPE;
    }
    if (pevent->OSEventCnt > 0) {                     /* If sem. is positive, resource available ...   */
        pevent->OSEventCnt--;                         /* ... decrement semaphore only if positive.     */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
    } else if (OSIntNesting > 0) {                    /* See if called from ISR ...                    */
        OS_EXIT_CRITICAL();                           /* ... can't PEND from an ISR                    */
        *err = OS_ERR_PEND_ISR;
    } else {                                          /* Otherwise, must wait until event occurs       */
        OSTCBCur->OSTCBStat    |= OS_STAT_SEM;        /* Resource not available, pend on semaphore     */
        OSTCBCur->OSTCBDly      = timeout;            /* Store pend timeout in TCB                     */
        OSEventTaskWait(pevent);                      /* Suspend task until event or timeout occurs    */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find next highest priority task ready         */
        OS_ENTER_CRITICAL();
        if (OSTCBCur->OSTCBStat & OS_STAT_SEM) {      /* Must have timed out if still waiting for event*/
            OSEventTO(pevent);
            OS_EXIT_CRITICAL();
            *err = OS_TIMEOUT;                        /* Indicate that didn't get event within TO      */
        } else {
            OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;
            OS_EXIT_CRITICAL();
            *err = OS_NO_ERR;
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired 
*                            semaphore.
*
* Returns    : OS_NO_ERR          The call was successful and the semaphore was signaled.
*              OS_SEM_OVF         If the semaphore count exceeded its limit.  In other words, you have 
*                                 signalled the semaphore more often than you waited on it with either
*                                 OSSemAccept() or OSSemPend().
*              OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a semaphore
*********************************************************************************************************
*/

INT8U OSSemPost (OS_EVENT *pevent)
{
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {        /* Validate event block type                */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    if (pevent->OSEventGrp) {                              /* See if any task waiting for semaphore    */
        OSEventTaskRdy(pevent, (void *)0, OS_STAT_SEM);    /* Ready highest prio task waiting on event */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    } else {
        if (pevent->OSEventCnt < 65535) {             /* Make sure semaphore will not overflow         */
            pevent->OSEventCnt++;                     /* Increment semaphore count to register event   */
            OS_EXIT_CRITICAL();
            return (OS_NO_ERR);
        } else {                                      /* Semaphore value has reached its maximum       */
            OS_EXIT_CRITICAL();
            return (OS_SEM_OVF);
        }
    }
}
/*
*********************************************************************************************************
*                                          QUERY A SEMAPHORE
*
* Description: This function obtains information about a semaphore
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired 
*                            semaphore
*
*              pdata         is a pointer to a structure that will contain information about the 
*                            semaphore.
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE  If you are attempting to obtain data from a non semaphore.
*********************************************************************************************************
*/

INT8U OSSemQuery (OS_EVENT *pevent, OS_SEM_DATA *pdata)
{
    INT8U  i;
    INT8U *psrc;
    INT8U *pdest;
    
    
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_SEM) {        /* Validate event block type                */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message mailbox wait list           */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;   
    }
    pdata->OSCnt      = pevent->OSEventCnt;                /* Get semaphore count                      */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
