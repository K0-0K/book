;$Id: intel.asm,v 1.2 2002/02/07 01:08:53 linfusheng Exp $

;*************************************************************************
;  USAGE:  UINT intel16( UINT val )
;          - convert to intel format
;*************************************************************************

;×Ó³ÌÐò intel.asm

.386p
.model flat
_TEXT segment dword public use32 "CODE"

_intel16 proc near
push ebp
mov ebp,esp
mov ax,word ptr[ebp+8]
xchg al,ah
pop ebp
ret
_intel16 endp


_intel proc near
push ebp
mov ebp,esp
mov eax,[ebp+8]
xchg al,ah
ror	eax,16
xchg al,ah
pop ebp
ret
_intel endp


_TEXT ends

end

;$Log: intel.asm,v $
;Revision 1.2  2002/02/07 01:08:53  linfusheng
;update
;
;Revision 1.1  2002/01/28 01:19:34  linfusheng
;update
;¡¡¡¡