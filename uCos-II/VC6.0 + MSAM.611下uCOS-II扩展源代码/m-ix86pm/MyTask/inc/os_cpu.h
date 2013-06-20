// oc_cpu.h

///////////////////////////////////////////////////////////////////////////////
// uCOS-II standard definitions and declarations.

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT				extern
#endif


#define  OS_ENTER_CRITICAL()	__asm	PUSHFD	__asm CLI
#define  OS_EXIT_CRITICAL()	__asm	POPFD

#define  OS_STK_GROWTH		1
#define  OS_TASK_SW()		__asm	INT	uCOS

#define	 OS_SAVE_ALLREG()	__asm   PUSHAD
#define  OS_RESTORE_ALLREG()	__asm   POPAD



///////////////////////////////////////////////////////////////////////////////
// Port-specific definitions and declarations

// Processor-dependent add-ons

typedef void *					PTR;

#pragma pack(push, 1)					// No padding please

typedef struct _idt_gate
{
	INT16U	Offset;
	INT16U	Selector;
	INT16U	Type;
	INT16U	Offset2;
} IDT_GATE;

#pragma pack(pop)

#define IDTGATE_TRAP			0x8f00	// Trap gate type
#define IDTDATE_INT				0x8e00	// Interrupt gate type
#define IDTGATE_TASK			0x8500	// Task gate type

#define OSIRQ_OFFSET				0x20

/*
    These are the port addresses of the 8259 Programmable Interrupt
    Controller (PIC).
*/
#define I8259_A0	0x020				// 8259 #1, port #1
#define I8259_A1	0x021				// 8259 #1, port #2
#define I8259_B0	0x0a0				// 8259 #2, port #1
#define I8259_B1	0x0a1				// 8259 #2, port #2

#define ICR_A         0x020   /* Interrupt Control Port       */
#define IMR_A         0x021   /* Interrupt Mask Register port */
#define ICR_B         0x0a0   /* Interrupt Control Port       */
#define IMR_B         0x0a1   /* Interrupt Mask Register port */

/*
    An end of interrupt needs to be sent to the Control Port of
    the 8259 when a hardware interrupt ends.
*/
#define EOI             0x20   /* End Of Interrupt */ 




#define	CS_SELECTOR	0x08	// CS selector in GDT
#define uCOS		0x30    // uC/OS-II Interrupt

// Prototypes to function declared in ix86p\os_cpu_c.c

extern void	OSCpuInit();

// Prototypes to function declared in ix86p\os_cpu_a.asm

extern INT8U	inportb(INT16U PortNo);
extern void	outportb(INT16U PortNo, INT8U Value);

extern void	DumpStackHandler();
extern void	SetIntVector(int IntNo, PTR Handler);



/* Flowwing is my define */

#define NET_PRIO	10
#define SHELL_PRIO	11

extern void	DefIntHandler();
extern void	DefIntHandler0(void);
extern void	DefIntHandler1(void);
extern void	DefIntHandler2(void);
extern void	DefIntHandler3(void);
extern void	DefIntHandler4(void);
extern void	DefIntHandler5(void);
extern void	DefIntHandler6(void);
extern void	DefIntHandler7(void);
extern void	DefIntHandler8(void);
extern void	DefIntHandler9(void);
extern void	DefIntHandler10(void);
extern void	DefIntHandler11(void);
extern void	DefIntHandler12(void);
extern void	DefIntHandler13(void);
extern void	DefIntHandler14(void);
extern void	DefIntHandler15(void);
