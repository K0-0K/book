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
* 文件名 : OS_CPU.H
* 作者   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                       数据类型
*                                     (编译器相关)
*********************************************************************************************************
*/

//详见C51.PDF第176页
typedef unsigned char  BOOLEAN;       //注意：不要使用bit定义，因为在结构体里无法使用
typedef unsigned char  INT8U;         //无符号8位数
typedef signed   char  INT8S;         //有符号8位数
typedef unsigned int   INT16U;        //无符号16位数
typedef signed   int   INT16S;        //有符号16位数
typedef unsigned long  INT32U;        //无符号32位数
typedef signed   long  INT32S;        //有符号32位数
typedef float          FP32;          //单精度浮点数
typedef double         FP64;          //双精度浮点数

typedef unsigned char  OS_STK;        //栈单元宽度为8比特

#define BYTE           INT8S          //兼容以前版本的数据类型
#define UBYTE          INT8U          //uC/OS-II可以不用这些数据类型
#define WORD           INT16S
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U

/* 
*********************************************************************************************************
*                              处理器相关代码 MCU-51 (大模式)
* 针对51单片机，只使用方法1(直接开关中断)
*********************************************************************************************************
*/

#define  OS_ENTER_CRITICAL()  EA=0    //关中断

#define  OS_EXIT_CRITICAL()   EA=1    //开中断

#define  OS_STK_GROWTH    0           //MCU-51堆栈从下往上增长  1=向下，0=向上

#define  OS_TASK_SW()     OSCtxSw()   //因为MCU-51没有软中断指令，所以用程序调用代替。两者的堆栈格式相同，
                                      //RETI指令复位中断系统，RET则没有。实践表明，对于MCU-51，用子程序调
                                      //用入栈，用中断返回指令RETI出栈是没有问题的，反之中断入栈RET出栈则
                                      //不行。总之，对于入栈，子程序调用与中断调用效果是一样的，可以混用。
                                      //在没有中断发生的情况下复位中断系统也不会影响系统正常运行。
                                      //详见《uC/OS-II》第八章193页第12行
                                      