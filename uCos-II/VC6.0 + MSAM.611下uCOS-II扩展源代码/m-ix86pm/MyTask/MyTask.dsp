# Microsoft Developer Studio Project File - Name="MyTask" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MYTASK - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MyTask.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MyTask.mak" CFG="MYTASK - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MyTask - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\Entry\Entry.obj ..\bin\ucos-ii.lib /nologo /base:"0x00000000" /entry:"main" /pdb:none /map /machine:I386 /fixed
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Converting...
TargetPath=.\Release\MyTask.exe
InputPath=.\Release\MyTask.exe
SOURCE="$(InputPath)"

".\Release\MyTask.img" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\bin\ExeToImg $(TargetPath)

# End Custom Build
# Begin Target

# Name "MyTask - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\arp.c
# End Source File
# Begin Source File

SOURCE=.\src\br.c
# End Source File
# Begin Source File

SOURCE=.\src\command.c
# End Source File
# Begin Source File

SOURCE=.\src\debug.c
# End Source File
# Begin Source File

SOURCE=.\src\in_cksum.c
# End Source File
# Begin Source File

SOURCE=.\src\InetAddr.c
# End Source File
# Begin Source File

SOURCE=.\src\intel.asm
# Begin Custom Build
InputDir=.\src
TargetDir=.\Release
InputPath=.\src\intel.asm
InputName=intel

"$(TargetDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /coff /Fl$(TargetDir)\$(InputName).lst /Fo$(TargetDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build
# End Source File
# Begin Source File

SOURCE=.\src\interrupt.c
# End Source File
# Begin Source File

SOURCE=.\src\ip.c
# End Source File
# Begin Source File

SOURCE=.\src\MyTask.c
# End Source File
# Begin Source File

SOURCE=.\src\neticmp.c
# End Source File
# Begin Source File

SOURCE=.\src\nic.c
# End Source File
# Begin Source File

SOURCE=.\src\os_cpu_a.asm
# Begin Custom Build - Performing Custom Build Step on $(InputPath)
InputDir=.\src
TargetDir=.\Release
InputPath=.\src\os_cpu_a.asm
InputName=os_cpu_a

"$(TargetDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /coff /Fl$(TargetDir)\$(InputName).lst /Fo$(TargetDir)\$(InputName).obj $(InputDir)\$(InputName).asm

# End Custom Build
# End Source File
# Begin Source File

SOURCE=.\src\os_cpu_c.c
# End Source File
# Begin Source File

SOURCE=.\src\pci.c
# End Source File
# Begin Source File

SOURCE=.\src\rtl8139.c
# End Source File
# Begin Source File

SOURCE=.\src\serial.c
# End Source File
# Begin Source File

SOURCE=.\src\sjy.c
# End Source File
# Begin Source File

SOURCE=.\src\sjy22.c
# End Source File
# Begin Source File

SOURCE=.\src\ucos_timer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\inc\br.h
# End Source File
# Begin Source File

SOURCE=.\inc\cards.h
# End Source File
# Begin Source File

SOURCE=.\inc\command.h
# End Source File
# Begin Source File

SOURCE=.\inc\debug.h
# End Source File
# Begin Source File

SOURCE=.\inc\ether.h
# End Source File
# Begin Source File

SOURCE=.\inc\in_cksum.h
# End Source File
# Begin Source File

SOURCE=.\inc\Includes.h
# End Source File
# Begin Source File

SOURCE=.\inc\interrupt.h
# End Source File
# Begin Source File

SOURCE=.\inc\neticmp.h
# End Source File
# Begin Source File

SOURCE=.\inc\nic.h
# End Source File
# Begin Source File

SOURCE=.\inc\Os_cfg.h
# End Source File
# Begin Source File

SOURCE=.\inc\os_cpu.h
# End Source File
# Begin Source File

SOURCE=.\inc\pci.h
# End Source File
# Begin Source File

SOURCE=.\inc\pci_ids.h
# End Source File
# Begin Source File

SOURCE=.\inc\serial.h
# End Source File
# Begin Source File

SOURCE=.\inc\sjy22.h
# End Source File
# Begin Source File

SOURCE=.\inc\swap.h
# End Source File
# Begin Source File

SOURCE=.\inc\sysdep.h
# End Source File
# Begin Source File

SOURCE=.\inc\types.h
# End Source File
# Begin Source File

SOURCE=.\inc\ucos_ii.h
# End Source File
# Begin Source File

SOURCE=.\inc\ucos_io.h
# End Source File
# Begin Source File

SOURCE=.\inc\ucos_timer.h
# End Source File
# End Group
# End Target
# End Project
