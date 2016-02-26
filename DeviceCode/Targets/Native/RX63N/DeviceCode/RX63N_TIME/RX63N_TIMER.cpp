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

RX63N_TIMER_Driver g_RX63N_TIMER_Driver;

BOOL RX63N_TIMER_Driver::Initialize(UINT32 Timer, HAL_CALLBACK_FPN ISR, void* ISR_Param)
{
    ASSERT(Timer < c_MaxTimers);
    GLOBAL_LOCK(irq);
    TimerISR& tmrISR = g_RX63N_TIMER_Driver.m_timers[Timer];
    if (tmrISR.Configured)
        return false;
    tmrISR.ISR = ISR;
    tmrISR.Arg = ISR_Param;
    tmrISR.id = Timer;
    SYSTEM.PRCR.WORD = 0xA502;
    if ((Timer == 0) || (Timer == 1)) {
        SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;
    } else {
        SYSTEM.MSTPCRA.BIT.MSTPA14 = 0;
    }
    SYSTEM.PRCR.WORD = 0xA500;
    CMT.CMSTR0.WORD &= (Timer == 0 ? 2 : 1);    // disable clock
    if (Timer == 0) {
        // Timer == 0
        CMT0.CMCOR = 0xffff;
        ICU.IPR[0x04].BIT.IPR = 0xe;            // IPR = 14 (15: highest priority)
#if CLKDEV == 8
        CMT0.CMCR.WORD = 0x0040;                // CMIE is Enable,CKS is PCLK/8
#elif CLKDEV == 32
        CMT0.CMCR.WORD = 0x0041;                // CMIE is Enable,CKS is PCLK/32
#elif CLKDEV == 128
        CMT0.CMCR.WORD = 0x0042;                // CMIE is Enable,CKS is PCLK/128
#else    // CKS == 512
        CMT0.CMCR.WORD = 0x0043;                // CMIE is Enable,CKS is PCLK/512
#endif
        ICU.IER[0x03].BIT.IEN4 = 1;             // IER enable
    } else {
        // Timer == 1
        CMT1.CMCOR = 0xffff;
        ICU.IPR[0x05].BIT.IPR = 0xe;            // IPR = 14 (15: highest priority)
#if CLKDEV == 8
        CMT1.CMCR.WORD = 0x0040;                // CMIE is Enable,CKS is PCLK/8
#elif CLKDEV == 32
        CMT1.CMCR.WORD = 0x0041;                // CMIE is Enable,CKS is PCLK/32
#elif CLKDEV == 128
        CMT1.CMCR.WORD = 0x0042;                // CMIE is Enable,CKS is PCLK/128
#else    // CKS == 512
        CMT1.CMCR.WORD = 0x0043;                // CMIE is Enable,CKS is PCLK/512
#endif
        ICU.IER[0x03].BIT.IEN5 = 1;             // IER enable
    }
    CMT.CMSTR0.WORD |= (Timer == 0 ? 1 : 2);    // enable clock
    tmrISR.Configured = true;
    return true;
}

BOOL RX63N_TIMER_Driver::Uninitialize(UINT32 Timer)
{
    GLOBAL_LOCK(irq);
    TimerISR& tmrISR = g_RX63N_TIMER_Driver.m_timers[Timer];
    if (tmrISR.Configured)
        return false;
    if (Timer == 0) {
        ICU.IER[0x03].BIT.IEN4 = 0;             // IER disable
    } else {
        ICU.IER[0x03].BIT.IEN5 = 0;             // IER disable
    }
    CMT.CMSTR0.WORD &= (Timer == 0 ? 2 : 1);    // disable clock
    tmrISR.Configured = false;
    return true;
}

UINT16 RX63N_TIMER_Driver::ReadCounter(UINT32 Timer)
{
    if (Timer == 0) {
        return (UINT16)CMT0.CMCNT;
    } else {
        return (UINT16)CMT1.CMCNT;
    }
}

void RX63N_TIMER_Driver::ForceInterrupt(UINT32 Timer)
{
    if (Timer == 0) {
        CMT0.CMCOR = (UINT16)(CMT0.CMCNT + 2);
    } else {
        CMT1.CMCOR = (UINT16)(CMT1.CMCNT + 2);
    }
}

void RX63N_TIMER_Driver::SetCompare(UINT32 Timer, UINT16 Compare)
{
    if (Timer == 0) {
        CMT0.CMCNT = (UINT16)0;
        CMT0.CMCOR = (UINT16)Compare;
    } else {
        CMT1.CMCNT = (UINT16)0;
        CMT1.CMCOR = (UINT16)Compare;
    }
}

static void RX63N_TIMER_ISR(UINT32 Timer)
{
    g_RX63N_TIMER_Driver.m_timers[Timer].ISR(g_RX63N_TIMER_Driver.m_timers[Timer].Arg);
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_CMT0_CMI0(void) { RX63N_TIMER_ISR(0); }
void __attribute__ ((interrupt)) INT_Excep_CMT1_CMI1(void) { RX63N_TIMER_ISR(1); }
void __attribute__ ((interrupt)) INT_Excep_CMT2_CMI2(void) { RX63N_TIMER_ISR(2); }
void __attribute__ ((interrupt)) INT_Excep_CMT3_CMI3(void) { RX63N_TIMER_ISR(3); }
#ifdef __cplusplus
}
#endif
