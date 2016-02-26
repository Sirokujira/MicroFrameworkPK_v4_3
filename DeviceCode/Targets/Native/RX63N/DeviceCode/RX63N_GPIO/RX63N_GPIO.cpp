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

RX63N_GPIO_Driver g_RX63N_GPIO_Driver;

BOOL CPU_GPIO_Initialize()
{
    return RX63N_GPIO_Driver::Initialize();
}

BOOL CPU_GPIO_Uninitialize()
{
    return RX63N_GPIO_Driver::Uninitialize();
}

UINT32 CPU_GPIO_Attributes(GPIO_PIN Pin)
{
    return RX63N_GPIO_Driver::Attributes(Pin);
}

void CPU_GPIO_DisablePin(GPIO_PIN Pin, GPIO_RESISTOR ResistorState, UINT32 Direction, GPIO_ALT_MODE AltFunction)
{
    RX63N_GPIO_Driver::DisablePin(Pin, ResistorState, Direction, AltFunction);
}

void CPU_GPIO_EnableOutputPin(GPIO_PIN Pin, BOOL InitialState)
{
    RX63N_GPIO_Driver::EnableOutputPin(Pin, InitialState);
}

BOOL CPU_GPIO_EnableInputPin(GPIO_PIN Pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState)
{
    return RX63N_GPIO_Driver::EnableInputPin(Pin, GlitchFilterEnable, ISR, 0, IntEdge, ResistorState);
}

BOOL CPU_GPIO_EnableInputPin2(GPIO_PIN Pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState)
{
    return RX63N_GPIO_Driver::EnableInputPin(Pin, GlitchFilterEnable, ISR, ISR_Param, IntEdge, ResistorState);
}

BOOL CPU_GPIO_GetPinState(GPIO_PIN Pin)
{
    return RX63N_GPIO_Driver::GetPinState(Pin);
}

void CPU_GPIO_SetPinState(GPIO_PIN Pin, BOOL PinState)
{
    RX63N_GPIO_Driver::SetPinState(Pin, PinState);
}

INT32 CPU_GPIO_GetPinCount()
{
    return RX63N_GPIO_Driver::GetPinCount();
}

void CPU_GPIO_GetPinsMap(UINT8* pins, size_t size)
{
    RX63N_GPIO_Driver::GetPinsMap( pins, size );
}

UINT8 CPU_GPIO_GetSupportedResistorModes(GPIO_PIN pin)
{
    return RX63N_GPIO_Driver::GetSupportedResistorModes(pin);
}

UINT8 CPU_GPIO_GetSupportedInterruptModes(GPIO_PIN pin)
{
    return RX63N_GPIO_Driver::GetSupportedInterruptModes(pin);
}

BOOL CPU_GPIO_PinIsBusy(GPIO_PIN Pin)
{
    return RX63N_GPIO_Driver::PinIsBusy(Pin);
}

BOOL CPU_GPIO_ReservePin(GPIO_PIN Pin, BOOL fReserve)
{
    return RX63N_GPIO_Driver::ReservePin(Pin, fReserve);
}

UINT32 CPU_GPIO_GetDebounce()
{
    return RX63N_GPIO_Driver::GetDebounce();
}

BOOL CPU_GPIO_SetDebounce( INT64 debounceTimeMilliseconds )
{
    return RX63N_GPIO_Driver::SetDebounce(debounceTimeMilliseconds);
}

//--//

///////////////////////////////////////////////////////////////////////////////
//  Initialize
///////////////////////////////////////////////////////////////////////////////

UINT8 PinAssign_100[] = {
    0b10100000,     // P0x
    0b11111100,     // P1x
    0b11111111,     // P2x
    0b11111111,     // P3x
    0b11111111,     // P4x
    0b00111111,     // P5x
    0b00000000,     // P6x
    0b00000000,     // P7x
    0b00000000,     // P8x
    0b00000000,     // P9x
    0b11111111,     // PAx
    0b11111111,     // PBx
    0b11111111,     // PCx
    0b11111111,     // PDx
    0b11111111,     // PEx
    0b00000000,     // PFx
    0b00000000,     // PGx
    0b00001000      // PJx
};

UINT8 *PinAssign = (UINT8 *)&PinAssign_100;

#if 0
UINT8 PinAssign_145_144[] = {
    0b10101111,     // P0x
    0b11111100,     // P1x
    0b11111111,     // P2x
    0b11111111,     // P3x
    0b11111111,     // P4x
    0b01111111,     // P5x
    0b11111111,     // P6x
    0b11111111,     // P7x
    0b11001111,     // P8x
    0b00001111,     // P9x
    0b11111111,     // PAx
    0b11111111,     // PBx
    0b11111111,     // PCx
    0b11111111,     // PDx
    0b11111111,     // PEx
    0b00100000,     // PFx
    0b00000000,     // PGx
    0b00101000      // PJx
};

UINT8 PinAssign_177_176[] = {
    0b10101111,     // P0x
    0b11111111,     // P1x
    0b11111111,     // P2x
    0b11111111,     // P3x
    0b11111111,     // P4x
    0b11111111,     // P5x
    0b11111111,     // P6x
    0b11111111,     // P7x
    0b11111111,     // P8x
    0b11111111,     // P9x
    0b11111111,     // PAx
    0b11111111,     // PBx
    0b11111111,     // PCx
    0b11111111,     // PDx
    0b11111111,     // PEx
    0b00111111,     // PFx
    0b11111111,     // PGx
    0b00101000      // PJx
};
#endif

#define DEFAULT_GPIO_ATTRIBUTE  (GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT)

static BOOL GPIO_IsVaild(GPIO_PIN pin)
{
    return (PinAssign[GPIO_PORT(pin)] & GPIO_MASK(pin) != 0)? TRUE:FALSE;
}

INT32 RX63N_GPIO_Driver::GetBitPos(UINT8 val)
{
    INT32 pos = 0;
    while (pos < 8) {
        if (val & 1)
            return pos;
        pos++;
        val >>= 1;
    }
    return -1;
}

void RX63N_GPIO_Driver::UpdatePinValue(GPIO_PIN pin)
{
    UINT32 port = GPIO_PORT(pin);
    UINT32 mask = GPIO_MASK(pin);
    g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[port] &= ~mask;
    g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[port] |= (_PIDR(port) & mask);
}

void RX63N_GPIO_Driver::EnableInputPolling(GPIO_PIN pin)
{
    if (pin < c_MaxPins)
        g_RX63N_GPIO_Driver.m_GPIO_PortReservedMask[GPIO_PORT(pin)] |= GPIO_MASK(pin);
}
void RX63N_GPIO_Driver::DisableInputPolling(GPIO_PIN pin)
{
    if (pin < c_MaxPins)
        g_RX63N_GPIO_Driver.m_GPIO_PortReservedMask[GPIO_PORT(pin)] &= ~GPIO_MASK(pin);
}

void RX63N_GPIO_Driver::ISR(void)
{
    UINT32 pin;
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (g_RX63N_GPIO_Driver.m_GPIO_InISR == false) {
        g_RX63N_GPIO_Driver.m_GPIO_InISR = true;
        pin = ChkPinChange();
        if (pin != 255) {
            PIN_ISR_DESCRIPTOR& pinIsr = g_RX63N_GPIO_Driver.m_PinIsr[pin];
            if (pinIsr.m_intEdge != GPIO_INT_NONE)
                pinIsr.Fire((void *)&pinIsr);
        }
        g_RX63N_GPIO_Driver.m_GPIO_InISR = false;
    }
    g_RX63N_GPIO_Driver.m_GPIO_Completion.EnqueueDelta(RX63N_GPIO_POLLING_INTERVAL);
    return;
}

UINT32 RX63N_GPIO_Driver::ChkPinChange(void)
{
    INT32 i, pos;
    UINT8 val, b, mask;
    UINT32 pin = 255;
    GLOBAL_LOCK(irq);
    for (i = 0; i < c_MaxPorts; i++) {
        mask = g_RX63N_GPIO_Driver.m_GPIO_PortReservedMask[i];
        if (mask) {
            val = VB(0x0008c040 + i) & mask;
            if (g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[i] != val) {
                b = g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[i] ^ val;
                g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[i] = val;
                pos = GetBitPos(b);
                if (pos != -1) {
                    pin = i*8 + pos;
                    if (pin != 255) {
                        PIN_ISR_DESCRIPTOR& pinIsr = g_RX63N_GPIO_Driver.m_PinIsr[pin];
                        if (b & val) {
                            // low -> high
                            pinIsr.m_status = PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
                        } else {
                            // high -> low
                            pinIsr.m_status = PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge;
                        }
                    }
                    break;
                }
            }
        }
    }
    return pin;
}

void RX63N_GPIO_Driver::PIN_ISR_DESCRIPTOR::Fire(void *arg)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    PIN_ISR_DESCRIPTOR *desc = (PIN_ISR_DESCRIPTOR *)arg;
    desc->m_isr(desc->m_pin, (desc->m_status & c_Status_AllowHighEdge) != 0, desc->m_param);
    UINT8 mask = c_Flags_RequireHighEdge | c_Flags_RequireLowEdge;
    if ((desc->m_flags & mask) == mask) {
        desc->m_status ^= PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge | PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
    }
}

void RX63N_GPIO_Driver::STUB_GPIOISRVector(GPIO_PIN Pin, BOOL PinState, void *Param)
{
}

BOOL RX63N_GPIO_Driver::Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    PIN_ISR_DESCRIPTOR *pinIsr = g_RX63N_GPIO_Driver.m_PinIsr;
    for (UINT8 port = 0; port < c_MaxPorts; port++) {
        g_RX63N_GPIO_Driver.m_GPIO_PortReservedMask[port] = 0;
        g_RX63N_GPIO_Driver.m_GPIO_PortLastValues[port] = 0;
    }

    for (UINT8 pin = 0; pin < c_MaxPins; pin++) {
        g_RX63N_GPIO_Driver.m_GPIO_Reserved[pin] = false;

        pinIsr->m_pin = pin;
        pinIsr->m_intEdge = GPIO_INT_NONE;
        pinIsr->m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
        pinIsr->m_param = NULL;
        pinIsr->m_completion.Initialize();
        pinIsr->m_completion.InitializeForISR(&PIN_ISR_DESCRIPTOR::Fire, pinIsr);
        pinIsr++;
    }
    g_RX63N_GPIO_Driver.m_GPIO_Driver_Initialized = true;
    g_RX63N_GPIO_Driver.m_GPIO_Completion.Initialize();
    g_RX63N_GPIO_Driver.m_GPIO_Completion.InitializeForISR((HAL_CALLBACK_FPN)ISR, NULL);
    g_RX63N_GPIO_Driver.m_GPIO_Completion.EnqueueDelta(RX63N_GPIO_POLLING_INTERVAL);
    return true;
}

BOOL RX63N_GPIO_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    GLOBAL_LOCK(irq);
    g_RX63N_GPIO_Driver.m_GPIO_Driver_Initialized = false;
    PIN_ISR_DESCRIPTOR* pinIsr = g_RX63N_GPIO_Driver.m_PinIsr;
    for (UINT8 pin = 0; pin < c_MaxPins; pin++) {
        pinIsr->m_completion.Abort();
        pinIsr++;
    }
    g_RX63N_GPIO_Driver.m_GPIO_Completion.Abort();
    return true;
}

UINT32 RX63N_GPIO_Driver::Attributes(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (GPIO_IsVaild(pin))
        return DEFAULT_GPIO_ATTRIBUTE;
    return GPIO_ATTRIBUTE_NONE;
}
 
void RX63N_GPIO_Driver::DisablePin(GPIO_PIN pin, GPIO_RESISTOR resistorState, UINT32 Direction, GPIO_ALT_MODE AltFunction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (!GPIO_IsVaild(pin)) return;
    GLOBAL_LOCK(irq);

    PIN_ISR_DESCRIPTOR& pinIsr = g_RX63N_GPIO_Driver.m_PinIsr[pin];
    pinIsr.m_intEdge = GPIO_INT_NONE;
    pinIsr.m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
    pinIsr.m_param = NULL;
    DisableInputPolling(pin);
}

void RX63N_GPIO_Driver::EnableOutputPin(GPIO_PIN pin, BOOL initialState)
{
    if (!GPIO_IsVaild(pin)) return;
    GLOBAL_LOCK(irq);

    UINT32 port = GPIO_PORT(pin);
    UINT8 mask = GPIO_MASK(pin);
    //*_PXXPFS(port, bit) = 0;
    _PMR(port) &= ~mask;
    _PDR(port) |= mask;
    if (initialState)
        _PODR(port) |= mask;
    else
        _PODR(port) &= ~mask;
    PIN_ISR_DESCRIPTOR& pinIsr = g_RX63N_GPIO_Driver.m_PinIsr[pin];
    pinIsr.m_intEdge = GPIO_INT_NONE;
    pinIsr.m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
    pinIsr.m_param = NULL;
    DisableInputPolling(pin);
}

BOOL RX63N_GPIO_Driver::EnableInputPin(GPIO_PIN pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* Param, GPIO_INT_EDGE intEdge, GPIO_RESISTOR resistorState)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (!GPIO_IsVaild(pin)) return FALSE;
    BOOL flag = true;
    GLOBAL_LOCK(irq);
    PIN_ISR_DESCRIPTOR& pinIsr = g_RX63N_GPIO_Driver.m_PinIsr[pin];
    pinIsr.m_intEdge = intEdge;
    pinIsr.m_isr = (ISR != NULL) ? ISR : (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
    pinIsr.m_param = Param;
    pinIsr.m_flags = GlitchFilterEnable ? PIN_ISR_DESCRIPTOR::c_Flags_Debounce : 0;
    pinIsr.m_status = 0;
    pinIsr.m_completion.Abort();
    pinIsr.m_completion.Initialize();

    UINT32 port = GPIO_PORT(pin);
    UINT8 mask = GPIO_MASK(pin);
    _PMR(port) &= ~mask;
    _PDR(port) &= ~mask;

    if (ISR) {
        switch (intEdge)
        {
        case GPIO_INT_NONE:
            flag = false;
            break;
        case GPIO_INT_EDGE_LOW:
        case GPIO_INT_LEVEL_LOW:
            pinIsr.m_status |= PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge;
            break;
        case GPIO_INT_EDGE_HIGH:
        case GPIO_INT_LEVEL_HIGH:
            pinIsr.m_status |= PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
            break;
        case GPIO_INT_EDGE_BOTH:
            pinIsr.m_flags |= PIN_ISR_DESCRIPTOR::c_Flags_RequireHighEdge | PIN_ISR_DESCRIPTOR::c_Flags_RequireLowEdge;
            if (GetPinState(pin)) {
                pinIsr.m_status &= ~PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
                pinIsr.m_status |=  PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge;
            } else {
                pinIsr.m_status &= ~PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge;
                pinIsr.m_status |=  PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
            }
            break;
        default:
            flag = false;
        }
    }
    UpdatePinValue(pin);
    EnableInputPolling(pin);
    return flag;
}

BOOL RX63N_GPIO_Driver::GetPinState(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (!GPIO_IsVaild(pin)) return false;
    UINT32 port = GPIO_PORT(pin);
    UINT32 mask = GPIO_MASK(pin);
    GLOBAL_LOCK(irq);
    return ((_PIDR(port) & mask) != 0)? true:false;
}

void RX63N_GPIO_Driver::SetPinState(GPIO_PIN pin, BOOL pinState)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (!GPIO_IsVaild(pin)) return;
    UINT32 port = GPIO_PORT(pin);
    UINT8 mask = GPIO_MASK(pin);
    GLOBAL_LOCK(irq);
    _PDR(port) |= mask;
    if (pinState)
        _PODR(port) |= mask;
    else
        _PODR(port) &= ~mask;
}

BOOL RX63N_GPIO_Driver::PinIsBusy(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return g_RX63N_GPIO_Driver.m_GPIO_Reserved[pin];
}

BOOL RX63N_GPIO_Driver::ReservePin(GPIO_PIN pin, BOOL fReserve)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    GLOBAL_LOCK(irq);
    g_RX63N_GPIO_Driver.m_GPIO_Reserved[pin] = fReserve;
    return true;
}

UINT32 RX63N_GPIO_Driver::GetDebounce()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return CPU_TicksToTime(g_RX63N_GPIO_Driver.m_DebounceTicks) / 10000;
}

BOOL RX63N_GPIO_Driver::SetDebounce(INT64 debounceTimeMilliseconds)
{   
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (debounceTimeMilliseconds < c_MinDebounceTimeMs || c_MaxDebounceTimeMs < debounceTimeMilliseconds)
        return false;
    g_RX63N_GPIO_Driver.m_DebounceTicks = CPU_MillisecondsToTicks((UINT32)debounceTimeMilliseconds);
    return true;
}

INT32 RX63N_GPIO_Driver::GetPinCount()
{
    return c_MaxPins;
}

void RX63N_GPIO_Driver::GetPinsMap(UINT8* pins, size_t size)
{
    GPIO_PIN pin;
    for (pin = (GPIO_PIN)0; pin < (GPIO_PIN)c_MaxPins; pin++) {
        if (size-- == 0)
            break;
        *pins++ = (UINT8)Attributes(pin);
    }
}

UINT8 RX63N_GPIO_Driver::GetSupportedResistorModes(GPIO_PIN pin)
{
    return RX63N_GPIO_Driver::c_GPIO_ResistorMode;
}


UINT8 RX63N_GPIO_Driver::GetSupportedInterruptModes(GPIO_PIN pin)
{
    return RX63N_GPIO_Driver::c_GPIO_InterruptMode;
}
