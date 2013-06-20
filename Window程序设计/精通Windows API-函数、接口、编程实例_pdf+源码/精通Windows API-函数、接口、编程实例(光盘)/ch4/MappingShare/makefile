# Nmake macros for building Windows 32-Bit apps

!include <Win32.Mak>

src1 = pro_s1
src2 = pro_s2

all: $(OUTDIR) $(OUTDIR)\$(src1).exe $(OUTDIR)\$(src2).exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OUTDIR)\$(src1).obj: $(src1).c
    $(cc) $(cflags) $(cvars)  /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $(src1).c

$(OUTDIR)\$(src1).exe: $(OUTDIR)\$(src1).obj
    $(link) $(conflags) -out:$(OUTDIR)\$(src1).exe $(OUTDIR)\$(src1).obj $(conlibs)
    
$(OUTDIR)\$(src2).obj: $(src2).c
    $(cc) $(cflags) $(cvars)  /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $(src2).c

$(OUTDIR)\$(src2).exe: $(OUTDIR)\$(src2).obj
    $(link) $(conflags) -out:$(OUTDIR)\$(src2).exe $(OUTDIR)\$(src2).obj $(conlibs)

#--------------------- Clean Rule --------------------------------------------------------
# Rules for cleaning out those old files
clean:
        $(CLEANUP)
