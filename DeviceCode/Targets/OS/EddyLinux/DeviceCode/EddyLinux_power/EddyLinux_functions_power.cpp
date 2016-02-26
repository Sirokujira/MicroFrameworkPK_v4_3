////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Modified for Linux by Device Drivers Limited.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "LinuxAPI.h"

//--//

void HAL_AssertEx()
{
#if !defined(BUILD_RTM)
	// cause an abort and let the abort handler take over
    //*((char*)0xFFFFFFFF) = 'a';

	static const char a[] = "@@@@@@@@@@@@@";
	if (LINUXDBG > 1)
	{
		DEBUG_PRINTF("%s%s%s\r\n", a, a, a);
		DEBUG_PRINTF("%s%s%s\r\n", a, a, a);
		DEBUG_PRINTF("%s A S S E R T ! %s\r\n", a, a);
		DEBUG_PRINTF("%s%s%s\r\n", a, a, a);
		DEBUG_PRINTF("%s%s%s\r\n", a, a, a);
	}
	else
	{
		DEBUG_PRINTF("@@ ASSERT @@\r\n");
	}

	if (linux_opt_assert == 1)
	{
		sleep(10);
	}
	else if (linux_opt_assert == 2)
	{
		sleep(30);
	}
	else if (linux_opt_assert == 3)
	{
		sleep(60);
	}
	else if (linux_opt_assert == 4)
	{
		sleep(600);
	}
	else if (linux_opt_assert > 4)
	{
		while(1) sleep(600);
	}
#endif
}

//--//


BOOL CPU_Initialize()
{
    return TRUE;
}

void CPU_Reset()
{
	 linux_CPU_Change_State(RESTART);
}

void CPU_Sleep(SLEEP_LEVEL level, UINT64 wakeEvents)
{
#if !defined(BUILD_RTM)
	if (LINUXDBG > 3)
	{
		LINUXDBG_print("Enter:level=%d(%02x), events=0x%08llx, %lld\r\n",
			level, level, wakeEvents,
		linux_Time_CurrentTicks());
	}
#endif
    switch(level)
    {
        case SLEEP_LEVEL__DEEP_SLEEP:
			linux_CPU_Change_State(HIBERNATE);
            break;
        case SLEEP_LEVEL__OFF:
			linux_CPU_Change_State(SHUTDOWN);
            break;
        case SLEEP_LEVEL__SLEEP:
        default:
			linux_CPU_Sleep();
			if (LINUXDBG > 3)
			{
				LINUXDBG_print("Exit:level=%d(%02x), events=0x%08llx, %lld\r\n",
					level, level, wakeEvents,
				linux_Time_CurrentTicks());
			}
            break;
    }
}

void CPU_ChangePowerLevel(POWER_LEVEL level)
{
    switch(level)
    {
        case POWER_LEVEL__MID_POWER:
            break;

        case POWER_LEVEL__LOW_POWER:
            break;

        case POWER_LEVEL__HIGH_POWER:
        default:
            break;
    }
}

BOOL CPU_IsSoftRebootSupported ()
{
    return FALSE;
}


void CPU_Halt()
{
	 linux_CPU_Change_State(SHUTDOWN);
}
