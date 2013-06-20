/*
*********************************************************************************************************
*                                          PC SUPPORT FUNCTIONS
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : PC.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define  DISP_BASE                  0xB800       /* Base segment of display (0xB800=VGA, 0xB000=Mono)  */
#define  DISP_MAX_X                     80       /* Maximum number of columns                          */
#define  DISP_MAX_Y                     25       /* Maximum number of rows                             */

#define  TICK_T0_8254_CWR             0x43       /* 8254 PIT Control Word Register address.            */
#define  TICK_T0_8254_CTR0            0x40       /* 8254 PIT Timer 0 Register address.                 */
#define  TICK_T0_8254_CTR1            0x41       /* 8254 PIT Timer 1 Register address.                 */
#define  TICK_T0_8254_CTR2            0x42       /* 8254 PIT Timer 2 Register address.                 */

#define  TICK_T0_8254_CTR0_MODE3      0x36       /* 8254 PIT Binary Mode 3 for Counter 0 control word. */
#define  TICK_T0_8254_CTR2_MODE0      0xB0       /* 8254 PIT Binary Mode 0 for Counter 2 control word. */
#define  TICK_T0_8254_CTR2_LATCH      0x80       /* 8254 PIT Latch command control word                */

#define  VECT_TICK                    0x08       /* Vector number for 82C54 timer tick                 */
#define  VECT_DOS_CHAIN               0x81       /* Vector number used to chain DOS                    */

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
             
static INT16U    PC_ElapsedOverhead;
static jmp_buf   PC_JumpBuf;
static BOOLEAN   PC_ExitFlag;
void           (*PC_TickISR)(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                           DISPLAY A SINGLE CHARACTER AT 'X' & 'Y' COORDINATE
*
* Description : This function writes a single character anywhere on the PC's screen.  This function
*               writes directly to video RAM instead of using the BIOS for speed reasons.  It assumed 
*               that the video adapter is VGA compatible.  Video RAM starts at absolute address 
*               0x000B8000.  Each character on the screen is composed of two bytes: the ASCII character 
*               to appear on the screen followed by a video attribute.  An attribute of 0x07 displays 
*               the character in WHITE with a black background.
*
* Arguments   : x      corresponds to the desired column on the screen.  Valid columns numbers are from
*                      0 to 79.  Column 0 corresponds to the leftmost column.
*               y      corresponds to the desired row on the screen.  Valid row numbers are from 0 to 24.
*                      Line 0 corresponds to the topmost row.
*               c      Is the ASCII character to display.  You can also specify a character with a 
*                      numeric value higher than 128.  In this case, special character based graphics
*                      will be displayed.
*               color  specifies the foreground/background color to use (see PC.H for available choices)
*                      and whether the character will blink or not.
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DispChar (INT8U x, INT8U y, INT8U c, INT8U color)
{
    INT8U  far *pscr;
    INT16U      offset;


    offset  = (INT16U)y * DISP_MAX_X * 2 + (INT16U)x * 2;  /* Calculate position on the screen         */
    pscr    = (INT8U far *)MK_FP(DISP_BASE, offset);
    *pscr++ = c;                                           /* Put character in video RAM               */
    *pscr   = color;                                       /* Put video attribute in video RAM         */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                            CLEAR A COLUMN
*
* Description : This function clears one of the 80 columns on the PC's screen by directly accessing video 
*               RAM instead of using the BIOS.  It assumed that the video adapter is VGA compatible.  
*               Video RAM starts at absolute address 0x000B8000.  Each character on the screen is 
*               composed of two bytes: the ASCII character to appear on the screen followed by a video 
*               attribute.  An attribute of 0x07 displays the character in WHITE with a black background.
*
* Arguments   : x            corresponds to the desired column to clear.  Valid column numbers are from 
*                            0 to 79.  Column 0 corresponds to the leftmost column.
*
*               color        specifies the foreground/background color combination to use 
*                            (see PC.H for available choices)
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DispClrCol (INT8U x, INT8U color)
{
    INT8U far *pscr;
    INT8U      i;


    pscr = (INT8U far *)MK_FP(DISP_BASE, (INT16U)x * 2);
    for (i = 0; i < DISP_MAX_Y; i++) {
        *pscr++ = ' ';                           /* Put ' ' character in video RAM                     */
        *pscr-- = color;                         /* Put video attribute in video RAM                   */
        pscr    = pscr + DISP_MAX_X * 2;         /* Position on next row                               */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             CLEAR A ROW
*
* Description : This function clears one of the 25 lines on the PC's screen by directly accessing video 
*               RAM instead of using the BIOS.  It assumed that the video adapter is VGA compatible.  
*               Video RAM starts at absolute address 0x000B8000.  Each character on the screen is 
*               composed of two bytes: the ASCII character to appear on the screen followed by a video 
*               attribute.  An attribute of 0x07 displays the character in WHITE with a black background.
*
* Arguments   : y            corresponds to the desired row to clear.  Valid row numbers are from 
*                            0 to 24.  Row 0 corresponds to the topmost line.
*
*               color        specifies the foreground/background color combination to use 
*                            (see PC.H for available choices)
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DispClrRow (INT8U y, INT8U color)
{
    INT8U far *pscr;
    INT8U      i;


    pscr = (INT8U far *)MK_FP(DISP_BASE, (INT16U)y * DISP_MAX_X * 2);
    for (i = 0; i < DISP_MAX_X; i++) {
        *pscr++ = ' ';                           /* Put ' ' character in video RAM                     */
        *pscr++ = color;                         /* Put video attribute in video RAM                   */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                              CLEAR SCREEN
*
* Description : This function clears the PC's screen by directly accessing video RAM instead of using
*               the BIOS.  It assumed that the video adapter is VGA compatible.  Video RAM starts at
*               absolute address 0x000B8000.  Each character on the screen is composed of two bytes:
*               the ASCII character to appear on the screen followed by a video attribute.  An attribute
*               of 0x07 displays the character in WHITE with a black background.
*
* Arguments   : color   specifies the foreground/background color combination to use 
*                       (see PC.H for available choices)
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DispClrScr (INT8U color)
{
    INT8U  far *pscr;
    INT16U      i;


    pscr = (INT8U far *)MK_FP(DISP_BASE, 0x0000);
    for (i = 0; i < (DISP_MAX_X * DISP_MAX_Y); i++) { /* PC display has 80 columns and 25 lines        */
        *pscr++ = ' ';                                /* Put ' ' character in video RAM                */
        *pscr++ = color;                              /* Put video attribute in video RAM              */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                 DISPLAY A STRING  AT 'X' & 'Y' COORDINATE
*
* Description : This function writes an ASCII string anywhere on the PC's screen.  This function writes
*               directly to video RAM instead of using the BIOS for speed reasons.  It assumed that the 
*               video adapter is VGA compatible.  Video RAM starts at absolute address 0x000B8000.  Each 
*               character on the screen is composed of two bytes: the ASCII character to appear on the 
*               screen followed by a video attribute.  An attribute of 0x07 displays the character in 
*               WHITE with a black background.
*
* Arguments   : x      corresponds to the desired column on the screen.  Valid columns numbers are from
*                      0 to 79.  Column 0 corresponds to the leftmost column.
*               y      corresponds to the desired row on the screen.  Valid row numbers are from 0 to 24.
*                      Line 0 corresponds to the topmost row.
*               s      Is the ASCII string to display.  You can also specify a string containing 
*                      characters with numeric values higher than 128.  In this case, special character 
*                      based graphics will be displayed.
*               color  specifies the foreground/background color to use (see PC.H for available choices)
*                      and whether the characters will blink or not.
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DispStr (INT8U x, INT8U y, INT8U *s, INT8U color)
{
    INT8U  far *pscr;
    INT16U      offset;


    offset  = (INT16U)y * DISP_MAX_X * 2 + (INT16U)x * 2;   /* Calculate position of 1st character     */
    pscr    = (INT8U far *)MK_FP(DISP_BASE, offset);
    while (*s) {
        *pscr++ = *s++;                                     /* Put character in video RAM              */
        *pscr++ = color;                                    /* Put video attribute in video RAM        */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                             RETURN TO DOS
*
* Description : This functions returns control back to DOS by doing a 'long jump' back to the saved
*               location stored in 'PC_JumpBuf'.  The saved location was established by the function
*               'PC_DOSSaveReturn()'.  After execution of the long jump, execution will resume at the 
*               line following the 'set jump' back in 'PC_DOSSaveReturn()'.  Setting the flag 
*               'PC_ExitFlag' to TRUE ensures that the 'if' statement in 'PC_DOSSaveReturn()' executes.
*
* Arguments   : None
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DOSReturn (void)
{
    PC_ExitFlag = TRUE;                                    /* Indicate we are returning to DOS         */
    longjmp(PC_JumpBuf, 1);                                /* Jump back to saved environment           */
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        SAVE DOS RETURN LOCATION
*
* Description : This function saves the location of where we are in DOS so that it can be recovered.
*               This allows us to abort multitasking under uC/OS-II and return back to DOS as if we had
*               never left.  When this function is called by 'main()', it sets 'PC_ExitFlag' to FALSE
*               so that we don't take the 'if' branch.  Instead, the CPU registers are saved in the
*               long jump buffer 'PC_JumpBuf' and we simply return to the caller.  If a 'long jump' is
*               performed using the jump buffer then, execution would resume at the 'if' statement and
*               this time, if 'PC_ExitFlag' is set to TRUE then we would execute the 'if' statements and
*               restore the DOS environment.
*
* Arguments   : None
*
* Returns     : None
*********************************************************************************************************
*/
void PC_DOSSaveReturn (void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    


    PC_ExitFlag  = FALSE;                                  /* Indicate that we are not exiting yet!    */
    OSTickDOSCtr =     1;                                  /* Initialize the DOS tick counter          */
    PC_TickISR   = PC_VectGet(VECT_TICK);                  /* Get MS-DOS's tick vector                 */
    
    PC_VectSet(VECT_DOS_CHAIN, PC_TickISR);                /* Store MS-DOS's tick to chain             */
    
    setjmp(PC_JumpBuf);                                    /* Capture where we are in DOS              */
    if (PC_ExitFlag == TRUE) {                             /* See if we are exiting back to DOS        */
        OS_ENTER_CRITICAL();
        PC_SetTickRate(18);                                /* Restore tick rate to 18.2 Hz             */
        OS_EXIT_CRITICAL();
        PC_VectSet(VECT_TICK, PC_TickISR);                 /* Restore DOS's tick vector                */
        PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);  /* Clear the display                        */
        exit(0);                                           /* Return to DOS                            */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       ELAPSED TIME INITIALIZATION
*
* Description : This function initialize the elapsed time module by determining how long the START and
*               STOP functions take to execute.  In other words, this function calibrates this module
*               to account for the processing time of the START and STOP functions.
*
* Arguments   : None.
*
* Returns     : None.
*********************************************************************************************************
*/
void PC_ElapsedInit(void)
{
    PC_ElapsedOverhead = 0;
    PC_ElapsedStart();
    PC_ElapsedOverhead = PC_ElapsedStop();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         INITIALIZE PC'S TIMER #2
*
* Description : This function initialize the PC's Timer #2 to be used to measure the time between events.
*               Timer #2 will be running when the function returns.
*
* Arguments   : None.
*
* Returns     : None.
*********************************************************************************************************
*/
void PC_ElapsedStart(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT8U      data;


    OS_ENTER_CRITICAL();
    data  = (INT8U)inp(0x61);                              /* Disable timer #2                         */
    data &= 0xFE;
    outp(0x61, data);
    outp(TICK_T0_8254_CWR,  TICK_T0_8254_CTR2_MODE0);      /* Program timer #2 for Mode 0              */
    outp(TICK_T0_8254_CTR2, 0xFF);
    outp(TICK_T0_8254_CTR2, 0xFF);
    data |= 0x01;                                          /* Start the timer                          */
    outp(0x61, data);
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                 STOP THE PC'S TIMER #2 AND GET ELAPSED TIME
*
* Description : This function stops the PC's Timer #2, obtains the elapsed counts from when it was
*               started and converts the elapsed counts to micro-seconds.
*
* Arguments   : None.
*
* Returns     : The number of micro-seconds since the timer was last started.
*
* Notes       : - The returned time accounts for the processing time of the START and STOP functions.
*               - 54926 represents 54926S-16 or, 0.838097 which is used to convert timer counts to
*                 micro-seconds.  The clock source for the PC's timer #2 is 1.19318 MHz (or 0.838097 uS)
*********************************************************************************************************
*/
INT16U PC_ElapsedStop(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT8U      data;
    INT8U      low;
    INT8U      high;
    INT16U     cnts;


    OS_ENTER_CRITICAL();
    data  = (INT8U)inp(0x61);                                    /* Disable the timer                  */
    data &= 0xFE;
    outp(0x61, data);
    outp(TICK_T0_8254_CWR, TICK_T0_8254_CTR2_LATCH);             /* Latch the timer value              */
    low  = inp(TICK_T0_8254_CTR2);
    high = inp(TICK_T0_8254_CTR2);
    cnts = (INT16U)0xFFFF - (((INT16U)high << 8) + (INT16U)low); /* Compute time it took for operation */
    OS_EXIT_CRITICAL();
    return ((INT16U)((INT32U)cnts * 54926L >> 16) - PC_ElapsedOverhead);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       GET THE CURRENT DATE AND TIME
*
* Description: This function obtains the current date and time from the PC.
*
* Arguments  : s     is a pointer to where the ASCII string of the current date and time will be stored.
*                    You must allocate at least 21 bytes (includes the NUL) of storage in the return 
*                    string.  The date and time will be formatted as follows:
*
*                        "YYYY-MM-DD  HH:MM:SS"
*
* Returns    : none
*********************************************************************************************************
*/
void PC_GetDateTime (char *s)
{
    struct time now;
    struct date today;


    gettime(&now);
    getdate(&today);
    sprintf(s, "%04d-%02d-%02d  %02d:%02d:%02d",
               today.da_year,
               today.da_mon,
               today.da_day,
               now.ti_hour,
               now.ti_min,
               now.ti_sec);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        CHECK AND GET KEYBOARD KEY
*
* Description: This function checks to see if a key has been pressed at the keyboard and returns TRUE if
*              so.  Also, if a key is pressed, the key is read and copied where the argument is pointing
*              to.
*
* Arguments  : c     is a pointer to where the read key will be stored.
*
* Returns    : TRUE  if a key was pressed
*              FALSE otherwise
*********************************************************************************************************
*/
BOOLEAN PC_GetKey (INT16S *c)
{
    if (kbhit()) {                                         /* See if a key has been pressed            */
        *c = (INT16S)getch();                              /* Get key pressed                          */
        return (TRUE);
    } else {
        *c = 0x00;                                         /* No key pressed                           */
        return (FALSE);
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                      SET THE PC'S TICK FREQUENCY
*
* Description: This function is called to change the tick rate of a PC.
*
* Arguments  : freq      is the desired frequency of the ticker (in Hz)
*
* Returns    : none
*
* Notes      : 1) The magic number 2386360 is actually twice the input frequency of the 8254 chip which
*                 is always 1.193180 MHz.
*              2) The equation computes the counts needed to load into the 8254.  The strange equation
*                 is actually used to round the number using integer arithmetic.  This is equivalent to
*                 the floating point equation:
*
*                             1193180.0 Hz
*                     count = ------------ + 0.5
*                                 freq
*********************************************************************************************************
*/
void PC_SetTickRate (INT16U freq)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT16U     count;


    if (freq == 18) {                            /* See if we need to restore the DOS frequency        */
        count = 0;
    } else if (freq > 0) {                        
                                                 /* Compute 8254 counts for desired frequency and ...  */
                                                 /* ... round to nearest count                         */
        count = (INT16U)(((INT32U)2386360L / freq + 1) >> 1); 
    } else {
        count = 0;
    }
    OS_ENTER_CRITICAL();
    outp(TICK_T0_8254_CWR,  TICK_T0_8254_CTR0_MODE3); /* Load the 8254 with desired frequency          */  
    outp(TICK_T0_8254_CTR0, count & 0xFF);            /* Low  byte                                     */
    outp(TICK_T0_8254_CTR0, (count >> 8) & 0xFF);     /* High byte                                     */
    OS_EXIT_CRITICAL();
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        OBTAIN INTERRUPT VECTOR
*
* Description: This function reads the pointer stored at the specified vector.
*
* Arguments  : vect  is the desired interrupt vector number, a number between 0 and 255.
*
* Returns    : The address of the Interrupt handler stored at the desired vector location.
*********************************************************************************************************
*/
void *PC_VectGet (INT8U vect)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT16U    *pvect;
    INT16U     off;
    INT16U     seg;
    
    
    pvect = (INT16U *)MK_FP(0x0000, vect * 4);        /* Point into IVT at desired vector location     */
    OS_ENTER_CRITICAL();
    off   = *pvect++;                                 /* Obtain the vector's OFFSET                    */
    seg   = *pvect;                                   /* Obtain the vector's SEGMENT                   */
    OS_EXIT_CRITICAL();
    return (MK_FP(seg, off));
}

/*
*********************************************************************************************************
*                                        INSTALL INTERRUPT VECTOR
*
* Description: This function sets an interrupt vector in the interrupt vector table.
*
* Arguments  : vect  is the desired interrupt vector number, a number between 0 and 255.
*              isr   is a pointer to a function to execute when the interrupt or exception occurs.
*
* Returns    : none
*********************************************************************************************************
*/
void PC_VectSet (INT8U vect, void (*isr)(void))
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
    INT16U    *pvect;
    
    
    pvect    = (INT16U *)MK_FP(0x0000, vect * 4);     /* Point into IVT at desired vector location     */
    OS_ENTER_CRITICAL();
    *pvect++ = (INT16U)FP_OFF(isr);                   /* Store ISR offset                              */
    *pvect   = (INT16U)FP_SEG(isr);                   /* Store ISR segment                             */
    OS_EXIT_CRITICAL();
}
