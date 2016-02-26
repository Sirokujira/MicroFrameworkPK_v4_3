////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestLCD.cpp
 *
 *  Created on: 2011/12/24
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_LCD
#include "sample_bmp.h"
unsigned short P8_LCD_read_dat(volatile unsigned short index);

void LCD_reg_dump(void)
{
    int i = 0;
    for (i = 0; i < 1; i++) {
        debug_printf("LCD reg[%02x] = %04x\r\n", i, P8_LCD_read_dat((volatile unsigned short)i));
    }
}

void TestLCD(void)
{
    LCD_Initialize();
    LCD_Clear();
    LCD_BitBltEx(12, 0, 100, 128, (UINT32 *)bmp);
}
#endif
