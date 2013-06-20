; $Id: os_cpu_a.asm,v 1.6 2002/02/07 01:08:53 linfusheng Exp $

	.386P						; Priviledged instructions can be used.


_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
_BSS	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS

; Put all segment directives into one single group (flat memory model).

GFLAT	GROUP _TEXT, _DATA, CONST, _BSS
		ASSUME	CS: GFLAT, DS: GFLAT, ES: GFLAT, SS: GFLAT

_TEXT	SEGMENT

;=======================================================================
; uCOS-II Functions
;=======================================================================



;-----------------------------------------------------------------------
; OSTickISR
;-----------------------------------------------------------------------

PUBLIC	_OSTickISR
EXTRN	_OSIntEnter:NEAR
EXTRN	_OSTimeTick:NEAR
EXTRN	_OSIntExit:NEAR

_OSTickISR	PROC NEAR

		pushad

; Send an end-of-interrupt to the i8259.

		mov	al,20h
		out	20h,al

; Standard uCOS processing.

		call	_OSIntEnter
		call	_OSTimeTick
		call	_OSIntExit

		popad
		iretd

_OSTickISR ENDP




;-----------------------------------------------------------------------
; OSStartHighRdy
;-----------------------------------------------------------------------

PUBLIC	_OSStartHighRdy
EXTRN	_OSTaskSwHook:NEAR
EXTRN	_OSRunning:BYTE
EXTRN	_OSTCBHighRdy:DWORD

_OSStartHighRdy PROC NEAR


	; Call OSTaskSwHook();

		call	_OSTaskSwHook

	; Increment OSRunning by 1 (the documentation says to set it to 1,
	; but the ix86l example increments it).

		inc		[_OSRunning]

	; Load the processor stack pointer with OSTCBHighRdy->OSTCBStkPtr

		mov	eax,[_OSTCBHighRdy]	; Point to TCB of highest priority task ready to run
		mov	esp,[eax]			; ESP = OSTCBHighRdy->OSTCBStkPtr

	; Pop all the processor registers from the stack

		popad

	; Execute a Return from interrupt intruction;

		iretd

_OSStartHighRdy ENDP

;-----------------------------------------------------------------------
; OSCtxSw
;-----------------------------------------------------------------------

PUBLIC	_OSCtxSw
EXTRN	_OSTCBCur:DWORD
EXTRN	_OSTaskSwHook:NEAR
EXTRN	_OSPrioHighRdy:BYTE
EXTRN	_OSPrioCur:BYTE
EXTRN	_OSTCBHighRdy:DWORD

_OSCtxSw	PROC NEAR

	; PUSH processor registers onto the current task's stack

		pushad

	; Save the stack pointer into OSTCBCur->OSTCBStkPtr

		mov	eax,[_OSTCBCur]
		mov	[eax],esp			; Stack pointer is ESP

	; Call OSTaskSwHook();

		call	_OSTaskSwHook

	; OSPrioCur = OSPrioHighRdy

		mov	al,[_OSPrioHighRdy] ; AL is OSPrioHighRdy
		mov	[_OSPrioCur],al

	; OSTCBCur = OSTCBHighRdy

		mov	eax,[_OSTCBHighRdy]	; EAX is OSTCBHighRdy
		mov	[_OSTCBCur],eax

	; Load the processor stack pointer with OSTCBHighRdy->OSTCBStkPtr
	; Note that EAX is still OSTCBHighRdy.

		mov	esp,[eax]			; ESP = OSTCBHighRdy->OSTCBStkPtr

	; Pop all the processor registers from the stack

		popad

	; Execute a Return from interrupt intruction;

		iretd

_OSCtxSw	ENDP

;-----------------------------------------------------------------------
; OSIntCtxSw
;-----------------------------------------------------------------------

PUBLIC	_OSIntCtxSw
EXTRN	_OSTCBCur:DWORD
EXTRN	_OSTaskSwHook:NEAR
EXTRN	_OSPrioHighRdy:BYTE
EXTRN	_OSPrioCur:BYTE
EXTRN	_OSTCBHighRdy:DWORD

_OSIntCtxSw PROC NEAR

	; Adjust the stack pointer to remove call to OsIntExit(), locals in 
	; OsIntExit() and the call to OSIntCtxSw();

;IFDEF _SAVE_INT_STATUS
		add	esp,12			; Ignore calls to OSIntExit, PUSHFD and OSIntCtxSw
;ELSE
;		add	esp,8			; Ignore calls to OSIntExit and OSIntCtxSw
;ENDIF

	; Save the stack pointer into OSTCBCur->OSTCBStkPtr

		mov	eax,[_OSTCBCur]
		mov	[eax],esp			; Stack pointer is ESP

	; Call OSTaskSwHook();

		call	_OSTaskSwHook

	; OSPrioCur = OSPrioHighRdy

		mov	al,[_OSPrioHighRdy] ; AL is OSPrioHighRdy
		mov	[_OSPrioCur],al

	; OSTCBCur = OSTCBHighRdy

		mov	eax,[_OSTCBHighRdy]	; EAX is OSTCBHighRdy
		mov	[_OSTCBCur],eax

	; Load the processor stack pointer with OSTCBHighRdy->OSTCBStkPtr
	; Note that EAX is still OSTCBHighRdy.

		mov	esp,[eax]			; ESP = OSTCBHighRdy->OSTCBStkPtr

	; Pop all the processor registers from the stack

		popad

	; Execute a Return from interrupt intruction;

		iretd

_OSIntCtxSw ENDP

;=======================================================================
; x86 Helper Functions.
;=======================================================================

;-----------------------------------------------------------------------
; void DefIntHandler();
;
; Default interrupt handler. It simply performs an interrupt return.
; It is required to initialize all interrupt entries in the IDT.
;-----------------------------------------------------------------------

PUBLIC		_DefIntHandler
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		call	_OSDefault_interrupt
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al


		popad
		iretd

_DefIntHandler	ENDP



; 中断 0
PUBLIC		_DefIntHandler0
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt_i:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler0	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0h
		push	eax
		call	_OSDefault_interrupt_i
		add		sp,4
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al


		popad
		iretd

_DefIntHandler0	ENDP



;中断1
PUBLIC		_DefIntHandler1
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler1	PROC NEAR

		pushad


; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,01h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler1	ENDP

;中断2
PUBLIC		_DefIntHandler2
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler2	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,02h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al


		popad
		iretd

_DefIntHandler2	ENDP



;中断3
PUBLIC		_DefIntHandler3
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler3	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,03h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler3	ENDP


;中断4
PUBLIC		_DefIntHandler4
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler4	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,04h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler4	ENDP



;中断5
PUBLIC		_DefIntHandler5
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler5	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,05h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler5	ENDP


;中断6
PUBLIC		_DefIntHandler6
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler6	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,06h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler6	ENDP


;中断7
PUBLIC		_DefIntHandler7
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler7	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,07h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler7	ENDP


;中断8
PUBLIC		_DefIntHandler8
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler8	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,08h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler8	ENDP



;中断9
PUBLIC		_DefIntHandler9
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler9	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,09h
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler9	ENDP


;中断10
PUBLIC		_DefIntHandler10
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler10	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0ah
		push	eax
		call	_OSDefault_interrupt
		add		sp,4h
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler10	ENDP





;中断11
PUBLIC		_DefIntHandler11
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler11	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0bh
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler11	ENDP

;中断12
PUBLIC		_DefIntHandler12
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler12	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0ch
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler12	ENDP


;中断13
PUBLIC		_DefIntHandler13
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler13	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0dh
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler13	ENDP



;中断14
PUBLIC		_DefIntHandler14
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler14	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0eh
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler14	ENDP




;中断15
PUBLIC		_DefIntHandler15
EXTRN		_OSIntEnter:NEAR
EXTRN		_OSDefault_interrupt:NEAR
EXTRN		_OSIntExit:NEAR
_DefIntHandler15	PROC NEAR

		pushad

; Standard uCOS processing.

		call	_OSIntEnter
		mov		eax,0fh
		push	eax
		call	_OSDefault_interrupt
		add		sp,4		
		call	_OSIntExit

; Send an end-of-interrupt to the i8259.
		mov	al,20h
		out	20h,al

		mov al,20h
		out 0a1h,al

		popad
		iretd

_DefIntHandler15	ENDP
;-----------------------------------------------------------------------
; void DumpStackHandler();
;
; This is usefull handler that dumps the last three DWORDS pushed
; on the stack. This is convenient in a fault handler to determine the
; source of the fault. The function never returns.
;
; _DEBUG must be defined for this code to be effective.
;-----------------------------------------------------------------------

IFDEF _DEBUG

PUBLIC		_DumpStackHandler
EXTERN		_ShowRegister:NEAR

_DumpStackHandler PROC NEAR

		mov	eax,[esp]		; EAX: last pushed DWORD
		push	eax				
		call	_ShowRegister		; Show it
		add	esp,4

		mov	eax,[esp+4]		; EAX: previous pushed DWORD
		push	eax
		call	_ShowRegister		; Show it
		add	esp,4

		mov	eax,[esp+8]		; EAX: 2nd previous pushed DWORD
		push	eax
		call	_ShowRegister
		add	esp,4

; Enter an infinite loop. It is likely that things went wrong and
; continuing could reset the PC, losing whatever is on the screen.

Here:
		jmp	Here

_DumpStackHandler ENDP

ENDIF

_TEXT		ENDS
		END


