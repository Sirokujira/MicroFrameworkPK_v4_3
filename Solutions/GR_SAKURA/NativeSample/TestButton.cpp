////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestButton.cpp
 *
 *  Created on: 2012/05/12
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_BUTTON
void TestButton(void)
{
    UINT32 ButtonsPressed = 0;
    UINT32 ButtonsReleased = 0;
    Buttons_Initialize();
    while (true) {
        UINT32 Events = Events_WaitForEvents(SYSTEM_EVENT_FLAG_BUTTON, 200);
        if (Events & SYSTEM_EVENT_FLAG_BUTTON) {
            Events_Clear(SYSTEM_EVENT_FLAG_BUTTON);
            while (Buttons_GetNextStateChange(ButtonsPressed, ButtonsReleased))
                ;
            if (ButtonsPressed)
                lcd_printf("ButtonPressed = 0x%08d.\r\n", ButtonsPressed);
            if (ButtonsReleased)
                lcd_printf("ButtonReleased = 0x%08d.\r\n", ButtonsReleased);
            if (ButtonsReleased & BUTTON_B0) {
                lcd_printf("Exiting\r\n");
                break;
            }
        }
    }
    Buttons_Uninitialize();
}
#endif
