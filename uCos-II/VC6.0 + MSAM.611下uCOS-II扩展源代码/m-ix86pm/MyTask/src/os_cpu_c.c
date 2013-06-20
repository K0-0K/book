/*
 * $Id: os_cpu_c.c,v 1.5 2002/02/21 08:25:00 linfusheng Exp $
 */

#include "includes.h"
#include "interrupt.h"


extern void netif_slowtimo(void *arg);

// Prototypes
static void InitA20();
static void InitPIC();
static void SetIDTGate(int IntNo, INT16U Type, INT16U Selector, INT32U Offset);
static void TaskBucket();

#ifdef _DEBUG
extern void OnScreen(int x, int y, char * pszText, int nColor);
#endif

///////////////////////////////////////////////////////////////////////////////
// uCOS-II Functions

// OSCpuInit()
// Performs CPU-dependent intiailization.

void OSCpuInit()
{
	int	IntNo;

	InitA20();								// Enable address line 20
	InitPIC();								// Relocate IRQs to 0x20-0x2F

	// Install a default handler for all supported interrupts:
	//	a) 0x00-0x1F: Intel-reserved
	//	b) 0x20-0x2F: IRQ (relocated)
	//	c) 0x30-0x3F: Available for uCOS and application.

	for (IntNo = 0; IntNo < 0x40; IntNo++)
		SetIntVector(IntNo, DefIntHandler);
#ifdef _DEBUG
	// Install a dump stack handler for int 13 (General Protection Fault).
	SetIntVector(13, DumpStackHandler);
#endif

	SetIntVector(0x20, OSTickISR);			// Install the tick handler 
	SetIntVector(uCOS, OSCtxSw);			// Install uCOS-II's context switch handler

	SetIntVector(0x21, DefIntHandler1);		// Install the interrupt1 default handler 
	SetIntVector(0x22, DefIntHandler2);		// Install the interrupt2 default handler 
	SetIntVector(0x23, DefIntHandler3);		// Install the interrupt3 default handler 
	SetIntVector(0x24, DefIntHandler4);		// Install the interrupt4 default handler 
	SetIntVector(0x25, DefIntHandler5);		// Install the interrupt5 default handler 
	SetIntVector(0x26, DefIntHandler6);		// Install the interrupt6 default handler 
	SetIntVector(0x27, DefIntHandler7);		// Install the interrupt7 default handler 
	SetIntVector(0x28, DefIntHandler8);		// Install the interrupt8 default handler 
	SetIntVector(0x29, DefIntHandler9);		// Install the interrupt9 default handler 
	SetIntVector(0x2a, DefIntHandler10);		// Install the interrupt10 default handler 
	SetIntVector(0x2b, DefIntHandler11);		// Install the interrupt11 default handler 
	SetIntVector(0x2c, DefIntHandler12);		// Install the interrupt12 default handler 
	SetIntVector(0x2d, DefIntHandler13);		// Install the interrupt13 default handler 
	SetIntVector(0x2e, DefIntHandler14);		// Install the interrupt14 default handler 
	SetIntVector(0x2f, DefIntHandler15);		// Install the interrupt15 default handler 
}

// OSTaskStkInit()
// This is a uCOS-II function. Only the stack setup part needs some modifications.

void * OSTaskStkInit(void (*task)(void *pd), void *pdata, void *ptos, INT16U opt)
{
	OS_STK*	stk;

	stk = (OS_STK *) ptos;			// Load stack pointer

	*--stk = (INT32U) pdata;			// Simulate a function call (to pass the parameter)
	*--stk = (INT32U) TaskBucket;	// Return address in case the task exits.

	*--stk = 0x00000202;			// Eflags (interrupt flag enabled)
	*--stk = CS_SELECTOR;			// CS
	*--stk = (INT32U) task;			// Entry point

	*--stk = 0;						// EAX
	*--stk = 0;						// ECX
	*--stk = 0;						// EDX
	*--stk = 0;						// EBX
	*--stk = 0;						// ESP (unused)
	*--stk = 0;						// EBP
	*--stk = 0;						// ESI
	*--stk = 0;						// EDI

    return stk;
}

// OSTaskCreateHook
// This hook is invoked during task creation.

void OSTaskCreateHook(OS_TCB * pTcb)
{
}

// OSTaskDelHook
// This hook is invoked during task deletion.

void OSTaskDelHook(OS_TCB * pTcb)
{

}

// OSTaskSwHook
// This hook is invoked during a task switch.
// OSTCBCur points to the current task (being switched out).
// OSTCBHighRdy points on the new task (being switched in).

void OSTaskSwHook()
{
}

// OSTaskStatHook
// This hook is invoked by the statistical task every second.

void OSTaskStatHook()
{
}

// OSTimeTickHook
// This hook is invoked during a time tick.

void OSTimeTickHook()
{
	netif_slowtimo(NULL);
}

void OSDefault_interrupt_i(void)
{
}
void OSDefault_interrupt(long irq)
{
	void *arg;
	struct intrhand *ih = &intrhand[irq];
	if(ih->ih_irq != irq)
		return;
	arg = ih->ih_arg;
	if(ih->ih_fun) {
		ih->ih_fun(arg);
	}
}



///////////////////////////////////////////////////////////////////////////////
// Helper Functions

// InitA20. 
// Enable the address line 20.
//
// The address line 20 must be under the control of the CPU (not always 0 as it is 
// in real-mode). This is done by sending commands to the keyboard controller (i8042). 
// Before sending commands, the i8042 must be polled to ensure that it can receive data.

void InitA20()
{
	INT8U	status;

	// Wait until i8042 can receive the command.
	do 
	{
		status = _inp(0x64);
	} while (status & 2);

	// Send the 'write' command to the i8042.
	_outp(0x64, 0xd1);

	// Wait until i8042 can receive the command.
	do 
	{
		status = _inp(0x64);
	} while (status & 2);

	// Send the new set-up.
	_outp(0x60, 0xdf);

	// Wait until i8042 has received the command.
	do 
	{
		status = _inp(0x64);
	} while (status & 2);
}

// InitPIC()
// Relocate the IRQ lanes from 0x00 to 0x20 to prevent conflicts 
// between IRQ and CPU's exceptions.
//
// This is achieved by sending a series of command to the Intel 8259 
// Programmable Interrupt Controllers (PIC).

void InitPIC()
{
	// Reprogram the master 8259.
	_outp(I8259_A0, 0x11);

	_outp(I8259_A1, 0x20);
	_outp(I8259_A1, 0x04);
	_outp(I8259_A1, 0x01);
	_outp(I8259_A1, 0x00);

	// Reprogram the slave 8259.
	_outp(I8259_B0, 0x11);

	_outp(I8259_B1, 0x28);
	_outp(I8259_B1, 0x02);
	_outp(I8259_B1, 0x01);
	_outp(I8259_B1, 0x00);
}

// SetIntVector()
// Installs an interrupt handler.
 
void SetIntVector(int IntNo, PTR Offset)
{
	SetIDTGate(IntNo, IDTDATE_INT, CS_SELECTOR, (INT32U) Offset);
}

// SetIDTGate()
// Install a generic gate in the IDT.
//
// Parameters:
//	IntNo   : The IDT entry no (0x00-0x3F)
//	Type    : IDTGATE_INT, IDTGATE_TRAP or IDTGATE_TASK
//	Selector: Code selector, usually CS_SELECTOR
//	Offset  : Handler

void SetIDTGate(int IntNo, INT16U Type, INT16U Selector, INT32U Offset)
{
	extern IDT_GATE	Idt[];

	IDT_GATE *	pGate = &Idt[IntNo];

	pGate->Offset   = (INT16U) (Offset & 0xffff);
	pGate->Offset2  = (INT16U) (Offset >> 16);
	pGate->Selector = Selector;
	pGate->Type     = Type;
}

// TaskBucket()
// This is the function that is executed if a task inadvertedly
// returns from its main function. This is specified in OSTaskStkInit(),
// when a task's stack is being intialized.

void TaskBucket()
{
	while (1)
		;
}

// ShowRegister()
// A simple debug function that shows a INT32U, starting from the
// top left corner of the screen and going down each time it is invoked.
// It thus works 25 times for a 25-line monitor. It is useful to dump
// a few DWORDs from the stack at a given point (particularly during faults).
// Not sophisticated, but useful.

#ifdef _DEBUG
void ShowRegister(INT32U v)
{
	static int	i = 0;
	char	buffer[32];

	itoa(v, buffer, 16);

	OnScreen(0, i++, buffer, 14);
}
#endif



