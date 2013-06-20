/* 
 * $Id: MyTask.c,v 1.8 2002/02/21 08:25:00 linfusheng Exp $
 */

#include "includes.h"
#include "ether.h"
#include "pci.h"
#include "serial.h"
#include "nic.h"
#include "command.h"
#include "interrupt.h"
#include "br.h"

#define	TASK_STK_SIZE	1024							// Stack size, in bytes

OS_STK	NET_TaskStk[TASK_STK_SIZE];	// Tasks stacks
OS_STK	SHELL_TaskStk[TASK_STK_SIZE];

extern void sjy22_init(void);

// Prototypes

void	Shell(void *pParam);
void	Net_do(void *pParam);
void	Sleep(INT16U uSec);


// Application entry point, invoked from Entry.obj

void main()
{

	INT8U	error = 0;



	// Set the top of stack.
	__asm	MOV	ESP,0A0000h;

	// Display a banner.
	debug_print("Welcome to Micro-C/OS-II on x86 in protected mode\r\n!");

	// Initialize uC/OS.
	OSCpuInit();
	OSInit();
	init_intr_table();
	// Initialize extension 
	OSport_init();
	init_pci_bus();


//	dump_pci_device();

	br_init();

	netif_init();	
	sjy22_init();
	// Create tasks.
	if( error = OSTaskCreateExt(Net_do, 
				    NULL, 
#if OS_STK_GROWTH == 1
				    &NET_TaskStk[TASK_STK_SIZE-1],
#else
				    &NET_TaskStk[0],
#endif
				    NET_PRIO,
				    0,
#if OS_STK_GROWTH == 1
				    &NET_TaskStk[0],
#else
				    &NET_TaskStk[TASK_STK_SIZE-1],
#endif
				    TASK_STK_SIZE,
				    NULL,
				    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR)
	){

		debug_print("create net task return %d error code . \r\n",error);
	}


	// Create tasks.
	if( error = OSTaskCreateExt(Shell, 
				    NULL, 
#if OS_STK_GROWTH == 1
				    &SHELL_TaskStk[TASK_STK_SIZE-1],
#else
				    &SHELL_TaskStk[0],
#endif
				    SHELL_PRIO,
				    1,
#if OS_STK_GROWTH == 1
				    &SHELL_TaskStk[0],
#else
				    &SHELL_TaskStk[TASK_STK_SIZE-1],
#endif
				    TASK_STK_SIZE,
				    NULL,
				    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR)
	){
		debug_print("create shell task return %d error code . \r\n",error);
	}


	// Start multitasking.
	OSStart();

	/* NEVER EXECUTED */
}

void Net_do(void *pParam)
{
	debug_print("process Net_do task ... \r\n");


	while(1) {
		netif_bh();
	}
}

void Shell(void * pParam)
{
        int     cc;
        char    buf[CMDLEN];
	debug_print("Shell task ... \r\n");

	while (TRUE) {
#define PROMPT	"UCOS> "
                debug_print("%s",PROMPT);
#undef  PROMPT
		/*
		 从串口获得数据
		*/
		cc = OSport_gets(SERIAL_PORT_1,buf,CMDLEN);

		if(cc) {

			while ((cc > 0) && isblank(buf[cc - 1]))
				cc--;
			buf[cc] = '\0';

			if(command(buf) == FALSE) {
				unsigned short l = 0xff11;
				unsigned long ll = 0x11223344;
				debug_print("Error command . \r\n");
//				debug_print_buf("input buf",buf,cc);	
					
			}
		}
	}

	return ;
}


// Sleeps for the specified delay (in seconds).
// Since the 8254 is not set in this implementation,
// sleeping one second means sleeping 18 clock ticks.

void Sleep(INT16U uSec)
{
	OSTimeDly((INT16U) (18 * uSec));
}
/* 
 * $Log: MyTask.c,v $
 * Revision 1.8  2002/02/21 08:25:00  linfusheng
 * update
 *
 * Revision 1.7  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.6  2002/02/06 07:28:51  linfusheng
 * update
 *
 * Revision 1.5  2002/02/06 02:16:25  linfusheng
 * update
 *
 * Revision 1.4  2002/02/05 05:26:03  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:46:25  linfusheng
 * update
 *
 */
