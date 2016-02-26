////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestEvent.cpp
 *
 *  Created on: 2011/12/24
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_EVENTS
void TestEvents(void)
{
    char key[1];
    int i = 0;
#if defined(PLATFORM_ARM_CQ_FRK_FM3) | defined(PLATFORM_ARM_WKFM3) | defined(PLATFORM_ARM_WXMP3PLCD_FM3)
    GPIO_PIN pin = (GPIO_PIN)0xF3;
#endif
#if defined(PLATFORM_RX62N_CQ_FRK_RX62N) | defined(PLATFORM_RX62N_WKLCD62N) | defined(PLATFORM_RX62N_WXMP3PLCD)
    GPIO_PIN pin = (GPIO_PIN)0x0D;
#endif
#if defined(PLATFORM_RX63N_GR_SAKURA)
    GPIO_PIN pin = (GPIO_PIN)86;
#endif
    BOOL state = TRUE;
    CPU_GPIO_EnableOutputPin(pin, state);
    while (true) {
        CPU_GPIO_SetPinState(pin, state);
        dbg_printf("Waiting key %d:", i);
#if 1
        UINT32 Events = Events_WaitForEvents(SYSTEM_EVENT_FLAG_COM_IN, 1000);
        if (Events & SYSTEM_EVENT_FLAG_COM_IN) {
            Events_Clear(SYSTEM_EVENT_FLAG_COM_IN);
            DebuggerPort_Read(COM1, key, 1);
            dbg_printf("%c\r\n", key[0]);
        } else {
            dbg_printf("\r\n");
        }
#else
        dbg_printf("\r\n");
        Events_WaitForEvents(0, 1000);
#endif
        state ^= 1;
        i++;
    }
}
#endif
