ASM     = ml
AFLAGS  = /c /Fl /coff /nologo
CL      = cl 
CFLAGS  = /nologo /ML /W3 /GX /O2 /FD 
AR	= lib
ARFLAGS	= /MACHINE:IX86 
RM      = deltree
RMFLAGS = /y

INCLUDE =.;$(INCLUDE)
LIB     =


OBJS    = Os_core.obj Os_mbox.obj Os_mem.obj Os_q.obj Os_sem.obj Os_task.obj \
        Os_time.obj
TARGET  = ucos-ii.lib

########## Begin to Build #############
ALL: $(OBJS)
	$(AR) $(ARFLAGS) /OUT:$(TARGET) $(OBJS) 

clean:
        -@if exist *.obj $(RM) $(RMFLAGS) *.obj 
