# Nmake macros for building Windows 32-Bit apps

!include <Win32.Mak>

OUTDIR = START_BIN

all: $(OUTDIR) $(OUTDIR)\start.exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OUTDIR)\start.obj: start.c
    $(cc) $(cflags) $(cvars) /Gz /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" start.c

$(OUTDIR)\start.exe: $(OUTDIR)\start.obj
    $(link) $(guiflags) -out:$(OUTDIR)\start.exe $(OUTDIR)\start.obj $(conlibs)

#--------------------- Clean Rule --------------------------------------------------------
# Rules for cleaning out those old files
clean:
        $(CLEANUP)
