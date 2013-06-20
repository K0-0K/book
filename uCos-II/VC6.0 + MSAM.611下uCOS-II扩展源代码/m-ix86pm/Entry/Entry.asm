; Entry.asm
; Copyright (C) 1998, Exposé Corp. All rights reserved.
;
; Description:
;	This code switches the CPU into the protected mode and initializes 
;	the registers to a flat 4GB address space, where linear addresses
;	equal physical addresses.
;
;	It then calls main() to run any linked C program that has been compiled 
;	to run in a 32-bit flat environment with a base address of 0.
;
;	This code is part of the port of uCOS (C) to the Intel 80x86 running
;	in Protected Mode. A full description of the port can be found in 
;	Port86pm.doc
;
; Build Commands:
;	This program requires Microsoft Assembler 6.11.
;
;	1. Execute the makefile by typing:
;		nmake
;
;	- or -
;
;	2. Type the following command:
;		ml /c /Fl /coff Entry
;
; Questions, comments and suggestions:
;	Please send e-mail to jean.gareau@exposecorp.com

		.386P				; Use 386+ privileged instructions

;-----------------------------------------------------------------------------;
; Macros (to use 32-bit instructions while in real mode)		      ;
;-----------------------------------------------------------------------------;

LGDT32		MACRO	Addr			; 32-bit LGDT Macro in 16-bit
		DB	66h			; 32-bit operand override
		DB	8Dh			; lea (e)bx,Addr
		DB	1Eh
		DD	Addr
		DB	0Fh			; lgdt fword ptr [bx]
		DB	01h
		DB	17h
ENDM

LIDT32		MACRO	Addr			; 32-bit LIDT Macro in 16-bit
		DB	66h			; 32-bit operand override
		DB	8Dh			; lea (e)bx,Addr
		DB	1Eh
		DD	Addr
		DB	0Fh			; lidt fword ptr [bx]
		DB	01h
		DB	1Fh
ENDM

FJMP32		MACRO	Selector,Offset		; 32-bit Far Jump Macro in 16-bit
		DB	66h			; 32-bit operand override
		DB	0EAh			; far jump
		DD	Offset			; 32-bit offset
		DW	Selector		; 16-bit selector
ENDM

		PUBLIC	EntryPoint		; The linker needs it.
		PUBLIC	_Idt
		PUBLIC	_Gdt
		EXTERN	_main:NEAR

_TEXT		SEGMENT PARA USE32 PUBLIC 'CODE'
		ASSUME	CS:_TEXT, DS:_TEXT, ES:_TEXT, SS:_TEXT

;		ORG	1000h			; => Depends on loader. <=

;-----------------------------------------------------------------------------;
; Entry Point. The CPU is executing in 16-bit real mode.		      ;
;-----------------------------------------------------------------------------;

EntryPoint	PROC	NEAR

		LGDT32	GdtDesc			; Load GDT descriptor
		LIDT32	IdtDesc			; Load IDT descriptor

		mov	eax,cr0			; Get control register 0
		or	ax,1			; Set PE bit (bit #0) in (e)ax
		mov	cr0,eax			; Activate protected mode!
		jmp	$+2			; To flush the instruction queue.

; The CPU is now executing in 16-bit protected mode. Make a far jump in order to
; load CS with a selector to a 32-bit executable code descriptor.

		FJMP32	08h,EntryPoint32	; Jump to EntryPoint32

; This point is never reached.

EntryPoint	ENDP

;-----------------------------------------------------------------------------;
; The CPU is now executing in 32-bit protected mode.			      ;
;-----------------------------------------------------------------------------;

EntryPoint32	PROC	NEAR

; Initialize all segment registers to 10h (entry #2 in the GDT)

		mov	ax,10h			; entry #2 in GDT
 		mov	ds,ax			; ds = 10h
 		mov	es,ax			; es = 10h
 		mov	fs,ax			; fs = 10h
 		mov	gs,ax			; gs = 10h
 		mov	ss,ax			; ss = 10h
 
; Set the top of stack to allow stack operations.
  
 		mov	esp,8000h		; arbitrary top of stack
 
; Call main(), which is not expected to return.

		call	_main

; In case main() returns, enter an infinite loop.

IdleLoop:
		hlt
		jmp	IdleLoop

; This point is never reached.

EntryPoint32	ENDP

;-------------------------------------------------------------------------------;
; Tables Descriptors (to use with LGDT32 & LIDT32)				;
;-------------------------------------------------------------------------------;

		ALIGN	4

GdtDesc:					; GDT descriptor
		DW	GDT_SIZE - 1		; GDT limit
 		DD	_Gdt			; GDT base address (below)

		ALIGN	4

IdtDesc:					; IDT descriptor
		DW	IDT_SIZE - 1		; IDT limit
 		DD	_Idt			; IDT base address (below)

;-------------------------------------------------------------------------------;
; GDT										;	
;-------------------------------------------------------------------------------;

		ALIGN	4

; Global Descriptor Table (GDT)

_Gdt:
 
; GDT[0]: Null entry, never used.
     
 		DD	0
 		DD	0
 
; GDT[1]: Executable, read-only code, base address of 0, limit of FFFFFh, 
;	  granularity bit (G) set (making the limit 4GB)
 
 		DW	0FFFFh			; Limit[15..0]
 		DW	0000h			; Base[15..0]
 		DB	00h			; Base[23..16]
 		DB	10011010b		; P(1) DPL(00) S(1) 1 C(0) R(1) A(0)
 		DB	11001111b		; G(1) D(1) 0 0 Limit[19..16]
 		DB	00h			; Base[31..24]
 		
; GDT[2]: Writable data segment, covering the save address space than GDT[1].
 
 		DW	0FFFFh			; Limit[15..0]
 		DW	0000h			; Base[15..0]
 		DB	00h			; Base[23..16]
 		DB	10010010b		; P(1) DPL(00) S(1) 0 E(0) W(1) A(0)
 		DB	11001111b		; G(1) B(1) 0 0 Limit[19..16]
 		DB	00h			; Base[31..24]
 
GDT_SIZE	EQU	$ - _Gdt		; Size, in bytes

;-------------------------------------------------------------------------------;
; IDT										;	
;-------------------------------------------------------------------------------;

		ALIGN	4

; Interrupt Descriptor Table (IDT)

GATE		STRUCT
		DD	?
		DD	?
GATE		ENDS

_Idt:
		GATE	32 dup (<0>)		; (00h-19h) Reserved for CPU interrupts
		GATE	16 dup (<0>)		; (20h-2Fh) Reserved for IRQ interrupts
		GATE	16 dup (<0>)		; (30h-3Fh) Reserved for OS interrupts

IDT_SIZE	EQU	$ - _Idt		; Size, in bytes

_TEXT		ENDS
		END
