////////////////////////////////////////////////////////////////////////////
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//  Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
//  Portions Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\RX63N.h"

RX63N_TIME_Driver g_RX63N_TIME_Driver;

void HAL_Time_SetCompare(UINT64 CompareValue)
{
    RX63N_TIME_Driver::SetCompareValue(CompareValue);
}

// Puts the hardware device into sleep mode for a specified number of microseconds.
// void HAL_Time_Sleep_MicroSeconds (
//         UINT32 uSec
// )
// Parameters
//    uSec: The number of microseconds that the hardware device is to remain in sleep mode.
void HAL_Time_Sleep_MicroSeconds(UINT32 uSec)
{
    RX63N_TIME_Driver::Sleep_uSec(uSec);
}

// Puts the hardware device into sleep mode for a specified number of microseconds.
// void HAL_Time_Sleep_MicroSeconds_InRam (
//          UINT32 uSec
// )
// Parameters
//    uSec: The number of microseconds that the hardware device is to remain in sleep mode.

// Puts the hardware device into interruptible sleep mode for a specified number of microseconds.
// void HAL_Time_Sleep_MicroSeconds_InterruptEnabled (
//          UINT32 uSec
// )
// Parameters
//    uSec: The number of microseconds that the hardware device is to remain in sleep mode.
void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(UINT32 uSec)
{
    RX63N_TIME_Driver::Sleep_uSec_Loop(uSec);
}

// Initializes the driver for the system clock.
// BOOL HAL_Time_Initialize ()
// Return Value
//    true if the system clock driver was successfully initialized; otherwise, false.
BOOL HAL_Time_Initialize()
{
    return RX63N_TIME_Driver::Initialize();
}

// Uninitializes the driver (resets the time) for the system clock.
// BOOL HAL_Time_Uninitialize ()
// Return Value
//    true if the reset succeeded; otherwise, false.
BOOL HAL_Time_Uninitialize()
{
    return RX63N_TIME_Driver::Uninitialize();
}

UINT64 HAL_Time_CurrentTicks()
{
    return RX63N_TIME_Driver::CounterValue();
}

UINT64 Time_CurrentTicks()
{
    return HAL_Time_CurrentTicks();
}

// Converts a specified number of real-time clock ticks to common language runtime (CLR) base time, in 100-nanosecond (ns) increments.
// INT64 HAL_Time_TicksToTime (
//         UINT64 Ticks
// )
// Parameters
//    Ticks: The number of real-time clock ticks to be converted to 100-ns CLR increments.
// Return Value
//    The number of 100-ns CLR increments that is equivalent to the specified number of real-time clock ticks.
INT64 HAL_Time_TicksToTime(UINT64 Ticks)
{
    return RX63N_TIME_Driver::TicksToTime(Ticks);
}

INT64 HAL_Time_CurrentTime()
{
    return RX63N_TIME_Driver::CurrentTime();
}

// Obtains the clock drift parameters.
// a: After the function call returns, contains a pointer to the divisor.
// b: After the function call returns, contains a pointer to the multiplication factor.
// c: After the function call returns, contains a pointer to the offset, if any.
// correct_time = (raw_time * b + c) / a
void HAL_Time_GetDriftParameters (INT32* a, INT32* b, INT64* c)
{
    *a = 1;
    *b = 1;
    *c = 0;
}

UINT32 CPU_SystemClock()
{
    return SYSTEM_CLOCK_HZ;            // ICLK=96MHz
}

UINT32 CPU_TicksPerSecond()
{
    return SLOW_CLOCKS_PER_SECOND;        // PCLK/512
}

// Converts a specified number of real-time clock ticks to common language runtime (CLR) base time, in 100-nanosecond (ns) increments.
// UINT64 CPU_TicksToTime (
//         UINT64 Ticks32
// )
// Parameters
//    Ticks64
//    The number of real-time clock ticks to be converted to 100-ns CLR increments.
// Return Value
//    The number of 100-ns CLR increments that is equivalent to the specified number of real-time clock ticks.
UINT64 CPU_TicksToTime(UINT64 Ticks)
{
    Ticks *= (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);
    return Ticks;
}

// Converts a specified number of real-time clock ticks to common language runtime (CLR) base time, in 100-nanosecond (ns) increments.
// UINT32 CPU_TicksToTime (
//         UINT64 Ticks32
// )
// Parameters
//    Ticks32
//    The number of real-time clock ticks to be converted to 100-ns CLR increments.
// Return Value
//    The number of 100-ns CLR increments that is equivalent to the specified number of real-time clock ticks.
UINT64 CPU_TicksToTime(UINT32 Ticks32)
{
#if 0
    UINT64 Ticks;
    Ticks =  (UINT64)Ticks32 * (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /=                   (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);
    return Ticks;
#else
    return CPU_TicksToTime((UINT64)Ticks32);
#endif
}

// Converts a specified number of milliseconds to the corresponding number of real-time clock ticks, for large values.
// UINT64 CPU_MillisecondsToTicks (
//         UINT64 mSec
// )
// Parameters
//    mSec: The number of milliseconds to be converted to real-time clock ticks.
// Return Value
//    The number of real-time clock ticks that is equivalent to the specified number of milliseconds.
UINT64 CPU_MillisecondsToTicks(UINT64 Ticks)
{
    Ticks *= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /= (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);
    return Ticks;
}

// Converts a specified number of milliseconds to the corresponding number of real-time clock ticks, for large values.
// UINT64 CPU_MillisecondsToTicks (
//         UINT32 mSec
// )
// Parameters
//    mSec: The number of milliseconds to be converted to real-time clock ticks.
// Return Value
//    The number of real-time clock ticks that is equivalent to the specified number of milliseconds.
UINT64 CPU_MillisecondsToTicks(UINT32 Ticks32)
{
#if 0
    UINT64 Ticks;
    Ticks  = (UINT64)Ticks32 * (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /=                   (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);
    return Ticks;
#else
    return CPU_MillisecondsToTicks((UINT64)Ticks32);
#endif
}

// Converts a specified number of microseconds to the corresponding number of real-time clock ticks.
// UINT64 CPU_MicrosecondsToTicks (
//         UINT64 uSec
//)
// Parameters
//    uSec: The number of microseconds to be converted to real-time clock ticks.
// Return Value
//    The number of real-time clock ticks that is equivalent to the specified number of microseconds.

//UINT64 CPU_MicrosecondsToTicks(UINT64 uSec) __attribute__ ((section ("SectionForFlashOperations")));
UINT64 CPU_MicrosecondsToTicks(UINT64 uSec)
{
#if ONE_MHZ < SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

// Converts a specified number of microseconds to the corresponding number of real-time clock ticks.
// UINT32 CPU_MicrosecondsToTicks (
//         UINT32 uSec
// )
// Parameters
//    uSec: The number of microseconds to be converted to real-time clock ticks.
// Return Value
//    The number of real-time clock ticks that is equivalent to the specified number of microseconds.
//UINT32 CPU_MicrosecondsToTicks(UINT32 uSec) __attribute__ ((section ("SectionForFlashOperations")));
UINT32 CPU_MicrosecondsToTicks(UINT32 uSec)
{
#if ONE_MHZ < SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

// Converts a specified number of microseconds to the corresponding number of system clock ticks.
// UINT32 CPU_MicrosecondsToSystemClocks (
//    UINT32 uSec
// )
// Parameters
//    uSec: The number of microseconds to be converted to system clock ticks.
// Return Value
//    The number of system clock ticks that is equivalent to the specified number of microseconds.
UINT32 CPU_MicrosecondsToSystemClocks(UINT32 uSec)
{
    uSec *= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ        /CLOCK_COMMON_FACTOR);
    return uSec;
}

int CPU_MicrosecondsToSystemClocks(int uSec)
{
    uSec *= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ        /CLOCK_COMMON_FACTOR);
    return uSec;
}

int CPU_SystemClocksToMicroseconds(int Ticks)
{
    Ticks *= (ONE_MHZ        /CLOCK_COMMON_FACTOR);
    Ticks /= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);

    return Ticks;
}

#define MAX_TIMER_VALUE    0x0000FFFFFFFFFFFFull

BOOL RX63N_TIME_Driver::Initialize()
{
    g_RX63N_TIME_Driver.m_lastRead    = 0;
    g_RX63N_TIME_Driver.m_nextCompare = (UINT64)RX63N_TIMER_Driver::c_MaxTimerValue;

    if (!RX63N_TIMER_Driver::Initialize(RX63N_TIMER_Driver::c_Timer_System, ISR_Overflow, NULL))
        return FALSE;
    if (!RX63N_TIMER_Driver::Initialize(RX63N_TIMER_Driver::c_Timer_Compare, ISR_Compare, NULL))
        return FALSE;
    return TRUE;
}

BOOL RX63N_TIME_Driver::Uninitialize()
{
    if (!RX63N_TIMER_Driver::Uninitialize(RX63N_TIMER_Driver::c_Timer_System))
        return FALSE;
    if (!RX63N_TIMER_Driver::Uninitialize(RX63N_TIMER_Driver::c_Timer_Compare))
        return FALSE;
    return TRUE;
}

UINT64 RX63N_TIME_Driver::CounterValue()
{
    return g_RX63N_TIME_Driver.m_lastRead + (UINT64)RX63N_TIMER_Driver::ReadCounter(RX63N_TIMER_Driver::c_Timer_System);
}

void RX63N_TIME_Driver::SetCompareValue(UINT64 CompareValue)
{
    GLOBAL_LOCK(irq);

    g_RX63N_TIME_Driver.m_nextCompare = CompareValue;
    UINT64 cntr = CounterValue();
    if (cntr > CompareValue) {
        RX63N_TIMER_Driver::ForceInterrupt(RX63N_TIMER_Driver::c_Timer_Compare);
    } else {
        UINT64 diff = CompareValue - cntr;
        if (diff > RX63N_TIMER_Driver::c_MaxTimerValue) {
            diff = RX63N_TIMER_Driver::c_MaxTimerValue;
        }
        RX63N_TIMER_Driver::SetCompare(RX63N_TIMER_Driver::c_Timer_Compare, (UINT16)diff);
        if (CounterValue() > CompareValue) {
            RX63N_TIMER_Driver::ForceInterrupt(RX63N_TIMER_Driver::c_Timer_Compare);
        }
    }
}

void RX63N_TIME_Driver::ISR_Overflow(void* Param)
{
    //GLOBAL_LOCK(irq);
    g_RX63N_TIME_Driver.m_lastRead += (UINT64)RX63N_TIMER_Driver::c_MaxTimerValue;
}

void RX63N_TIME_Driver::ISR_Compare(void* Param)
{
    if (CounterValue() >= g_RX63N_TIME_Driver.m_nextCompare) {
        // this also schedules the next one, if there is one
        HAL_COMPLETION::DequeueAndExec();
    } else {
        //
        // Because we are limited in the resolution of timer,
        // resetting the compare will properly configure the next interrupt.
        //
        SetCompareValue(g_RX63N_TIME_Driver.m_nextCompare);
    }
}

INT64 RX63N_TIME_Driver::TicksToTime(UINT64 Ticks)
{
    return CPU_TicksToTime(Ticks);
}

INT64 RX63N_TIME_Driver::CurrentTime()
{
    // return Time_TicksToTime(Time_CurrentTicks());
    // we collapse the above to improve perf on a high hit function

    INT64 Time = CPU_TicksToTime(CounterValue());

#if defined(HAL_TIMEWARP)
    return s_timewarp_compensate + Time;
#else
    return Time;
#endif
}

#define RX63N_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS 3

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((section ("SectionForFlashOperations"))) IDelayLoop(volatile int count)
{
    while (count-- > 0) {
        __asm__ __volatile ("nop");
    }
}
#ifdef __cplusplus
}
#endif

//void  RX63N_TIME_Driver::Sleep_uSec(UINT32 uSec) __attribute__ ((section ("SectionForFlashOperations")));

void __attribute__ ((section ("SectionForFlashOperations"))) RX63N_TIME_Driver::Sleep_uSec(UINT32 uSec)
{
    GLOBAL_LOCK(irq);

    UINT64 value    = CounterValue();
    UINT64 maxDiff  = CPU_MicrosecondsToTicks( uSec );      // The free-running timer clocks at a constant 3.25 MHz

    if (maxDiff <= RX63N_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS)
        maxDiff  = RX63N_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS;
    else
        maxDiff -= RX63N_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS;

    while((CounterValue() - value) <= maxDiff);}

//void RX63N_TIME_Driver::Sleep_uSec_Loop(UINT32 uSec) __attribute__ ((section ("SectionForFlashOperations")));

void RX63N_TIME_Driver::Sleep_uSec_Loop(UINT32 uSec)
{
    uSec *= (SYSTEM_CYCLE_CLOCK_HZ / CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ               / CLOCK_COMMON_FACTOR);

    // iterations is equal to the number of CPU instruction cycles in the required time minus
    // overhead cycles required to call this subroutine.
    int iterations = (int)uSec - 12;      // Subtract off call & calculation overhead
    CYCLE_DELAY_LOOP(iterations);
}
