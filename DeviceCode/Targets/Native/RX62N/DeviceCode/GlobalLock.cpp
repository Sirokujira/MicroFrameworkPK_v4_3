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
//  Portions Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include "tinyhal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_STATE    1
#define DISABLE_STATE   0

static inline UINT32 get_int(void)
{
    UINT32 ipl;
    __asm__ __volatile__ ("mvfc psw,%0":"=r"(ipl):);
    return ((ipl & 0x00010000) >> 16);
}

static inline void set_int(UINT32 i)
{
    UINT32 ipl = i << 16;
    __asm__ __volatile__ ("mvtc %0,psw":"=r"(ipl):);
}

static inline void enable_int(void)
{
    __asm__ __volatile__ ("setpsw i");
}

static inline void disable_int(void)
{
    __asm__ __volatile__ ("clrpsw i");
}

/*
 *  Usage:
 *  constructor { Release Acquire | Probe } destructor
 *
 *  legal states:     m_state    psw
 *  WAS_DISABLED         0       0 (disabled)
 *  WAS_ENABLED          1       0 (disabled)
 *  RELEASED             1       1 (enabled)
 */

// Disables interrupts if they are currently enabled.
// This constructor also stores the previous interrupt state so that
// it can be restored when the SmartPtr_IRQ object is destroyed.
SmartPtr_IRQ::SmartPtr_IRQ(void* context)
{ 
    m_context = context; 
    Disable(); 
}

// Restores the system interrupt to the state it was in
// when the SmartPTR_IRQ object was created.
SmartPtr_IRQ::~SmartPtr_IRQ() 
{ 
    Restore(); 
}

// Determines whether interrupts were disabled
// before the smart pointer was created.
BOOL SmartPtr_IRQ::WasDisabled()
{
    return (m_state == DISABLE_STATE);
}

// Disables interrupts if they are currently enabled,
// storing the previous interrupt state.
void SmartPtr_IRQ::Acquire()
{
    if (m_state == DISABLE_STATE)
        Disable();
}

// Restores an interrupt to its previous state,
// as stored by the SmartPtr_IRQ.Acquire method.
void SmartPtr_IRQ::Release()
{
    m_state = get_int();
    enable_int();
}

// Enables interrupts long enough for one interrupt to be processed,
// and then disables them again.
void SmartPtr_IRQ::Probe()
{
    UINT32 cp = get_int();
    enable_int();
    __asm__ __volatile__ ("nop");
    set_int(cp);
}

// Gets the current state of interrupts.
// Return Value
// true if interrupts are currently enabled; otherwise, false.
BOOL SmartPtr_IRQ::GetState(void* context)
{
    UINT32 cp = get_int();
    return (cp != DISABLE_STATE);
}

// Forces interrupts to be disabled immediately.
// Return Value
// The previous interrupt state.
// true if enabled
// false if disabled
BOOL SmartPtr_IRQ::ForceDisabled(void* context)
{
    disable_int();
    return true;
}

// Forces interrupts to be enabled immediately.
// Return Value
// The previous interrupt state.
// true if enabled
// false if disabled
BOOL SmartPtr_IRQ::ForceEnabled(void* context)
{
    enable_int();
    return true;
}

void SmartPtr_IRQ::Disable()
{
    UINT32 cp;

    cp = get_int();
    disable_int();
    m_state = cp;
}

void SmartPtr_IRQ::Restore()
{
    if (m_state != DISABLE_STATE)
        enable_int();
}

