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

RX62N_GPIO_Driver g_RX62N_GPIO_Driver;

BOOL CPU_GPIO_Initialize()
{
    return RX62N_GPIO_Driver::Initialize();
}

BOOL CPU_GPIO_Uninitialize()
{
    return RX62N_GPIO_Driver::Uninitialize();
}

UINT32 CPU_GPIO_Attributes(GPIO_PIN Pin)
{
    return RX62N_GPIO_Driver::Attributes(Pin);
}

void CPU_GPIO_DisablePin(GPIO_PIN Pin, GPIO_RESISTOR ResistorState, UINT32 Direction, GPIO_ALT_MODE AltFunction)
{
    RX62N_GPIO_Driver::DisablePin(Pin, ResistorState, Direction, AltFunction);
}

void CPU_GPIO_EnableOutputPin(GPIO_PIN Pin, BOOL InitialState)
{
    RX62N_GPIO_Driver::EnableOutputPin(Pin, InitialState);
}

BOOL CPU_GPIO_EnableInputPin(GPIO_PIN Pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState)
{
    return RX62N_GPIO_Driver::EnableInputPin(Pin, GlitchFilterEnable, ISR, 0, IntEdge, ResistorState);
}

BOOL CPU_GPIO_EnableInputPin2(GPIO_PIN Pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* ISR_Param, GPIO_INT_EDGE IntEdge, GPIO_RESISTOR ResistorState)
{
    return RX62N_GPIO_Driver::EnableInputPin(Pin, GlitchFilterEnable, ISR, ISR_Param, IntEdge, ResistorState);
}

BOOL CPU_GPIO_GetPinState(GPIO_PIN Pin)
{
    return RX62N_GPIO_Driver::GetPinState(Pin);
}

void CPU_GPIO_SetPinState(GPIO_PIN Pin, BOOL PinState)
{
    RX62N_GPIO_Driver::SetPinState(Pin, PinState);
}

INT32 CPU_GPIO_GetPinCount()
{
    return RX62N_GPIO_Driver::GetPinCount();
}

void CPU_GPIO_GetPinsMap(UINT8* pins, size_t size)
{
    RX62N_GPIO_Driver::GetPinsMap( pins, size );
}

UINT8 CPU_GPIO_GetSupportedResistorModes(GPIO_PIN pin)
{
    return RX62N_GPIO_Driver::GetSupportedResistorModes(pin);
}

UINT8 CPU_GPIO_GetSupportedInterruptModes(GPIO_PIN pin)
{
    return RX62N_GPIO_Driver::GetSupportedInterruptModes(pin);
}

BOOL CPU_GPIO_PinIsBusy(GPIO_PIN Pin)
{
    return RX62N_GPIO_Driver::PinIsBusy(Pin);
}

BOOL CPU_GPIO_ReservePin(GPIO_PIN Pin, BOOL fReserve)
{
    return RX62N_GPIO_Driver::ReservePin(Pin, fReserve);
}

UINT32 CPU_GPIO_GetDebounce()
{
    return RX62N_GPIO_Driver::GetDebounce();
}

BOOL CPU_GPIO_SetDebounce( INT64 debounceTimeMilliseconds )
{
    return RX62N_GPIO_Driver::SetDebounce(debounceTimeMilliseconds);
}

//--//

///////////////////////////////////////////////////////////////////////////////
//  Initialize
///////////////////////////////////////////////////////////////////////////////

const UINT8 RX62N_GPIO_Driver::c_GPIO_Attributes[RX62N_GPIO_Driver::c_MaxPins] =
{
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P00
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P01
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P02
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P03
    GPIO_ATTRIBUTE_NONE,                            // P04
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P05
    GPIO_ATTRIBUTE_NONE,                            // P06
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P07

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P10
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P11
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P12
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P13
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P14
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P15
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P16
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P17

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P20
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P21
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P22
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P23
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P24
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P25
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P26
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P27

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P30
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P31
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P32
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P33
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P34
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P35
    GPIO_ATTRIBUTE_NONE,                            // P36
    GPIO_ATTRIBUTE_NONE,                            // P37

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P40
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P41
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P42
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P43
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P44
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P45
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P46
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P47

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P50
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P51
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P52
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P53
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P54
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P55
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P56
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P57

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P60
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P61
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P62
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P63
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P64
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P65
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P66
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P67

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P70
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P71
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P72
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P73
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P74
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P75
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P76
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P77

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P80
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P81
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P82
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P83
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P84
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P85
    GPIO_ATTRIBUTE_NONE,                            // P86
    GPIO_ATTRIBUTE_NONE,                            // P87

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P90
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P91
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P92
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P93
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P94
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P95
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P96
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // P97

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PA7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PB7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PC7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PD7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PE7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PF0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PF1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PF2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PF3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PF4
    GPIO_ATTRIBUTE_NONE,                            // PF5
    GPIO_ATTRIBUTE_NONE,                            // PF6
    GPIO_ATTRIBUTE_NONE,                            // PF7

    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG0
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG3
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG4
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT,   // PG6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT    // PG7
};

INT32 RX62N_GPIO_Driver::GetBitPos(UINT8 val)
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

void RX62N_GPIO_Driver::UpdatePinValue(GPIO_PIN pin)
{
    UINT32 port = GPIO_PORT(pin);
    UINT32 mask = GPIO_MASK(pin);
    g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[port] &= ~mask;
    g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[port] |= (_PIDR(port) & mask);
}

void RX62N_GPIO_Driver::EnableInputPolling(GPIO_PIN pin)
{
    if (pin < c_MaxPins)
        g_RX62N_GPIO_Driver.m_GPIO_PortReservedMask[GPIO_PORT(pin)] |= GPIO_MASK(pin);
}
void RX62N_GPIO_Driver::DisableInputPolling(GPIO_PIN pin)
{
    if (pin < c_MaxPins)
        g_RX62N_GPIO_Driver.m_GPIO_PortReservedMask[GPIO_PORT(pin)] &= ~GPIO_MASK(pin);
}

void RX62N_GPIO_Driver::ISR(void)
{
    UINT32 pin;
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (g_RX62N_GPIO_Driver.m_GPIO_InISR == false) {
        g_RX62N_GPIO_Driver.m_GPIO_InISR = true;
        pin = ChkPinChange();
        if (pin != 255) {
            PIN_ISR_DESCRIPTOR& pinIsr = g_RX62N_GPIO_Driver.m_PinIsr[pin];
            if (pinIsr.m_intEdge != GPIO_INT_NONE)
                pinIsr.Fire((void *)&pinIsr);
        }
        g_RX62N_GPIO_Driver.m_GPIO_InISR = false;
    }
    g_RX62N_GPIO_Driver.m_GPIO_Completion.EnqueueDelta(RX62N_GPIO_POLLING_INTERVAL);
    return;
}

UINT32 RX62N_GPIO_Driver::ChkPinChange(void)
{
    INT32 i, pos;
    UINT8 val, b, mask;
    UINT32 pin = 255;
    GLOBAL_LOCK(irq);
    for (i = 0; i < c_MaxPorts; i++) {
        mask = g_RX62N_GPIO_Driver.m_GPIO_PortReservedMask[i];
        if (mask) {
            val = VB(0x0008c040 + i) & mask;
            if (g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[i] != val) {
                b = g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[i] ^ val;
                g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[i] = val;
                pos = GetBitPos(b);
                if (pos != -1) {
                    pin = i*8 + pos;
                    if (pin != 255) {
                        PIN_ISR_DESCRIPTOR& pinIsr = g_RX62N_GPIO_Driver.m_PinIsr[pin];
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

void RX62N_GPIO_Driver::PIN_ISR_DESCRIPTOR::Fire(void *arg)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    PIN_ISR_DESCRIPTOR *desc = (PIN_ISR_DESCRIPTOR *)arg;
    desc->m_isr(desc->m_pin, (desc->m_status & c_Status_AllowHighEdge) != 0, desc->m_param);
    UINT8 mask = c_Flags_RequireHighEdge | c_Flags_RequireLowEdge;
    if ((desc->m_flags & mask) == mask) {
        desc->m_status ^= PIN_ISR_DESCRIPTOR::c_Status_AllowLowEdge | PIN_ISR_DESCRIPTOR::c_Status_AllowHighEdge;
    }
}

void RX62N_GPIO_Driver::STUB_GPIOISRVector(GPIO_PIN Pin, BOOL PinState, void *Param)
{
}

BOOL RX62N_GPIO_Driver::Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    PIN_ISR_DESCRIPTOR *pinIsr = g_RX62N_GPIO_Driver.m_PinIsr;
    for (UINT8 port = 0; port < c_MaxPorts; port++) {
        g_RX62N_GPIO_Driver.m_GPIO_PortReservedMask[port] = 0;
        g_RX62N_GPIO_Driver.m_GPIO_PortLastValues[port] = 0;
    }

    for (UINT8 pin = 0; pin < c_MaxPins; pin++) {
        g_RX62N_GPIO_Driver.m_GPIO_Reserved[pin] = false;

        pinIsr->m_pin = pin;
        pinIsr->m_intEdge = GPIO_INT_NONE;
        pinIsr->m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
        pinIsr->m_param = NULL;
        pinIsr->m_completion.Initialize();
        pinIsr->m_completion.InitializeForISR(&PIN_ISR_DESCRIPTOR::Fire, pinIsr);
        pinIsr++;
    }
    g_RX62N_GPIO_Driver.m_GPIO_Driver_Initialized = true;
    g_RX62N_GPIO_Driver.m_GPIO_Completion.Initialize();
    g_RX62N_GPIO_Driver.m_GPIO_Completion.InitializeForISR((HAL_CALLBACK_FPN)ISR, NULL);
    g_RX62N_GPIO_Driver.m_GPIO_Completion.EnqueueDelta(RX62N_GPIO_POLLING_INTERVAL);
    return true;
}

BOOL RX62N_GPIO_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    GLOBAL_LOCK(irq);
    g_RX62N_GPIO_Driver.m_GPIO_Driver_Initialized = false;
    PIN_ISR_DESCRIPTOR* pinIsr = g_RX62N_GPIO_Driver.m_PinIsr;
    for (UINT8 pin = 0; pin < c_MaxPins; pin++) {
        pinIsr->m_completion.Abort();
        pinIsr++;
    }
    g_RX62N_GPIO_Driver.m_GPIO_Completion.Abort();
    return true;
}

UINT32 RX62N_GPIO_Driver::Attributes(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    ASSERT(pin < c_MaxPins);

    if (pin < c_MaxPins) {
        return c_GPIO_Attributes[pin];
    }
    return GPIO_ATTRIBUTE_NONE;
}
 
void RX62N_GPIO_Driver::DisablePin(GPIO_PIN pin, GPIO_RESISTOR resistorState, UINT32 Direction, GPIO_ALT_MODE AltFunction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    GLOBAL_LOCK(irq);

    PIN_ISR_DESCRIPTOR& pinIsr = g_RX62N_GPIO_Driver.m_PinIsr[pin];
    pinIsr.m_intEdge = GPIO_INT_NONE;
    pinIsr.m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
    pinIsr.m_param = NULL;
    DisableInputPolling(pin);
}

void RX62N_GPIO_Driver::EnableOutputPin(GPIO_PIN pin, BOOL initialState)
{
    ASSERT(pin < c_MaxPins);
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
    PIN_ISR_DESCRIPTOR& pinIsr = g_RX62N_GPIO_Driver.m_PinIsr[pin];
    pinIsr.m_intEdge = GPIO_INT_NONE;
    pinIsr.m_isr = (GPIO_INTERRUPT_SERVICE_ROUTINE)STUB_GPIOISRVector;
    pinIsr.m_param = NULL;
    DisableInputPolling(pin);
}

BOOL RX62N_GPIO_Driver::EnableInputPin(GPIO_PIN pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* Param, GPIO_INT_EDGE intEdge, GPIO_RESISTOR resistorState)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    ASSERT(pin < c_MaxPins);
    BOOL flag = true;
    GLOBAL_LOCK(irq);
    PIN_ISR_DESCRIPTOR& pinIsr = g_RX62N_GPIO_Driver.m_PinIsr[pin];
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

BOOL RX62N_GPIO_Driver::GetPinState(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    ASSERT(pin < c_MaxPins);
    UINT32 port = GPIO_PORT(pin);
    UINT32 mask = GPIO_MASK(pin);
    GLOBAL_LOCK(irq);
    return ((_PIDR(port) & mask) != 0)? true:false;
}

void RX62N_GPIO_Driver::SetPinState(GPIO_PIN pin, BOOL pinState)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    ASSERT(pin < c_MaxPins);
    UINT32 port = GPIO_PORT(pin);
    UINT8 mask = GPIO_MASK(pin);
    GLOBAL_LOCK(irq);
    _PDR(port) |= mask;
    if (pinState)
        _PODR(port) |= mask;
    else
        _PODR(port) &= ~mask;
}

BOOL RX62N_GPIO_Driver::PinIsBusy(GPIO_PIN pin)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return g_RX62N_GPIO_Driver.m_GPIO_Reserved[pin];
}

BOOL RX62N_GPIO_Driver::ReservePin(GPIO_PIN pin, BOOL fReserve)
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    GLOBAL_LOCK(irq);
    g_RX62N_GPIO_Driver.m_GPIO_Reserved[pin] = fReserve;
    return true;
}

UINT32 RX62N_GPIO_Driver::GetDebounce()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return CPU_TicksToTime(g_RX62N_GPIO_Driver.m_DebounceTicks) / 10000;
}

BOOL RX62N_GPIO_Driver::SetDebounce(INT64 debounceTimeMilliseconds)
{   
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (debounceTimeMilliseconds < c_MinDebounceTimeMs || c_MaxDebounceTimeMs < debounceTimeMilliseconds)
        return false;
    g_RX62N_GPIO_Driver.m_DebounceTicks = CPU_MillisecondsToTicks((UINT32)debounceTimeMilliseconds);
    return true;
}

INT32 RX62N_GPIO_Driver::GetPinCount()
{
    return c_MaxPins;
}

void RX62N_GPIO_Driver::GetPinsMap(UINT8* pins, size_t size)
{
    const UINT8 *src = c_GPIO_Attributes;
    UINT8 *dst = pins;
    UINT32 maxpin = c_MaxPins;

    if (size == 0)
        return;
    while ((size--) && (maxpin --)) {
        *dst = *src;  
        ++dst; ++src;
    }
}

UINT8 RX62N_GPIO_Driver::GetSupportedResistorModes(GPIO_PIN pin)
{
    // ToDo
    return 0;
}


UINT8 RX62N_GPIO_Driver::GetSupportedInterruptModes(GPIO_PIN pin)
{
    // ToDo
    return 0;
}
