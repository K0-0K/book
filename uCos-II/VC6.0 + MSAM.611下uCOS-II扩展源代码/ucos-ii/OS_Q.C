/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                        MESSAGE QUEUE MANAGEMENT
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : OS_Q.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if OS_Q_EN && (OS_MAX_QS >= 2)
/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES  
*********************************************************************************************************
*/

typedef struct os_q {                  /* QUEUE CONTROL BLOCK                                          */
    struct os_q   *OSQPtr;             /* Link to next queue control block in list of free blocks      */
    void         **OSQStart;           /* Pointer to start of queue data                               */
    void         **OSQEnd;             /* Pointer to end   of queue data                               */
    void         **OSQIn;              /* Pointer to where next message will be inserted  in   the Q   */
    void         **OSQOut;             /* Pointer to where next message will be extracted from the Q   */
    INT16U         OSQSize;            /* Size of queue (maximum number of entries)                    */
    INT16U         OSQEntries;         /* Current number of entries in the queue                       */
} OS_Q;

/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_Q        *OSQFreeList;              /* Pointer to list of free QUEUE control blocks         */
static  OS_Q         OSQTbl[OS_MAX_QS];        /* Table of QUEUE control blocks                        */

/*$PAGE*/
/*
*********************************************************************************************************
*                                      ACCEPT MESSAGE FROM QUEUE
*
* Description: This function checks the queue to see if a message is available.  Unlike OSQPend(),
*              OSQAccept() does not suspend the calling task if a message is not available.
*
* Arguments  : pevent        is a pointer to the event control block
*
* Returns    : != (void *)0  is the message in the queue if one is available.  The message is removed
*                            from the so the next time OSQAccept() is called, the queue will contain
*                            one less entry.
*              == (void *)0  if the queue is empty
*                            if you passed an invalid event type
*********************************************************************************************************
*/

void *OSQAccept (OS_EVENT *pevent)
{
    void  *msg;
    OS_Q  *pq;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return ((void *)0);
    }
    pq = pevent->OSEventPtr;                     /* Point at queue control block                       */
    if (pq->OSQEntries != 0) {                   /* See if any messages in the queue                   */
        msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
    } else {
        msg = (void *)0;                         /* Queue is empty                                     */
    }
    OS_EXIT_CRITICAL();
    return (msg);                                /* Return message received (or NULL)                  */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              size          is the number of elements in the storage area
*
* Returns    : != (void *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                            created queue
*              == (void *)0  if no event control blocks were available
*********************************************************************************************************
*/

OS_EVENT *OSQCreate (void **start, INT16U size)
{
    OS_EVENT *pevent;
    OS_Q     *pq;


    OS_ENTER_CRITICAL();
    pevent = OSEventFreeList;                    /* Get next free event control block                  */
    if (OSEventFreeList != (OS_EVENT *)0) {      /* See if pool of free ECB pool was empty             */
        OSEventFreeList = (OS_EVENT *)OSEventFreeList->OSEventPtr;
    }
    OS_EXIT_CRITICAL();
    if (pevent != (OS_EVENT *)0) {               /* See if we have an event control block              */
        OS_ENTER_CRITICAL();                     /* Get a free queue control block                     */
        pq = OSQFreeList;
        if (OSQFreeList != (OS_Q *)0) {
            OSQFreeList = OSQFreeList->OSQPtr;
        }
        OS_EXIT_CRITICAL();
        if (pq != (OS_Q *)0) {                   /* See if we were able to get a queue control block   */
            pq->OSQStart        = start;         /* Yes, initialize the queue                          */
            pq->OSQEnd          = &start[size];
            pq->OSQIn           = start;
            pq->OSQOut          = start;
            pq->OSQSize         = size;
            pq->OSQEntries      = 0;
            pevent->OSEventType = OS_EVENT_TYPE_Q;
            pevent->OSEventPtr  = pq;
            OSEventWaitListInit(pevent);
        } else {                                 /* No,  since we couldn't get a queue control block   */
            OS_ENTER_CRITICAL();                 /* Return event control block on error                */
            pevent->OSEventPtr = (void *)OSEventFreeList;
            OSEventFreeList    = pevent;
            OS_EXIT_CRITICAL();
            pevent = (OS_EVENT *)0;
        }
    }
    return (pevent);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                           FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : OS_NO_ERR          upon success
*               OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a queue
*********************************************************************************************************
*/

INT8U OSQFlush (OS_EVENT *pevent)
{
    OS_Q  *pq;
    

    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    pq             = pevent->OSEventPtr;              /* Point to queue storage structure              */
    pq->OSQIn      = pq->OSQStart;
    pq->OSQOut     = pq->OSQStart;
    pq->OSQEntries = 0;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      QUEUE MODULE INITIALIZATION
*
* Description : This function is called by uC/OS-II to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*********************************************************************************************************
*/

void OSQInit (void)
{
    INT16U i;


    for (i = 0; i < (OS_MAX_QS - 1); i++) {      /* Init. list of free QUEUE control blocks            */
        OSQTbl[i].OSQPtr = &OSQTbl[i+1];
    }
    OSQTbl[OS_MAX_QS - 1].OSQPtr = (OS_Q *)0;
    OSQFreeList                  = &OSQTbl[0];
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the queue up to the amount of time 
*                            specified by this argument.  If you specify 0, however, your task will wait 
*                            forever at the specified queue or, until a message arrives.
*
*              err           is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_NO_ERR         The call was successful and your task received a message.
*                            OS_TIMEOUT        A message was not received within the specified timeout
*                            OS_ERR_EVENT_TYPE You didn't pass a pointer to a queue
*                            OS_ERR_PEND_ISR    If you called this function from an ISR and the result
*                                               would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if no message was received or you didn't pass a pointer to a queue.
*********************************************************************************************************
*/

void *OSQPend (OS_EVENT *pevent, INT16U timeout, INT8U *err)
{
    void  *msg;
    OS_Q  *pq;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {/* Validate event block type                          */
        OS_EXIT_CRITICAL();
        *err = OS_ERR_EVENT_TYPE;
        return ((void *)0);
    }
    pq = pevent->OSEventPtr;                     /* Point at queue control block                       */
    if (pq->OSQEntries != 0) {                   /* See if any messages in the queue                   */
        msg = *pq->OSQOut++;                     /* Yes, extract oldest message from the queue         */
        pq->OSQEntries--;                        /* Update the number of entries in the queue          */
        if (pq->OSQOut == pq->OSQEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            pq->OSQOut = pq->OSQStart;
        }
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;
    } else if (OSIntNesting > 0) {               /* See if called from ISR ...                         */
        OS_EXIT_CRITICAL();                      /* ... can't PEND from an ISR                         */
        *err = OS_ERR_PEND_ISR;
    } else {
        OSTCBCur->OSTCBStat    |= OS_STAT_Q;     /* Task will have to pend for a message to be posted  */
        OSTCBCur->OSTCBDly      = timeout;       /* Load timeout into TCB                              */
        OSEventTaskWait(pevent);                 /* Suspend task until event or timeout occurs         */
        OS_EXIT_CRITICAL();
        OSSched();                               /* Find next highest priority task ready to run       */
        OS_ENTER_CRITICAL();
        if ((msg = OSTCBCur->OSTCBMsg) != (void *)0) {/* Did we get a message?                         */
            OSTCBCur->OSTCBMsg      = (void *)0;      /* Extract message from TCB (Put there by QPost) */
            OSTCBCur->OSTCBStat     = OS_STAT_RDY;
            OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;  /* No longer waiting for event                   */
            OS_EXIT_CRITICAL();
            *err                    = OS_NO_ERR;
        } else if (OSTCBCur->OSTCBStat & OS_STAT_Q) { /* Timed out if status indicates pending on Q    */
            OSEventTO(pevent);
            OS_EXIT_CRITICAL();
            msg                     = (void *)0;      /* No message received                           */
            *err                    = OS_TIMEOUT;     /* Indicate a timeout occured                    */
        } else {
            msg = *pq->OSQOut++;                      /* Extract message from queue                    */
            pq->OSQEntries--;                         /* Update the number of entries in the queue     */
            if (pq->OSQOut == pq->OSQEnd) {           /* Wrap OUT pointer if we are at the end of Q    */
                pq->OSQOut = pq->OSQStart;
            }
            OSTCBCur->OSTCBEventPtr = (OS_EVENT *)0;
            OS_EXIT_CRITICAL();
            *err = OS_NO_ERR;
        }
    }                                                 
    return (msg);                                     /* Return message received (or NULL)             */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.  
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_Q_FULL          If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a queue.
*********************************************************************************************************
*/

INT8U OSQPost (OS_EVENT *pevent, void *msg)
{
    OS_Q   *pq;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    if (pevent->OSEventGrp) {                         /* See if any task pending on queue              */
        OSEventTaskRdy(pevent, msg, OS_STAT_Q);       /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    } else {
        pq = pevent->OSEventPtr;                      /* Point to queue control block                  */
        if (pq->OSQEntries >= pq->OSQSize) {          /* Make sure queue is not full                   */
            OS_EXIT_CRITICAL();
            return (OS_Q_FULL);
        } else {
            *pq->OSQIn++ = msg;                       /* Insert message into queue                     */
            pq->OSQEntries++;                         /* Update the nbr of entries in the queue        */
            if (pq->OSQIn == pq->OSQEnd) {            /* Wrap IN ptr if we are at end of queue         */
                pq->OSQIn = pq->OSQStart;
            }
            OS_EXIT_CRITICAL();
        }
        return (OS_NO_ERR);
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                   POST MESSAGE TO THE FRONT OF A QUEUE
*
* Description: This function sends a message to a queue but unlike OSQPost(), the message is posted at
*              the front instead of the end of the queue.  Using OSQPostFront() allows you to send
*              'priority' messages.  
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired queue
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.  
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_Q_FULL          If the queue cannot accept any more messages because it is full.
*              OS_ERR_EVENT_TYPE  If you didn't pass a pointer to a queue.
*********************************************************************************************************
*/

INT8U OSQPostFront (OS_EVENT *pevent, void *msg)
{
    OS_Q   *pq;


    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {     /* Validate event block type                     */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    if (pevent->OSEventGrp) {                         /* See if any task pending on queue              */
        OSEventTaskRdy(pevent, msg, OS_STAT_Q);       /* Ready highest priority task waiting on event  */
        OS_EXIT_CRITICAL();
        OSSched();                                    /* Find highest priority task ready to run       */
        return (OS_NO_ERR);
    } else {
        pq = pevent->OSEventPtr;                      /* Point to queue control block                  */
        if (pq->OSQEntries >= pq->OSQSize) {          /* Make sure queue is not full                   */
            OS_EXIT_CRITICAL();
            return (OS_Q_FULL);
        } else {
            if (pq->OSQOut == pq->OSQStart) {         /* Wrap OUT ptr if we are at the 1st queue entry */
                pq->OSQOut = pq->OSQEnd;
            }
            pq->OSQOut--;
            *pq->OSQOut = msg;                        /* Insert message into queue                     */
            pq->OSQEntries++;                         /* Update the nbr of entries in the queue        */
            OS_EXIT_CRITICAL();
        }
        return (OS_NO_ERR);
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        QUERY A MESSAGE QUEUE
*
* Description: This function obtains information about a message queue.
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              pdata         is a pointer to a structure that will contain information about the message
*                            queue.
*
* Returns    : OS_NO_ERR          The call was successful and the message was sent
*              OS_ERR_EVENT_TYPE  If you are attempting to obtain data from a non queue.
*********************************************************************************************************
*/

INT8U OSQQuery (OS_EVENT *pevent, OS_Q_DATA *pdata)
{
    OS_Q   *pq;
    INT8U   i;
    INT8U  *psrc;
    INT8U  *pdest;
    
    
    OS_ENTER_CRITICAL();
    if (pevent->OSEventType != OS_EVENT_TYPE_Q) {          /* Validate event block type                */
        OS_EXIT_CRITICAL();
        return (OS_ERR_EVENT_TYPE);
    }
    pdata->OSEventGrp = pevent->OSEventGrp;                /* Copy message mailbox wait list           */
    psrc              = &pevent->OSEventTbl[0];
    pdest             = &pdata->OSEventTbl[0];
    for (i = 0; i < OS_EVENT_TBL_SIZE; i++) {
        *pdest++ = *psrc++;   
    }
    pq = (OS_Q *)pevent->OSEventPtr;
    if (pq->OSQEntries > 0) {
        pdata->OSMsg = pq->OSQOut;                         /* Get next message to return if available  */
    } else {
        pdata->OSMsg = (void *)0;
    }
    pdata->OSNMsgs = pq->OSQEntries;
    pdata->OSQSize = pq->OSQSize;
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
