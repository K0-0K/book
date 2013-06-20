# Nmake macros for building Windows 32-Bit apps

!include <Win32.Mak>

##--------------------- Build Rule -------------------------------------------------------
all: $(OUTDIR) $(OUTDIR)\start.exe

#----- If OUTDIR does not exist, then create directory
$(OUTDIR):
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

#----- compile ---------------------------------------
$(OUTDIR)\start.obj: start.c
    cl -c -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo -GS -D_X86_=1  -DWIN32 -D_WIN32 -W3 -D_WINNT -D_WIN32_WINNT=0x0500 -D_WIN32_IE=0x0500 -DWINVER=0x0500   -D_MT -MTd /Gz /Fo"START_BIN\\" /Fd"START_BIN\\" start.c
#----- link ------------------------------------------
$(OUTDIR)\start.exe: $(OUTDIR)\start.obj
		link /INCREMENTAL:NO /NOLOGO -subsystem:windows,5.0 -out:START_BIN\start.exe START_BIN\start.obj kernel32.lib  ws2_32.lib mswsock.lib advapi32.lib bufferoverflowu.lib

##-------------------- Clean Rule --------------------------------------------------------
# Rules for cleaning out those old files
clean:
        if exist $(OUTDIR) rd /s /q $(OUTDIR)
