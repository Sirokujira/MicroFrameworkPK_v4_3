////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Modified for Linux by Device Drivers Limited.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "LinuxAPI.h"

//--//

void HAL_Time_GetDriftParameters  ( INT32* a, INT32* b, INT64* c )
{
    *a = 1;
    *b = 1;
    *c = 0;
}

BOOL HAL_Time_Initialize()
{
	//// ************************** ////
	linux_Time_Init();
	//// ************************** ////
    return FALSE;
}

BOOL HAL_Time_Uninitialize()
{
	//// ************************** ////
	linux_Time_Exit();
	//// ************************** ////
    return FALSE;
}

UINT64 HAL_Time_CurrentTicks()
{
	//// ************************** ////
    return(linux_Time_CurrentTicks());
	//// ************************** ////
}

UINT64 Time_CurrentTicks()
{
    return HAL_Time_CurrentTicks();
}


INT64 HAL_Time_TicksToTime( UINT64 Ticks )
{
    return(CPU_TicksToTime(Ticks));
}

INT64 HAL_Time_CurrentTime()
{
	//// ************************** ////
    return(linux_Time_CurrentTicks() * 10);
	//// ************************** ////
}

void HAL_Time_SetCompare( UINT64 CompareValue )
{
	//// ************************** ////
    linux_Time_SetCompare(CompareValue);
	//// ************************** ////
}

void HAL_Time_Sleep_MicroSeconds( UINT32 uSec )
{
	//// ************************** ////
    linux_Time_Sleep_MicroSeconds(uSec);
	//// ************************** ////
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled( UINT32 uSec )
{
    HAL_Time_Sleep_MicroSeconds(uSec);
}


void HAL_Time_GetLocalTime( SYSTEMTIME *CurrentTime )
{
    // Does nothing for this processor - leaves the default time unchanged
}

void HAL_Time_SetLocalTime( const SYSTEMTIME *CurrentTime )
{
}

UINT64 CPU_MicrosecondsToTicks( UINT64 uSec );
UINT64 HAL_Time_MicrosecondsToTicks( INT64 ms )
{
    return(CPU_MicrosecondsToTicks((UINT64) ms));
}

UINT32 CPU_SystemClock()
{
    return SYSTEM_CLOCK_HZ;
}


UINT32 CPU_TicksPerSecond()
{
    return SLOW_CLOCKS_PER_SECOND;
}

UINT64 CPU_MillisecondsToTicks( UINT64 Ticks )
{
    Ticks *= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /= (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);

    return Ticks;
}

UINT64 CPU_MillisecondsToTicks( UINT32 Ticks32 )
{
    UINT64 Ticks;

    Ticks  = (UINT64)Ticks32 * (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /=                   (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);

    return Ticks;;
}

UINT64 CPU_MicrosecondsToTicks( UINT64 uSec )
{
#if ONE_MHZ < SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

UINT32 CPU_MicrosecondsToTicks( UINT32 uSec )
{
#if ONE_MHZ < SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

UINT32 CPU_MicrosecondsToSystemClocks( UINT32 uSec )
{
    uSec *= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ        /CLOCK_COMMON_FACTOR);

    return uSec;
}

UINT64 CPU_TicksToTime( UINT64 Ticks )
{
    Ticks *= (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);

    return Ticks;
}

UINT64 CPU_TicksToTime( UINT32 Ticks32 )
{
    UINT64 Ticks;

    Ticks  = (UINT64)Ticks32 * (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /=                   (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);

    return Ticks;
}
