/*
*********************************************************************************************************
*                                               uC/OS-II
*                                               实时内核
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                               版权所有
*
*                                            MCU-51 专用代码
*                                           KEIL C51大模式编译
*
* 文件名 : INCLUDES.H
* 作者   : Jean J. Labrosse
*********************************************************************************************************
*/

//uC/OS-II核心
#include    <os_cpu.h>
#include    <os_cfg.h>
#include    <ucos_ii.h>

//用户头文件加在最后面
#include <reg52.h>
#include <absacc.h>
#include <intrins.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>