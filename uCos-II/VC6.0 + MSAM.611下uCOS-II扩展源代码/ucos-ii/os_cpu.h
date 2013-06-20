// oc_cpu.h

///////////////////////////////////////////////////////////////////////////////
// uCOS-II standard definitions and declarations.

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT				extern
#endif

typedef unsigned char			BOOLEAN;
typedef unsigned char			INT8U;
typedef char					INT8S;
typedef unsigned short			INT16U;
typedef short					INT16S;
typedef unsigned long			INT32U;
typedef long					INT32S;
typedef float					FP32;
typedef double					FP64;

typedef INT32U					OS_STK;

#define  OS_ENTER_CRITICAL()	__asm	PUSHFD	__asm CLI
#define  OS_EXIT_CRITICAL()		__asm	POPFD

#define  OS_STK_GROWTH			1
#define  OS_TASK_SW()			__asm	INT	uCOS

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

#define	CS_SELECTOR				0x08	// CS selector in GDT
#define uCOS					0x30    // uC/OS-II Interrupt

// Prototypes to function declared in ix86p\os_cpu_c.c

extern void				OSCpuInit();

// Prototypes to function declared in ix86p\os_cpu_a.asm

extern INT8U			inportb(INT16U PortNo);
extern void				outportb(INT16U PortNo, INT8U Value);
extern void				DefIntHandler();
extern void				DumpStackHandler();
