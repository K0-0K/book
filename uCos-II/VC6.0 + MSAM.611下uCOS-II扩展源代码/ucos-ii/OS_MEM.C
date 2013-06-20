/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                            MEMORY MANAGEMENT
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
*                                                  V2.00
*
* File : OS_MEM.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "includes.h"
#endif

#if OS_MEM_EN && OS_MAX_MEM_PART >= 2
/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_MEM      *OSMemFreeList;            /* Pointer to free list of memory partitions            */
static  OS_MEM       OSMemTbl[OS_MAX_MEM_PART];/* Storage for memory partition manager                 */
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-II.
*
* Arguments   : addr     is the starting address of the memory partition
*
*               nblks    is the number of memory blocks to create from the partition.
*
*               blksize  is the size (in bytes) of each block in the memory partition.
*
*               err      is a pointer to a variable containing an error message which will be set by
*                        this function to either:
*             
*                        OS_NO_ERR            if the memory partition has been created correctly.
*                        OS_MEM_INVALID_PART  no free partitions available
*                        OS_MEM_INVALID_BLKS  user specified an invalid number of blocks (must be >= 2)
*                        OS_MEM_INVALID_SIZE  user specified an invalid block size 
*                                             (must be greater than the size of a pointer)
* Returns    : != (OS_MEM *)0  is the partition was created 
*              == (OS_MEM *)0  if the partition was not created because of invalid arguments or, no
*                              free partition is available.
*********************************************************************************************************
*/

OS_MEM *OSMemCreate (void *addr, INT32U nblks, INT32U blksize, INT8U *err)
{
    OS_MEM  *pmem;
    INT8U   *pblk;
    void   **plink;
    INT32U   i;


    if (nblks < 2) {                                  /* Must have at least 2 blocks per partition      */
        *err = OS_MEM_INVALID_BLKS;
        return ((OS_MEM *)0);
    }
    if (blksize < sizeof(void *)) {                   /* Must contain space for at least a pointer      */
        *err = OS_MEM_INVALID_SIZE;
        return ((OS_MEM *)0);
    }
    OS_ENTER_CRITICAL();
    pmem = OSMemFreeList;                             /* Get next free memory partition                */
    if (OSMemFreeList != (OS_MEM *)0) {               /* See if pool of free partitions was empty      */
        OSMemFreeList = (OS_MEM *)OSMemFreeList->OSMemFreeList;
    }
    OS_EXIT_CRITICAL();
    if (pmem == (OS_MEM *)0) {                        /* See if we have a memory partition             */
        *err = OS_MEM_INVALID_PART;
        return ((OS_MEM *)0);
    }
    plink = (void **)addr;                            /* Create linked list of free memory blocks      */
    pblk  = (INT8U *)addr + blksize;
    for (i = 0; i < (nblks - 1); i++) {
        *plink = (void *)pblk;
        plink  = (void **)pblk;
        pblk   = pblk + blksize;
    }
    *plink = (void *)0;                               /* Last memory block points to NULL              */
    OS_ENTER_CRITICAL();
    pmem->OSMemAddr     = addr;                       /* Store start address of memory partition       */
    pmem->OSMemFreeList = addr;                       /* Initialize pointer to pool of free blocks     */
    pmem->OSMemNFree    = nblks;                      /* Store number of free blocks in MCB            */
    pmem->OSMemNBlks    = nblks;
    pmem->OSMemBlkSize  = blksize;                    /* Store block size of each memory blocks        */
    OS_EXIT_CRITICAL();
    *err   = OS_NO_ERR;
    return (pmem);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               err     is a pointer to a variable containing an error message which will be set by this
*                       function to either:
*
*                       OS_NO_ERR           if the memory partition has been created correctly.
*                       OS_MEM_NO_FREE_BLKS if there are no more free memory blocks to allocate to caller
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
*********************************************************************************************************
*/

void *OSMemGet (OS_MEM *pmem, INT8U *err)
{
    void    *pblk;


    OS_ENTER_CRITICAL();
    if (pmem->OSMemNFree > 0) {                       /* See if there are any free memory blocks       */
        pblk                = pmem->OSMemFreeList;    /* Yes, point to next free memory block          */
        pmem->OSMemFreeList = *(void **)pblk;         /*      Adjust pointer to new free list          */
        pmem->OSMemNFree--;                           /*      One less memory block in this partition  */
        OS_EXIT_CRITICAL();
        *err = OS_NO_ERR;                             /*      No error                                 */
        return (pblk);                                /*      Return memory block to caller            */
    } else {
        OS_EXIT_CRITICAL();
        *err = OS_MEM_NO_FREE_BLKS;                   /* No,  Notify caller of empty memory partition  */
        return ((void *)0);                           /*      Return NULL pointer to caller            */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-II to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

void OSMemInit (void)
{
    OS_MEM  *pmem;
    INT16U   i;


    pmem = (OS_MEM *)&OSMemTbl[0];                    /* Point to memory control block (MCB)           */
    for (i = 0; i < (OS_MAX_MEM_PART - 1); i++) {     /* Init. list of free memory partitions          */
        pmem->OSMemFreeList = (void *)&OSMemTbl[i+1]; /* Chain list of free partitions                 */
        pmem->OSMemAddr     = (void *)0;              /* Store start address of memory partition       */
        pmem->OSMemNFree    = 0;                      /* No free blocks                                */
        pmem->OSMemNBlks    = 0;                      /* No blocks                                     */
        pmem->OSMemBlkSize  = 0;                      /* Zero size                                     */
        pmem++;
    }
    OSMemTbl[OS_MAX_MEM_PART - 1].OSMemFreeList = (void *)0;
    OSMemFreeList                               = (OS_MEM *)&OSMemTbl[0];
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pblk    is a pointer to the memory block being released.
*
* Returns     : OS_NO_ERR         if the memory block was inserted into the partition
*               OS_MEM_FULL       if you are returning a memory block to an already FULL memory partition
*                                 (You freed more blocks than you allocated!)
*********************************************************************************************************
*/

INT8U OSMemPut (OS_MEM  *pmem, void *pblk)
{
    OS_ENTER_CRITICAL();
    if (pmem->OSMemNFree >= pmem->OSMemNBlks) {  /* Make sure all blocks not already returned          */
        OS_EXIT_CRITICAL();
        return (OS_MEM_FULL);       
    }
    *(void **)pblk      = pmem->OSMemFreeList;   /* Insert released block into free block list         */
    pmem->OSMemFreeList = pblk;
    pmem->OSMemNFree++;                          /* One more memory block in this partition            */
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);                          /* Notify caller that memory block was released       */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          QUERY MEMORY PARTITION
*
* Description : This function is used to determine the number of free memory blocks and the number of 
*               used memory blocks from a memory partition.
*
* Arguments   : pmem    is a pointer to the memory partition control block
*
*               pdata   is a pointer to a structure that will contain information about the memory
*                       partition.
*
* Returns     : OS_NO_ERR         Always returns no error.
*********************************************************************************************************
*/

INT8U OSMemQuery (OS_MEM *pmem, OS_MEM_DATA *pdata)
{
    OS_ENTER_CRITICAL();
    pdata->OSAddr     = pmem->OSMemAddr;
    pdata->OSFreeList = pmem->OSMemFreeList;
    pdata->OSBlkSize  = pmem->OSMemBlkSize;
    pdata->OSNBlks    = pmem->OSMemNBlks;
    pdata->OSNFree    = pmem->OSMemNFree;
    OS_EXIT_CRITICAL();
    pdata->OSNUsed    = pdata->OSNBlks - pdata->OSNFree;
    return (OS_NO_ERR);                         
}
#endif
