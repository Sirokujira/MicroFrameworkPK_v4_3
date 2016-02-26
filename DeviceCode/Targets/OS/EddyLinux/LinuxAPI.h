////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Device Drivers, Ltd.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX_API_H_
#define _LINUX_API_H_

#include <tgmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <setjmp.h>

#if !defined(BUILD_RTM)
#define LINUX_DEBUG_LEVEL 5
#endif

#define LINUX_COM_ALWAYS_SER0 1

#define LINUX_DEFAULT_DBG_PORT 0
#define LINUX_DEFAULT_LCD_PORT 0
#define LINUX_DEFAULT_SER_PORT 0
#define LINUX_DEFAULT_TCP_PORT 0

#define HAL_COMPLETION_IDELEVALUE 0x000FFFFFFFFFFFFFull

#define LINUX_HEAP_SIZE ((1024*1024)+(256*1024)) //1.25MB,0x00140000
#define LINUX_CUSTOM_HEAP_SIZE 0

extern void *glinux_CustomHeap;
extern void *glinux_Heap;
extern jmp_buf glinux_ResetBuffer;
extern char *linux_assembly;

#define LINUXDBG_error(f,a...) linux_debug_printf("** ERROR ** %s: " f, __FUNCTION__, ##a)
#if !defined(BUILD_RTM)
extern int linux_debug;
#define LINUXDBG linux_debug
#define LINUXDBG_print(f,a...) linux_debug_printf("# %s: " f, __FUNCTION__, ##a)
#else
#define LINUXDBG 0
#define LINUXDBG_print(f,a...)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Timers
//
enum wakeup_actions
{
	INITIALIZE = 0,
    WAKEUP = 1,    /* Indicate current status to others */
    SLEEPING = 2,  /* Indicate current status to others */
    ALARM = 3,     /* External Instruction to set alarm */
    HIBERNATE = 4, /* External Instruction to hibernate, deep sleep */
    SHUTDOWN = 5,  /* External Instruction to exit, shutdown */
    RESTART = 6,   /* External Instruction to restart, reboot */
    EXIT = 9       /* External Instruction to stop this timer, terminate */
};

//
//	Debug Options //
//
extern int linux_opt_ticks;        //'T'
extern int linux_opt_ticks_number;
extern int linux_opt_wait;         //'W'
extern int linux_opt_wait_ms;
extern int linux_opt_assert;       //'E' or 'X'
//
//	C_syntax_APIs()... //
//
extern int linux_USART_Write( int ComPortNum, const char* Data, size_t size );
extern int linux_USART_Read( int ComPortNum, char* Data, size_t size );

extern double linux_ceil(double x);
extern double linux_pow(double x, double y);
extern double linux_floor(double x);
extern double linux_fmod(double x, double y);

extern void linux_Time_Init();
extern void linux_Time_Exit();
extern long long linux_Time_CurrentTicks();
extern void linux_Time_SetCompare(unsigned long long CompareValue);
extern void linux_Time_Sleep_MicroSeconds(unsigned long uSec);

extern void linux_CPU_Change_State(int status);
extern void linux_CPU_Restart(void);
extern void linux_CPU_Sleep(void);
extern void linux_CPU_Shutdown(void);

extern int  linux_InterruptsAreEnabled(void);
extern void linux_InterruptEnable(void);
extern void linux_InterruptDisable(void);

extern void*  linux_allocHeap(void);
extern void*  linux_allocCustomHeap(void);
extern size_t linux_HeapSize(void);
extern size_t linux_CustomHeapSize(void);
extern void   linux_freeHeap ( void*  ptr );

extern int    linux_loadAssembly(char *name, char *addr);
extern int    linux_load_options(int ac, char **av);

extern void linux_debug_printf( const char* format, ... );

extern void periodic_timer();

extern void linux_assert(int i);

#ifdef __cplusplus
}
#endif  // __cplusplus

//
//
//
// typedef unsigned int uint32_t;

#endif // _LINUX_API_H_
// EOF =======================================================================
