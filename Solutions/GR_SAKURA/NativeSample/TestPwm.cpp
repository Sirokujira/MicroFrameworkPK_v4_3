////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011-2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestPwm.cpp
 *
 *  Created on: 2011/12/24
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_PWM

void TestPWM(void)
{
    PWM_CHANNEL channel = (PWM_CHANNEL)6;        // PF3
    PWM_Initialize(channel);
    GPIO_PIN pin = PWM_GetPinForChannel(channel);

    // from 90% to 2/3, to 50%, to 1/3 to 10%
    float dc[5] = {0.9, 0.666, 0.5, 0.333, 0.1};
    UINT32 period1 = 2000;    // 1Kxz
    for (UINT32 idx = 0; idx < 5; ++idx) {
        UINT32 duration1 = (UINT32)((float)period1 * dc[idx]);
        PWM_ApplyConfiguration(channel, pin, period1, duration1, FALSE);
        PWM_Start(channel, pin);
        //HAL_Time_Sleep_MicroSeconds_InterruptEnabled(1*1000*1000);
        Events_WaitForEvents(0, 2*1000);
        PWM_Stop(channel, pin);
    }
    // from 10Khz to 1Khz, 50% duty cycle
    for (UINT32 period = 10000; period >= 1000; period -= 1000) {
        UINT32 duration = period / 2;
        PWM_ApplyConfiguration(channel, pin, period, duration, FALSE);
        PWM_Start(channel, pin);
        Events_WaitForEvents(0, 2*1000);
        PWM_Stop(channel, pin);
    }
    PWM_Uninitialize(channel);
}
#endif
