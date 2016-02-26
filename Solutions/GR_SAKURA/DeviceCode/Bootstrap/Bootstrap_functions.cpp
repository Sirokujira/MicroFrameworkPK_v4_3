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
#include "..\..\..\DeviceCode\Targets\Native\RX63N\DeviceCode\iodefine.h"

//--//

#ifdef __cplusplus
extern "C" {
#endif
void BootstrapCode_Ext_SRAM(void)  __attribute__((section("SectionForBootstrapOperations")));
void BootstrapCode_Board(void)  __attribute__((section("SectionForBootstrapOperations")));
#ifdef __cplusplus
}
#endif

void BootstrapCode_Ext_SRAM(void) __attribute__ ((section ("SectionForBootstrapOperations")));
void BootstrapCode_Ext_SRAM(void)
{
}

void BootstrapCode_Board(void) __attribute__ ((section ("SectionForBootstrapOperations")));
void BootstrapCode_Board(void)
{
    SYSTEM.PRCR.WORD = 0xA502;
    SYSTEM.SYSCR0.WORD = 0x5a03;
	MPC.PWPR.BIT.B0WI = 0;  /* Enable write to PFSWE */
	MPC.PWPR.BIT.PFSWE = 1; /* Enable write to PFS */
	// ===== GR-SAKURA LCD Shield =====
	// LCD_CS: GND
	// LCD_RS: A17
	// LCD_RESET: P51
	MPC.PFCSE.BIT.CS3E = 1;
	MPC.PFCSS0.BIT.CS3S = 2;
	MPC.PFAOE1.BIT.A17E = 1;
	BSC.CS3CR.WORD = 0x0001 | (2 << 4);
	BSC.CS3MOD.WORD = 0x8001;
	BSC.CS3WCR1.LONG = 0x03030303;
	BSC.CS3WCR2.LONG = 0x11110111;
	BSC.CS3REC.WORD = 0x0000;
    SYSTEM.PRCR.WORD = 0xA500;
}
