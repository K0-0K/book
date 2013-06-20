# Microsoft Developer Studio Generated NMAKE File, Based on MyTask.dsp
!IF "$(CFG)" == ""
CFG=MYTASK - WIN32 RELEASE
!MESSAGE No configuration specified. Defaulting to MYTASK - WIN32 RELEASE.
!ENDIF 

!IF "$(CFG)" != "MyTask - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MyTask.mak" CFG="MYTASK - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MyTask - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\MyTask.exe" ".\Release\MyTask.img"


CLEAN :
	-@erase "$(INTDIR)\com.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\MyTask.obj"
	-@erase "$(INTDIR)\os_cpu_c.obj"
	-@erase "$(INTDIR)\pci.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MyTask.exe"
	-@erase "$(OUTDIR)\MyTask.map"
	-@erase ".\Release\MyTask.img"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MyTask.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=..\Entry\Entry.obj ..\bin\ucos-ii.lib /nologo /base:"0x00000000" /entry:"main" /pdb:none /map:"$(INTDIR)\MyTask.map" /machine:I386 /out:"$(OUTDIR)\MyTask.exe" /fixed 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\MyTask.obj" \
	"$(INTDIR)\os_cpu_c.obj" \
	"$(INTDIR)\pci.obj" \
	"$(INTDIR)\os_cpu_a.obj" \
	"$(INTDIR)\com.obj"

"$(OUTDIR)\MyTask.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\Release\MyTask.exe
InputPath=.\Release\MyTask.exe
SOURCE="$(InputPath)"

"$(OUTDIR)\MyTask.img" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	..\bin\ExeToImg $(TargetPath)
<< 
	
CPP_PROJ=/nologo /ML /W3 /GX /O2 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MyTask.dep")
!INCLUDE "MyTask.dep"
!ELSE 
!MESSAGE Warning: cannot find "MyTask.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MyTask - Win32 Release"
SOURCE=.\com.c

"$(INTDIR)\com.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\debug.c

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MyTask.c

"$(INTDIR)\MyTask.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os_cpu_a.asm
TargetDir=.\Release
InputPath=.\os_cpu_a.asm
InputName=os_cpu_a

"$(INTDIR)\os_cpu_a.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	ml /c /coff /Fl$(TargetDir)\$(InputName).lst /Fo$(TargetDir)\$(InputName).obj $(InputName).asm
<< 
	
SOURCE=.\os_cpu_c.c

"$(INTDIR)\os_cpu_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pci.c

"$(INTDIR)\pci.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

