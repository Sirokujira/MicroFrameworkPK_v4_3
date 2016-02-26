#ifdef PLATFORM_ARM_LINUX
#include "tinyclr_application.h"
#include "tinyhal.h"
#endif
#include "linuxAPI.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

#define DebuggerPort_Flush(p) \
	fflush(stdout)

#define DebuggerPort_Write(port, buffer, len ) \
	fwrite(buffer, len, 1, stdout)

#ifdef vsnprintf
#undef vsnprintf
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
void linux_assert(int i);
}
#endif

#ifdef __cplusplus
extern "C"
{
void MFabort(void);
}
#endif

#ifdef __cplusplus
extern "C"
{
void MFmain(int ac, char **av);
} // extern "C"
#endif

extern "C"
{
  int linux_vsnprintf(char *buffer, size_t size, char *format, va_list arg )
  {
	return(vsnprintf(buffer, size, format, arg));
  }
} // extern "C"

#if !defined(BUILD_RTM)
int linux_debug;
#endif
char *linux_assembly;
char TinyClr_Dat_Start[128*1024]; //128KB for Assembly
char TinyClr_Dat_End  [1       ];

#if !defined(BUILD_RTM)

void debug_printf(char const* format, ... )
{
    char    buffer[256];
    va_list arg_ptr;

    va_start( arg_ptr, format );

	int len = vsnprintf( buffer, sizeof(buffer)-1, format, arg_ptr );
    // flush existing characters
    DebuggerPort_Flush( USART_DEFAULT_PORT );

    // write string
    DebuggerPort_Write( USART_DEFAULT_PORT, buffer, len );

    // flush new characters
    DebuggerPort_Flush( USART_DEFAULT_PORT );

    va_end( arg_ptr );
}

void lcd_printf(char const * format,...)
{
    char    buffer[256];
    va_list arg_ptr;

    va_start( arg_ptr, format );

	int len = vsnprintf( buffer, sizeof(buffer)-1, format, arg_ptr );
    // flush existing characters
    DebuggerPort_Flush( USART_DEFAULT_PORT );

    // write string
    DebuggerPort_Write( USART_DEFAULT_PORT, buffer, len );

    // flush new characters
    DebuggerPort_Flush( USART_DEFAULT_PORT );

    va_end( arg_ptr );
}

#endif

/////////////////////////////////////////////////////////////////////

void InitCRuntime()
{
    //setlocale( LC_ALL, "" );
}

/////////////////////////////////////////////////////////////////////

#ifdef hal_strlen_s
#undef hal_strlen_s
int hal_strlen_s(char const*str)
{
    return(strlen(str));
}
#define hal_strlen_s( str ) strlen(str)
#endif

#ifdef hal_strncmp_s
#undef hal_strncmp_s
int hal_strncmp_s(const char* str1, const char* str2, size_t num )
{
    return(strncmp(str1, str2, num));
}
#define hal_strncmp_s( str1, str2, num ) strncmp(str1, str2, num)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
void linux_assert(int i)
{
	ASSERT(i);
}
#endif

#ifdef __cplusplus
void MFabort(void)
{
}
#endif


void* CustomHeap();
void* Heap();
size_t CustomHeapSize();
size_t HeapSize();

size_t HeapSize()       { return(linux_HeapSize()); }
size_t CustomHeapSize() { return(linux_CustomHeapSize()); }
void*  Heap()           { debug_printf(" H:%p\r\n", glinux_Heap);       return (glinux_Heap); }
void*  CustomHeap()     { debug_printf("CH:%p\r\n", glinux_CustomHeap); return (glinux_CustomHeap); }

// Dummy declaration
int HeapBegin;
int HeapEnd;
int CustomHeapBegin;
int CustomHeapEnd;

//-//
void MFmain(int ac, char **av)
{
	int size;
	int ReturnValue;

	linux_assembly = (char *) "TINYCLR.DAT";

	linux_load_options(ac, av);
	
	if (LINUXDBG > 0)
	{
		// LINUXDBG_print("debug:%d, CWD:%s, Assembly:%s \r\n", LINUXDBG, getcwd(NULL, 0), linux_assembly);
		// LINUXDBG_print("debug:%d, CWD:%s, Assembly:%s \r\n", LINUXDBG, _getcwd(NULL, 0), linux_assembly);
	}

	size = linux_loadAssembly(linux_assembly, TinyClr_Dat_Start);
	if (size <= 0)
	{
		LINUXDBG_print("##### MFmain: assembly load Error ! (%d) #####\r\n", size);
		//return;
	}
	if (LINUXDBG > 3)
	{
		LINUXDBG_print("Assembly:%s loaded size:%d\r\n", linux_assembly, size);
	}

	glinux_Heap = linux_allocHeap();
	if (NULL == glinux_Heap)
	{
		LINUXDBG_error("##### MFmain: allocHeap Error ! #####\r\n");
	}
	glinux_CustomHeap = linux_allocCustomHeap();
	if (NULL == glinux_CustomHeap)
	{
		LINUXDBG_error("##### MFmain: allocCustomHeap Error ! #####\r\n");
	}

REBOOT:
    if((ReturnValue = setjmp(glinux_ResetBuffer)) == 0)
    {
		InitCRuntime();

#ifdef PLATFORM_ARM_LINUX
		CPU_Initialize();

		HAL_Time_Initialize();

		HAL_Initialize();
#endif

		if (LINUXDBG > 0)
		{
			LINUXDBG_print("HAL: H=%p(%d), C=%p(%d)\r\n",
				glinux_Heap, linux_HeapSize(),
				glinux_CustomHeap, linux_CustomHeapSize());
		}
#ifdef PLATFORM_ARM_LINUX
		ApplicationEntryPoint();
#endif
		HAL_Uninitialize();
		HAL_Time_Uninitialize();
	}
	else if (ReturnValue == RESTART)
	{
		debug_printf("Restarted by longjmp()...\n");
		goto REBOOT;
	}
	else if (ReturnValue == SHUTDOWN)
	{
		debug_printf("Terminate by shutdown from longjmp()...\n");
	}
	else
	{
		debug_printf("Unknown terminate by longjmp()...\n");
	}
	LINUXDBG_print("MFmain Terminated.\n");

	linux_freeHeap(glinux_Heap);
	linux_freeHeap(glinux_CustomHeap);
}

///////////////////////////////////////////////////////////////////////////////////////////

// Helpers //

extern "C" {

	void periodic_timer()
	{
		SOCKETS_ProcessSocketActivity(SOCK_SOCKET_ERROR);

		// this also schedules the next one, if there is one
		HAL_COMPLETION::DequeueAndExec();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
