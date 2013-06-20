/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                       MESSAGE MAILBOX MANAGEMENT
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : OS_MBOX.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if OS_MBOX_EN
/*
*********************************************************************************************************
*                                     ACCEPT MESSAGE FROM MAILBOX
*
* Description: This function checks the mailbox to see if a message is available.  Unlike OSMboxPend(),
*              OSMboxAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent        is a pointer to the event control block
*
* Returns    : != (void *)0  is the message in the mailbox if one is available.  The mailbox is cleared
*                            so the next time OSMboxAccept() is called, the mailbox will be empty.
*              == (void *)0  if the mailbox is empty or if you didn't pass the proper event pointer.
*********************************************************************************************************
*/

void *OSMboxAccept (OS_EVENT *pevent)
{
    void  *msg;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_MBOX) {      /* Validate event block type                 */
        OS_EXIT_CRITICAL();
        return ((void *)0);
    }
    msg = pevent->OSEventPtr; 
    if (msg != (void *)0) {                               /* See if there is already a message         */
        pevent->OSEventPtr = (void *)0;                   /* Clear the mailbox                         */
    }
    OS_EXIT_CRITICAL();
    return (msg);                                         /* Return the message received (or NULL)     */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MESSAGE MAILBOX
*
* Description: This function creates a message mailbox if free event control blocks are available.
*
* Arguments  : msg           is a pointer to a message that you wish to deposit in the mailbox.  If
*                            you set this value to the NULL pointer (i.e. (void *)0) then the mailbox
*                            will be considered empty.
*
* Returns    : != (void *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                            created mailbox
*              == (void *)0  if no event control blocks were available
*********************************************************************************************************
*/

OS_EVENT *OSMboxCreate (void *msg)
{
    OS_EVENT *pevent;


    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
    if (OSEventFreeList != (OS_EVENT *)0) {      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {
        pevent->OSEventType = OS_EVENT_TYPE_MBOX;
        pevent->OSEventPtr  = msg;               /* Deposit message in event control block             */
        OSEventWaitListInit(pevent);
    }
    return (pevent);                             /* Return pointer to event control block              */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                      PEND ON MAILBOX FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a mailbox
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the mailbox up to the amount of time 
*                            specified by this argument.  If you specify 0, however, your task will wait 
*                            forever at the specified mailbox or, until a message arrives.
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR         The call was successful and your task received a message.
*                            OS_TIMEOUT        A message was not received within the specified timeout
*                            OS_ERR_EVENT_TYPE Invalid event type
*                            OS_ERR_PEND_ISR   If you called this function from an ISR and the result
*                                              would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if no message was received or you didn't pass the proper pointer to the
*                            event control block.
*********************************************************************************************************
*/

void *OSMboxPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
    void  *msg;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_MBOX) {  /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        *err = OS_ERR_EVENT_TYPE;
        return ((void *)0);
    }
    msg = pevent->OSEventPtr;
    if (msg != (void *)0) {                           /* See if there is already a message             */
        pevent->OSEventPtr = (void *)0;               /* Clear the mailbox                             */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
    } else if (OSIntNesting > 0) {                    /* See if called from ISR ...                    */
        OS_EXIT_CRITICAL();                           /* ... can't PEND from an ISR                    */
        *err = OS_ERR_PEND_ISR;
    } else {
        OSTCBCur->OSTCBStat |= OS_STAT_MBOX;          /* Message not available, task will pend         */
        OSTCBCur->OSTCBDly   = timeout;               /* Load timeout in TCB                           */
        OSEventTaskWait(pevent);                      /* Suspend task until event or timeout occurs    */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find next highest priority task ready to run  */
        OS_ENTER_CRITICAL();
        if ((msg = OSTCBCur->OSTCBMsg) != (void *)0) {     /* See if we were given the message         */
            OSTCBCur->OSTCBMsg      = (void *)0;           /* Yes, clear message received              */
            OSTCBCur->OSTCBStat     = OS_STAT_RDY;
            OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;       /* No longer waiting for event              */
            OS_EXIT_CRITICAL();
            *err                    = OS_NO_ERR;
        } else if (OSTCBCur->OSTCBStat & OS_STAT_MBOX) {   /* If status is not OS_STAT_RDY, timed out  */
            OSEventTO(pevent);                             /* Make task ready                          */
            OS_EXIT_CRITICAL();
            msg                     = (void *)0;           /* Set message contents to NULL             */
            *err                    = OS_TIMEOUT;          /* Indicate that a timeout occured          */
        } else {
            msg                     = pevent->OSEventPtr;  /* Message received                         */
            pevent->OSEventPtr      = (void *)0;           /* Clear the mailbox                        */
            OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;
            OS_EXIT_CRITICAL();
            *err                    = OS_NO_ERR;
        }
    }
    return (msg);                                          /* Return the message received (or NULL)    */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       POST MESSAGE TO A MAILBOX
*
* Description: This function sends a message to a mailbox
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.  
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_MBOX_FULL       If the mailbox already contains a message.  You can can only send one
*                                 message at a time and thus, the message MUST be consumed before you are
*                                 allowed to send another one.
*              OS_ERR_EVENT_TYPE  If you are attempting to post to a non mailbox.
*********************************************************************************************************
*/

INT8U OSMboxPost (OS_EVENT *pevent, void *msg)
{
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_MBOX) {  /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    if (pevent->OSEventGrp) {                         /* See if any task pending on mailbox            */
        OSEventTaskRdy(pevent, msg, OS_STAT_MBOX);    /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    } else {
        if (pevent->OSEventPtr != (void *)0) {        /* Make sure mailbox doesn't already have a msg  */
            OS_EXIT_CRITICAL();
            return (OS_MBOX_FULL);
        } else {
            pevent->OSEventPtr = msg;                 /* Place message in mailbox                      */
            OS_EXIT_CRITICAL();
            return (OS_NO_ERR);
        }
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE MAILBOX
*
* Description: This function obtains information about a message mailbox.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              pdata         is a pointer to a structure that will contain information about the message
*                            mailbox.
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE  If you are attempting to obtain data from a non mailbox.
*********************************************************************************************************
*/

INT8U OSMboxQuery (OS_EVENT *pevent, OS_MBOX_DATA *pdata)
{
    INT8U  i;
    INT8U *psrc;
    INT8U *pdest;
    
    
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_MBOX) {       /* Validate event block type                */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message mailbox wait list           */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;   
    }
    pdata->OSMsg = pevent->OSEventPtr;                     /* Get message from mailbox                 */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
