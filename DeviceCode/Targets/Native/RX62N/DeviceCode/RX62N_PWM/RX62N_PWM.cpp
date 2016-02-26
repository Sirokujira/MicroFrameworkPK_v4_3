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

struct RX62N_PWM_Driver
{
    static const UINT32 c_Channels = 3;

    static const GPIO_PIN c_Channel_0_Pin1 = RX62N_GPIO::P15;   // PWM Mode 2   0B
    static const GPIO_PIN c_Channel_0_Pin2 = RX62N_GPIO::P32;   // PWM Mode 2   0C
    static const GPIO_PIN c_Channel_0_Pin3 = RX62N_GPIO::P33;   // PWM Mode 2   0D
    static const GPIO_PIN c_Channel_1_Pin1 = RX62N_GPIO::P17;   // PWM Mode 1   3A
    static const GPIO_PIN c_Channel_1_Pin2 = RX62N_GPIO::P16;   // PWM Mode 1   3C
    static const GPIO_PIN c_Channel_2_Pin1 = RX62N_GPIO::P24;   // PWM Mode 1   4A
    static const GPIO_PIN c_Channel_2_Pin2 = RX62N_GPIO::P25;   // PWM Mode 1   4C

    void RX62_PWM_Driver(void);
    BOOL Initialize(PWM_CHANNEL channel);
    BOOL Uninitialize(PWM_CHANNEL channel);
#ifdef NETMF42_RTM
    BOOL ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, BOOL invert);
#else
    BOOL ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, PWM_SCALE_FACTOR& scale, BOOL invert);
#endif
    BOOL Start(PWM_CHANNEL channel, GPIO_PIN pin);
    void Stop(PWM_CHANNEL channel, GPIO_PIN pin);
    BOOL Start(PWM_CHANNEL *channel, GPIO_PIN* pin, UINT32 count);
    void Stop(PWM_CHANNEL *channel, GPIO_PIN* pin, UINT32 count);
    UINT32 Channels(void);
    GPIO_PIN GetPinForChannel(PWM_CHANNEL channel);
    void EnablePin(PWM_CHANNEL channel);
    void DisablePin(PWM_CHANNEL channel);
    static const GPIO_PIN c_Pins[c_Channels];
};

const GPIO_PIN RX62N_PWM_Driver::c_Pins[RX62N_PWM_Driver::c_Channels] = {
    RX62N_GPIO::RX62N_GPIO::P15,
    RX62N_GPIO::RX62N_GPIO::P17,
    RX62N_GPIO::RX62N_GPIO::P24
};

RX62N_PWM_Driver g_RX62N_PWM_Driver;

BOOL PWM_Initialize(PWM_CHANNEL channel)
{
    return g_RX62N_PWM_Driver.Initialize(channel);
}

BOOL PWM_Uninitialize(PWM_CHANNEL channel)
{
    return g_RX62N_PWM_Driver.Uninitialize(channel);
}

#ifdef NETMF42_RTM
BOOL PWM_ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, BOOL invert)
#else
BOOL PWM_ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, PWM_SCALE_FACTOR& scale, BOOL invert)
#endif
{
#ifdef NETMF42_RTM
    return g_RX62N_PWM_Driver.ApplyConfiguration(channel, pin, period, duration, invert);
#else
    return g_RX62N_PWM_Driver.ApplyConfiguration(channel, pin, period, duration, scale, invert);
#endif
}

BOOL PWM_Start(PWM_CHANNEL channel, GPIO_PIN pin)
{
    return g_RX62N_PWM_Driver.Start(channel,pin);
}

void PWM_Stop(PWM_CHANNEL channel, GPIO_PIN pin)
{
    return g_RX62N_PWM_Driver.Stop(channel,pin);
}

BOOL PWM_Start(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
    return g_RX62N_PWM_Driver.Start(channel, pin, count);
}

void PWM_Stop(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
    return g_RX62N_PWM_Driver.Stop(channel, pin, count);
}

UINT32 PWM_PWMChannels() 
{
    return g_RX62N_PWM_Driver.Channels();
}

GPIO_PIN PWM_GetPinForChannel(PWM_CHANNEL channel)
{
    return g_RX62N_PWM_Driver.GetPinForChannel(channel);
}

//--//

UINT32 PWM_Clkdev_Get(PWM_SCALE_FACTOR& scale, UINT32 period)
{
    UINT32 clkdev;
    if ((scale == PWM_NANOSECONDS) && (period < 1000000))
        clkdev = 1;
    else if ((scale == PWM_NANOSECONDS) && (period >= 1000000))
        clkdev = 64;
    else if ((scale == PWM_MICROSECONDS) && (period <= 10000))
        clkdev = 64;
    else
        clkdev = 1024;
    return clkdev;
}

void PWM_Clock_Set(PWM_CHANNEL channel, UINT32 clkdev)
{
    switch (channel) {
    case 0:
        switch (clkdev) {
        case 1:
            MTU0.TCR.BIT.TPSC = 0x0;
            break;
        case 64:
        default:
            MTU0.TCR.BIT.TPSC = 0x3;
            break;
        }
        break;
    case 1:
        switch (clkdev) {
        case 1:
            MTU3.TCR.BIT.TPSC = 0x0;
            break;
        case 64:
            MTU3.TCR.BIT.TPSC = 0x3;
            break;
        case 1024:
        default:
            MTU3.TCR.BIT.TPSC = 0x5;
            break;
        }
        break;
    case 2:
    default:
        switch (clkdev) {
        case 1:
            MTU4.TCR.BIT.TPSC = 0x0;
            break;
        case 64:
            MTU4.TCR.BIT.TPSC = 0x3;
            break;
        case 1024:
        default:
            MTU4.TCR.BIT.TPSC = 0x5;
            break;
        }
        break;
    }
}

BOOL RX62N_PWM_Driver::Initialize(PWM_CHANNEL channel)
{
    SYSTEM.MSTPCRA.BIT.MSTPA9 = 0;  // SYSTEM.MSTPCRA.BIT.MSTPA9
    switch (channel)
    {
    case 0:
        MTUA.TSTR.BYTE &= 0xFE;     // stop counter
        MTU0.TCR.BIT.CCLR = 0x6;    // compare match clear by TGRD
        //MTU0.TCR.BIT.TPSC = 0x3;    // compare match clear PCLK/64
        MTU0.TMDR.BIT.MD = 0x3;     // PWM mode 2
        MTU0.TIORH.BIT.IOB = 0x5;   // compare match high
        //MTU0.TIORL.BIT.IOC = 0x6; // compare match high
        MTU0.TIORL.BIT.IOD = 0x6;   // compare match low
        break;
    case 1:
        MTUA.TSTR.BYTE &= 0xBF;     // stop counter
        MTU3.TCR.BIT.CCLR = 0x1;    // compare match clear by TGRA
        //MTU3.TCR.BIT.TPSC = 0x3;    // compare match clear PCLK/64
        MTU3.TMDR.BIT.MD = 0x2;     // PWM mode 1
        MTU3.TIORH.BIT.IOA = 0x6;   // compare match high
        MTU3.TIORH.BIT.IOB = 0x5;   // compare match low
        //MTU3.TIORL.BIT.IOC = 0x6; // compare match high
        //MTU3.TIORL.BIT.IOD = 0x5; // compare match low
        break;
    case 2:
        MTUA.TSTR.BYTE &= 0x7F;     // stop counter
        MTU4.TCR.BIT.CCLR = 0x1;    // compare match clear by TGRA
        //MTU4.TCR.BIT.TPSC = 0x3;    // compare match clear PCLK/64
        MTU4.TMDR.BIT.MD = 0x2;     // PWM mode 1
        MTU4.TIORH.BIT.IOA = 0x6;   // compare match high
        MTU4.TIORH.BIT.IOB = 0x5;   // compare match low
        //MTU4.TIORL.BIT.IOC = 0x6; // compare match high
        //MTU4.TIORL.BIT.IOD = 0x5; // compare match low
        MTUA.TOER.BIT.OE4A = 1;
        //MTUA.TOER.BIT.OE4C = 1;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL RX62N_PWM_Driver::Uninitialize(PWM_CHANNEL channel)
{
    return TRUE;
}

#ifdef NETMF42_RTM
BOOL RX62N_PWM_Driver::ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, BOOL invert)
#else
BOOL RX62N_PWM_Driver::ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, PWM_SCALE_FACTOR& scale, BOOL invert)
#endif
{
    UINT32 period_ticks;
    UINT32 duration_ticks;
    UINT32 clkdev = PWM_Clkdev_Get(scale, period);
    PWM_Clock_Set(channel, clkdev);
    float ratio = (float)( (SYSTEM_CLOCK_HZ/2) / clkdev) / (float)scale;
    period_ticks = (UINT32)((float)period *  ratio);
    duration_ticks = (UINT32)((float)duration *  ratio);
    switch (channel)
    {
    case 0:
        MTU0.TGRD = period_ticks;
        MTU0.TGRB = duration_ticks;
        //MTU0.TGRC = duration_ticks;
        MTU0.TCNT = 0;
        break;
    case 1:
        MTU3.TGRA = period_ticks;
        MTU3.TGRB = duration_ticks;
        //MTU3.TGRC = period_ticks;
        //MTU3.TGRD = duration_ticks;
        MTU3.TCNT = 0;
        break;
    case 2:
        MTU4.TGRA = period_ticks;
        MTU4.TGRB = duration_ticks;
        //MTU4.TGRC = period_ticks;
        //MTU4.TGRD = duration_ticks;
        MTU4.TCNT = 0;
        break;
    default:
        return FALSE;
    }
#ifdef DEBUG_PWM
    //lcd_printf("C/D/P %d/%04x/%04x\r\n", (UINT32)channel, (UINT16)duration_ticks, (UINT16)period_ticks);
    //lcd_printf("S %d\r\n", (UINT32)scale);
    lcd_printf("Dt/D/P %04x/%06x/%06x\r\n", (UINT16)duration_ticks, duration, period);
#endif
    return TRUE;
}

void RX62N_PWM_Driver::EnablePin(PWM_CHANNEL channel)
{
    //CPU_GPIO_DisablePin(g_RX62N_PWM_Driver.c_Pins[channel], RESISTOR_DISABLED, 0, GPIO_ALT_MODE_2);
    switch (channel)
    {
    case 0:
        MTUA.TSTR.BYTE |= 0x01;     // stop counter
        break;
    case 1:
        MTUA.TSTR.BYTE |= 0x40;     // stop counter
        break;
    case 2:
        MTUA.TSTR.BYTE |= 0x80;     // stop counter
        break;
    default:
        break;
    }
}

void RX62N_PWM_Driver::DisablePin(PWM_CHANNEL channel)
{
    switch (channel)
    {
    case 0:
        MTUA.TSTR.BYTE &= 0xFE;    // start counter
        break;
    case 1:
        MTUA.TSTR.BYTE &= 0xBF;    // start counter
        break;
    case 2:
        MTUA.TSTR.BYTE &= 0x7F;    // start counter
        break;
    default:
        break;
    }
    //CPU_GPIO_DisablePin(g_RX62N_PWM_Driver.c_Pins[channel], RESISTOR_PULLDOWN, 0, GPIO_ALT_PRIMARY);
}

BOOL RX62N_PWM_Driver::Start(PWM_CHANNEL channel, GPIO_PIN pin)
{
    EnablePin(channel);
    return TRUE;
}

void RX62N_PWM_Driver::Stop(PWM_CHANNEL channel, GPIO_PIN pin)
{    
    DisablePin(channel);
}

BOOL RX62N_PWM_Driver::Start(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
    return TRUE;
}

void RX62N_PWM_Driver::Stop(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
}

UINT32 RX62N_PWM_Driver::Channels()
{
    return c_Channels;
}

GPIO_PIN RX62N_PWM_Driver::GetPinForChannel(PWM_CHANNEL channel)
{
    if ((UINT32)channel >= c_Channels)
        return GPIO_PIN_NONE;
    return g_RX62N_PWM_Driver.c_Pins[channel];
}
