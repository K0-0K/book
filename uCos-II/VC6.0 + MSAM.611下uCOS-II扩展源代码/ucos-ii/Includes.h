// includes.h

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <conio.h>
#include    <dos.h>
#include    <setjmp.h>

#include    "os_cpu.h"			// For ix86p
#include    "os_cfg.h"
#include    "ucos_ii.h"

#define  OS_GLOBALS                           /* Declare GLOBAL variables                              */
#define  OS_MASTER_FILE                       /* Prevent the following files from including includes.h */
