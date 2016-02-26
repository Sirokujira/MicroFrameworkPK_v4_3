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

#include <tinyhal.h>
#include "..\RX63N.h"
#include "RX63N_ANALOG.h"

#define TEST_10BITS
//--//

const UINT32 RX63N_AD_Driver::c10bits_Pins[RX63N_AD_Driver::c_Channels] =
{
    RX63N_GPIO::PE2, RX63N_GPIO::PE3, RX63N_GPIO::PE4, RX63N_GPIO::PE5,
    RX63N_GPIO::PE6, RX63N_GPIO::PE7, RX63N_GPIO::PD6, RX63N_GPIO::PD7
};

const UINT32 RX63N_AD_Driver::c12bits_Pins[RX63N_AD_Driver::c_Channels] =
{
    RX63N_GPIO::P40, RX63N_GPIO::P41, RX63N_GPIO::P42, RX63N_GPIO::P43,
    RX63N_GPIO::P44, RX63N_GPIO::P45, RX63N_GPIO::P46, RX63N_GPIO::P47
};

UINT8 *P4nPFS = (UINT8 *)0x0008C160;
UINT8 *PDnPFS = (UINT8 *)0x0008C1A8;
UINT8 *PEnPFS = (UINT8 *)0x0008C1B0;

RX63N_AD_Driver g_RX63N_AD_Driver;

// 10 bit AD Converter
BOOL AD_Initialize(ANALOG_CHANNEL channel, INT32 precisionInBits)
{
    UINT32 off = (UINT32)channel;

#if defined(TEST_10BITS)
    precisionInBits = 10;
#endif
    if (precisionInBits != 10)
        precisionInBits = 12;
    g_RX63N_AD_Driver.m_Precision = precisionInBits;
    if ((UINT32)channel >= g_RX63N_AD_Driver.c_Channels)
        return FALSE;
    if (precisionInBits == 10) {
        SYSTEM.PRCR.WORD = 0xA502;
        SYSTEM.MSTPCRA.BIT.MSTPA23 = 0;         // ADO standby off
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        if (off < 6) {
            *(PEnPFS + off + 2) |= 0x80;
            PORTE.PMR.BYTE |= (1 << (off + 2));
        } else {
            *(PDnPFS + off) |= 0x80;
            PORTD.PMR.BYTE |= (1 << off);
        }
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        AD.ADCR.BYTE = 0x0C;           // software trigger, PCLK, single mode
        AD.ADCR2.BIT.DPSEL = 0;        // LSB alignment
        return TRUE;
    } else if (precisionInBits == 12) {
        SYSTEM.PRCR.WORD = 0xA502;
        SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;         // ADO standby off
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        *(P4nPFS + off) |= 0x80;
        PORT4.PMR.BYTE |= (1 << off);
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        S12AD.ADCSR.BYTE = 0x0;
        S12AD.ADCER.BIT.ADRFMT = 0;
        return TRUE;
    } else
        return FALSE;
}

void AD_Uninitialize(ANALOG_CHANNEL channel)
{
    UINT32 off = (UINT32)channel;
    MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
    MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
    if (g_RX63N_AD_Driver.m_Precision == 10) {
        *(P4nPFS + off) &= ~0x80;
        PORT4.PMR.BYTE &= ~(1 << off);
    } else if (g_RX63N_AD_Driver.m_Precision == 12) {
        if (off < 6) {
            *(PEnPFS + off + 2) &= ~0x80;
            PORTE.PMR.BYTE &= ~(1 << (off + 2));
        } else {
            *(PDnPFS + off) &= ~0x80;
            PORTD.PMR.BYTE &= ~(1 << off);
        }
    }
    MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
}

INT32 AD_Read(ANALOG_CHANNEL channel)
{
    INT32 value32 = 0;
    UINT16 off = channel;
    if (g_RX63N_AD_Driver.m_Precision == 10) {
        AD.ADCSR.BYTE = 0x20 | off;
        while (AD.ADCSR.BIT.ADST);
        value32 = *((unsigned short*)&AD.ADDRA + off);
    } else if (g_RX63N_AD_Driver.m_Precision == 12) {
        S12AD.ADANS0.WORD |= (1 << off);
        S12AD.ADCSR.BIT.ADST = 1;
        while (S12AD.ADCSR.BIT.ADST);
        value32 = *((unsigned short*)&S12AD.ADDR0 + off);
    }
    return value32;
}

UINT32 AD_ADChannels()
{
    return RX63N_AD_Driver::c_Channels;
}

GPIO_PIN AD_GetPinForChannel(ANALOG_CHANNEL channel)
{
    if ((UINT32)channel >= g_RX63N_AD_Driver.c_Channels)
        return GPIO_PIN_NONE;
    if (g_RX63N_AD_Driver.m_Precision == 10)
        return g_RX63N_AD_Driver.c10bits_Pins[channel];
    else
        return g_RX63N_AD_Driver.c12bits_Pins[channel];
}

BOOL AD_GetAvailablePrecisionsForChannel(ANALOG_CHANNEL channel, INT32* precisions, UINT32& size)
{
    size = 0;
    if (precisions == NULL || (UINT32)channel >= g_RX63N_AD_Driver.c_Channels)
        return FALSE;
    precisions[0] = g_RX63N_AD_Driver.m_Precision;
    size = 1;
    return TRUE;
}
