////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestAD.cpp
 *
 *  Created on: 2011/12/24
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_AD
#if defined(PLATFORM_ARM_CQ_FRK_FM3) | defined(PLATFORM_ARM_WKFM3) | defined(PLATFORM_ARM_WXMP3PLCD_FM3)
#define AD_CH    (16)
#endif
#if defined(PLATFORM_RX63N_GR_SAKURA)
#define AD_CH    (0)
#endif

void TestAD(void)
{
    ANALOG_CHANNEL channel = (ANALOG_CHANNEL)AD_CH;
    AD_Initialize(channel, 12);
    GPIO_PIN pin = AD_GetPinForChannel(channel);

    UINT32 i = 100;
    while (i-- >0) {
        UINT32 value = AD_Read(channel);
        Events_WaitForEvents(0, 1000);
        dbg_printf("A/D %04d\r\n", value);
    }
}
#endif
