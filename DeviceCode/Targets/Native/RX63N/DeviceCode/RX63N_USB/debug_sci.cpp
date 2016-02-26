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
#include "..\RX63N.h"
#include "debug_printf.h"

#ifdef USB_DEBUG

void SCI2_Init(void)
{
    IOPORT.PFFSCI.BIT.SCI2S = 0;
    MSTP_SCI2 = 0;
    PORT1.ICR.BIT.B2 = 1;
    SCI2.SCR.BYTE = 0x00;
    SCI2.SMR.BYTE = 0x00;
    SCI2.BRR = 12;

    // wait for 1 bit time.
    for(int i = 1000000; i > 0; i--);

    SCI2.SCR.BIT.TE = 1;
    return;
}

void SCI2_TxStr(unsigned char* p)
{
    while (*p != 0) {
        if (*p == '\n') {
            SCI2.TDR = '\r';
            while(SCI2.SSR.BIT.TDRE == 0);
            while(SCI2.SSR.BIT.TEND == 0);
        }
        SCI2.TDR = *p;
        while(SCI2.SSR.BIT.TDRE == 0);
        while(SCI2.SSR.BIT.TEND == 0);
        p++;
    }
    return;
}
#endif
