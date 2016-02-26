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
//  Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\RX63N\DeviceCode\RX63N.h"

void SPISW_GPIO_Initialize(void);

#define SPISWPIN    0

#if (SPISWPIN == 0)
// GR_SAKURA + Aitendo T18SPI LCD Shield
// 4 line SPI mode
// CLK  PC5
// DI   PC6
// CS   PC4
// RS   PC7
// Res  PC2

#define MOSI    (PORTC.PODR.BIT.B6)
#define CS_LOW  (PORTC.PODR.BIT.B4 = 0)
#define CS_HIGH (PORTC.PODR.BIT.B4 = 1)
#define CK_LOW  (PORTC.PODR.BIT.B5 = 0)
#define CK_HIGH (PORTC.PODR.BIT.B5 = 1)
#define DI_LOW  (PORTC.PODR.BIT.B6 = 0)
#define DI_HIGH (PORTC.PODR.BIT.B6 = 1)
#define RS_LOW  (PORTC.PODR.BIT.B7 = 0)
#define RS_HIGH (PORTC.PODR.BIT.B7 = 1)
#define RS_IN   (PORTC.PIDR.BIT.B7)
#define RES_LOW  (PORTC.PODR.BIT.B2 = 0)
#define RES_HIGH (PORTC.PODR.BIT.B2 = 1)
#endif

void SPISW_GPIO_Initialize(void)
{
#if (SPISWPIN == 0)
    PORTC.PMR.BIT.B2 = 0;
    PORTC.PMR.BIT.B4 = 0;
    PORTC.PMR.BIT.B5 = 0;
    PORTC.PMR.BIT.B6 = 0;
    PORTC.PMR.BIT.B7 = 0;

    PORTC.PDR.BIT.B2 = 1;
    PORTC.PDR.BIT.B4 = 1;
    PORTC.PDR.BIT.B5 = 1;
    PORTC.PDR.BIT.B6 = 1;
    PORTC.PDR.BIT.B7 = 1;
#endif
}

void SPISW_Initialize(void)
{
    SPISW_GPIO_Initialize();
    CS_HIGH;
    CK_HIGH;
}

void SPISW_Reset(UINT32 v)
{
    if (v) {
        RES_HIGH;
    } else {
        RES_LOW;
    }
}

void SPISW_Write(UINT8 dat)
{
    UINT8 i = 8;
    while (i-- > 0) {
        MOSI = (dat & 0x80) ? 1 : 0;
        CK_LOW;
        CK_HIGH;
        dat <<= 1;
    }
}

void SPISW_LCD_cmd(UINT8 dat)
{
    // Enter command mode: SDATA=LOW at rising edge of 1st SCLK
    CS_LOW;
    RS_LOW;
    SPISW_Write(dat);
    CS_HIGH;
}

void SPISW_LCD_dat(UINT8 dat)
{
    // Enter data mode: SDATA=HIGH at rising edge of 1st SCLK
    CS_LOW;
    RS_HIGH;
    SPISW_Write(dat);
    CS_HIGH;
}
