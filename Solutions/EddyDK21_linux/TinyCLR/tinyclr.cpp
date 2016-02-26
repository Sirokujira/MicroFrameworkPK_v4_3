////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyclr_application.h>
#include <tinyhal.h>
#include <TinyCLR_Interop.h>
#include <TinyCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////
void ApplicationEntryPoint()
{
    CLR_SETTINGS clrSettings;

	lcd_printf(   "LCD: tinyCLR: ApplicationEntryPoint...\r\n" );
	hal_printf(   "HAL: tinyCLR: ApplicationEntryPoint...\r\n" );
	debug_printf( "DBG: tinyCLR: ApplicationEntryPoint...\r\n" );

    // CLR entry point
    memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches         = 50;
    clrSettings.WaitForDebugger            = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

#if defined(__EVENT_DEBUG__)
	for(int i = 0; i < 2; i++) 
	{
		hal_printf  ( "HAL: Hello, world!\r\n" );
		debug_printf( "HAL: Hello, world!\r\n" );
		HAL_Time_Sleep_MicroSeconds(1000 * 1000);
	}

	for(int i = 0; i < 2; i++) 
	{
		hal_printf  ( "EVT: Hello, world!\r\n" );
		debug_printf( "EVT: Hello, world!\r\n" );
		Events_WaitForEvents( 0, 1000 );
	}
#endif
#if !defined(BUILD_RTM)
    debug_printf( "ClrStartup.\r\n" );
#endif
    ClrStartup( clrSettings );

#if !defined(BUILD_RTM)
    debug_printf( "Exiting.\r\n" );
#else
    ::CPU_Reset();
#endif
}


BOOL Solution_GetReleaseInfo(MfReleaseInfo& releaseInfo)
{
    MfReleaseInfo::Init(releaseInfo,
                        VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_REVISION,
                        OEMSYSTEMINFOSTRING, hal_strlen_s(OEMSYSTEMINFOSTRING)
                        );
    return TRUE; // alternatively, return false if you didn't initialize the releaseInfo structure.
}

//
// Those are to avoid undef loop as below
//

extern const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[];
void *_CLR_InteropAssembliesNativeData()
{
	return((void *)g_CLR_InteropAssembliesNativeData);
}

//extern CLR_UINT32 g_scratchVirtualMethodTableLink     [];
void *_scratchVirtualMethodTableLink()
{
	return((void *)g_scratchVirtualMethodTableLink);
}

size_t _LinkArraySize   () { return (LinkArraySize   ()); } 
size_t _LinkMRUArraySize() { return (LinkMRUArraySize()); } 
size_t _PayloadArraySize() { return (PayloadArraySize()); }
size_t _InterruptRecords() { return (InterruptRecords()); }
