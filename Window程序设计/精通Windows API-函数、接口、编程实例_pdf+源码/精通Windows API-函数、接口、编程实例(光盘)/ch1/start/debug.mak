#----------------- 编译供调试的程序 ------------------
!include <Win32.Mak>

OUTDIR = START_DEBUG
#--------------------- 程序构建 ----------------------
all: $(OUTDIR) $(OUTDIR)\start.exe

$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

$(OUTDIR)\start.obj: start.c
    $(cc) $(cflags) /Gz /MT /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\vc80.pdb" /ZI /Od start.c

$(OUTDIR)\start.exe: $(OUTDIR)\start.obj
    $(link) /NOLOGO /subsystem:windows,5.0 /DEBUG /out:$(OUTDIR)\start.exe /PDB:"$(OUTDIR)\start.pdb" $(OUTDIR)\start.obj $(guilibs)

#--------------------- 程序清理 ----------------------
clean:
        $(CLEANUP)