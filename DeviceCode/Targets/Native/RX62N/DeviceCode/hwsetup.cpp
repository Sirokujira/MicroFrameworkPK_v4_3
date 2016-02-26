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
#include "iodefine.h"

//#define OP_OPTIMIZE

#ifdef __cplusplus
extern "C" {
#endif
    extern void HardwareSetup(void);
#ifdef __cplusplus
}
#endif

#ifdef MII_MODE
static inline void MII_mode(void)
{
    // MII_MODE
    // EE=1, PHYMODE=1, ENETE3=1, ENETE2=0, ENETE1=1, ENETE0=0 (Ethernet)
    IOPORT.PFENET.BYTE = 0x9A;
    PORT5.ICR.BIT.B4 = 1;    // P54=1 Set ET_LINKSTA input
    PORT7.ICR.BIT.B1 = 1;    // P71=1 Set ET_MDIO input
    PORT7.ICR.BIT.B4 = 1;    // P74=1 Set ET_ERXD1 input
    PORT7.ICR.BIT.B5 = 1;    // P75=1 Set ET_ERXD0 input
    PORT7.ICR.BIT.B6 = 1;    // P76=1 Set ET_RX_CLK input
    PORT7.ICR.BIT.B7 = 1;    // P77=1 Set ET_RX_ER input
    PORT8.ICR.BIT.B3 = 1;    // P83=1 Set ET_CRS input
    PORTC.ICR.BIT.B0 = 1;    // PC0=1 Set ET_ERXD3 input
    PORTC.ICR.BIT.B1 = 1;    // PC1=1 Set ET_ERXD2 input
    PORTC.ICR.BIT.B2 = 1;    // PC2=1 Set ET_RX_DV input
    PORTC.ICR.BIT.B4 = 1;    // PC4=1 Set EX_TX_CLK input
    PORTC.ICR.BIT.B7 = 1;    // PC7=1 Set ET_COL input
}
#else
static inline void RMII_mode(void)
{
    // RMII_MODE
    // EE=1, PHYMODE=0, ENETE3=0, ENETE2=0, ENETE1=1, ENETE0=0 (Ethernet)
    IOPORT.PFENET.BYTE = 0x82;
    PORT5.ICR.BIT.B4 = 1;    // P54=1 Set ET_LINKSTA input
    PORT7.ICR.BIT.B1 = 1;    // P71=1 Set ET_MDIO input
    PORT7.ICR.BIT.B4 = 1;    // P74=1 Set RMII_RXD1 input
    PORT7.ICR.BIT.B5 = 1;    // P75=1 Set RMII_RXD0 input
    PORT7.ICR.BIT.B6 = 1;    // P76=1 Set REF50CLK input
    PORT7.ICR.BIT.B7 = 1;    // P77=1 Set RMII_RX_ER input
    PORT8.ICR.BIT.B3 = 1;    // P83=1 Set RMII_CRS_DV input
}
#endif

void HardwareSetup(void)
{
    //SYSTEM.SCKCR.LONG = 0x00010100;     // ICLK=8x, BCLK=2x, PCLK=4x
    SYSTEM.SCKCR.LONG = 0x00020100;     // ICLK=8x, BCLK=4x, PCLK=4x
    // Setup Ehternet
    SYSTEM.MSTPCRB.BIT.MSTPB15 = 0;     // EtherC, EDMAC
#ifdef MII_MODE
    MII_mode();
#else
    RMII_mode();
#endif
}
