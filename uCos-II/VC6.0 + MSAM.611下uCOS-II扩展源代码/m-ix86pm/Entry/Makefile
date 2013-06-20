# Makefile
#
# Constructs Entry.obj, the entry point for MyTask

all: Entry.obj

.asm.obj:
    ml /c /Fl /coff $*.asm

clean:
    deltree /y *.obj
