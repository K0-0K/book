#--- 用户自定义宏 ---
OUT_DIR = subdir
WINDIR = OK

all:showpath $(OUT_DIR)\showmacro.exe recurssion

#---- 演示引用环境变量 ----
showpath:
	echo	shwo environment variable macro
	echo	WINDIR=$(WINDIR)
	echo	---

#--- 演示文件名宏 ---
$(OUT_DIR)\showmacro.exe:
	echo	show file name macro
	echo	$(@D)
	echo 	$(@F)
	
recurssion:
	echo ---
	$(MAKE) /N /$(MAKEFLAGS) /F s.mak showpath