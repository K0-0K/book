# Nmake macros for building Windows 32-Bit apps

!include <Win32.Mak>

src = wr

all: $(OUTDIR) $(OUTDIR)\$(src).exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OUTDIR)\$(src).obj: $(src).c
    $(cc) $(cflags) $(cvars)  /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $(src).c

$(OUTDIR)\$(src).exe: $(OUTDIR)\$(src).obj
    $(link) $(conflags) -out:$(OUTDIR)\$(src).exe $(OUTDIR)\$(src).obj $(conlibs)

#--------------------- Clean Rule --------------------------------------------------------
# Rules for cleaning out those old files
clean:
        $(CLEANUP)
