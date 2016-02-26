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
#include "..\RX62N.h"

//--//

struct RX62N_AD_Driver
{
    static const UINT32 c_Channels = 8;
    static const UINT32 c_Pins[c_Channels];
};

const UINT32 RX62N_AD_Driver::c_Pins[RX62N_AD_Driver::c_Channels] =
{
        RX62N_GPIO::PA0, RX62N_GPIO::PA1, RX62N_GPIO::PA2, RX62N_GPIO::PA3,
        RX62N_GPIO::PA4, RX62N_GPIO::PA5, RX62N_GPIO::PA6, RX62N_GPIO::PA7
};

RX62N_AD_Driver g_RX62N_AD_Driver;

// 10 bit AD Converter
BOOL AD_Initialize(ANALOG_CHANNEL channel, INT32 precisionInBits)
{
    SYSTEM.MSTPCRA.BIT.MSTPA23 = 0;         // ADO standby off
    if ((UINT32)channel >= g_RX62N_AD_Driver.c_Channels)
        return FALSE;
    if (precisionInBits == 10) {
        if (channel & 0x04) {
            AD1.ADCR.BYTE = 0x0C;           // software trigger, PCLK, single mode
            AD1.ADDPR.BIT.DPSEL = 0;        // LSB alignment
        } else {
            AD0.ADCR.BYTE = 0x0C;           // software trigger, PCLK, single mode
            AD0.ADDPR.BIT.DPSEL = 0;        // LSB alignment
        }
        return TRUE;
    } else
        return FALSE;
}

void AD_Uninitialize(ANALOG_CHANNEL channel)
{

}

INT32 AD_Read(ANALOG_CHANNEL channel)
{
    INT32 value32;
    UINT16 off = channel & 0x3;
    if (channel & 0x04) {
        AD1.ADCSR.BYTE = 0x20 | off;
        while(AD1.ADCSR.BIT.ADST);
        value32 = *((unsigned short*)&AD1.ADDRA + off);
    } else {
        AD0.ADCSR.BYTE = 0x20 | off;
        while(AD0.ADCSR.BIT.ADST);
        value32 = *((unsigned short*)&AD0.ADDRA + off);
    }
    return value32;
}

UINT32 AD_ADChannels()
{
    return RX62N_AD_Driver::c_Channels;
}

GPIO_PIN AD_GetPinForChannel(ANALOG_CHANNEL channel)
{
    if ((UINT32)channel >= g_RX62N_AD_Driver.c_Channels)
        return GPIO_PIN_NONE;
    return g_RX62N_AD_Driver.c_Pins[channel];
}

BOOL AD_GetAvailablePrecisionsForChannel(ANALOG_CHANNEL channel, INT32* precisions, UINT32& size)
{
    size = 0;
    if (precisions == NULL || (UINT32)channel >= g_RX62N_AD_Driver.c_Channels)
        return FALSE;
    precisions[0] = 10;
    size = 1;
    return TRUE;
}
