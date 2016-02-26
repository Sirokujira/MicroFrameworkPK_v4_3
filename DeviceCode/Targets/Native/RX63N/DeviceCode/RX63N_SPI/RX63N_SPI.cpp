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

//#define SPI_DEBUG

#include <tinyhal.h>
#include "..\RX63N.h"
#include "RX63N_SPI.h"

vp_ir g_spri[] = {
    (vp_ir)(0x87000 + IR_RSPI0_SPRI0),
    (vp_ir)(0x87000 + IR_RSPI1_SPRI1),
    (vp_ir)(0x87000 + IR_RSPI2_SPRI2)
};

vp_ier g_ier[] = {
    (vp_ier)(0x87200 + IER_RSPI0_SPRI0),
    (vp_ier)(0x87200 + IER_RSPI1_SPRI1),
    (vp_ier)(0x87200 + IER_RSPI2_SPRI2)
};

unsigned char g_ier_bit[] = {
    7,      // IEN7
    2,      // IEN2
    5,      // IEN5
};

vp_ipr g_ipr[] = {
    (vp_ipr)(0x87300 + IPR_RSPI0_SPRI0),
    (vp_ipr)(0x87300 + IPR_RSPI1_SPRI1),
    (vp_ipr)(0x87300 + IPR_RSPI2_SPRI2)
};

vp_rspi g_rspi[] = {
    (vp_rspi)0x88380,   // RSPI0
    (vp_rspi)0x883A0,   // RSPI1
    (vp_rspi)0x883C0    // RSPI2
};

UINT8 SPI_PINS[] = {
    RX63N_GPIO::PC5,    // RSPCKA
    RX63N_GPIO::PC6,    // MOSIA
    RX63N_GPIO::PC7,    // MISOA
    RX63N_GPIO::PE5,    // RSPCKB
    RX63N_GPIO::PE6,    // MOSIB
    RX63N_GPIO::PE7,    // MISOB
    RX63N_GPIO::PD3,    // RSPCKC
    RX63N_GPIO::PD1,    // MOSIC
    RX63N_GPIO::PD2     // MISOC
};

RX63N_SPI_Driver g_RX63N_SPI_Driver;

void SetMSTP(UINT32 spi_mod, int bit)
{
    switch (spi_mod) {
    case 0:
        SYSTEM.MSTPCRB.BIT.MSTPB17 = bit;
        break;
    case 1:
        SYSTEM.MSTPCRB.BIT.MSTPB18 = bit;
        break;
    default:
        SYSTEM.MSTPCRC.BIT.MSTPC22 = bit;
        break;
    }
}

void SetSPRI(UINT32 spi_mod, int bit)
{
    g_spri[spi_mod]->IR = bit;
}

BOOL ChkSPRI(UINT32 spi_mod, int bit)
{
    return (g_spri[spi_mod]->IR == bit);
}

void SetIER(UINT32 spi_mod, int bit)
{
    if (bit)
        g_ier[spi_mod]->BYTE |= (1 << g_ier_bit[spi_mod]);
    else
        g_ier[spi_mod]->BYTE &= ~(1 << g_ier_bit[spi_mod]);
}

void SetIPR(UINT32 spi_mod, int bit)
{
    g_ipr[spi_mod]->IPR = bit;
}

void RX63N_SPI_Driver::SetSPIBits(UINT32 spi_mod, UINT32 spi_bits)
{
    vp_rspi prspi = g_rspi[spi_mod];
    if (spi_bits)
        prspi->SPCMD0.WORD = 0xEF80; // Command Reg: SPI mode: MODE0, 16bit, MSB first
    else
        prspi->SPCMD0.WORD = 0xE780; // Command Reg: SPI mode: MODE0, 8bit, MSB first
}

void RX63N_SPI_Driver::SetSPIClk(UINT32 spi_mod, UINT32 spi_clk)
{
    if (spi_clk == 0)
        return;
    vp_rspi prspi = g_rspi[spi_mod];
    prspi->SPCR.BIT.SPE = 0;
    prspi->SPBR = BCLK / 2 / spi_clk - 1;
    prspi->SPCR.BIT.SPE = 1;
}

void SetPMR(UINT32 pin, UINT32 value)
{
    UINT32 port = pin >> 3;
    UINT32 bit = pin & 0x7;
    if (value)
        _PMR(port) |= (1 << bit);
    else
        _PMR(port) &= ~(1 << bit);
}

void SelectSPIPin(UINT32 spi_mod)
{
    UINT32 pin0 = SPI_PINS[spi_mod*3+0];
    UINT32 pin1 = SPI_PINS[spi_mod*3+1];
    UINT32 pin2 = SPI_PINS[spi_mod*3+2];
    SetPMR(pin0, 0);
    SetPMR(pin1, 0);
    SetPMR(pin2, 0);
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    _MPC(pin0) = 13;
    _MPC(pin1) = 13;
    _MPC(pin2) = 13;
    MPC.PWPR.BIT.PFSWE = 0;
    SetPMR(pin0, 1);
    SetPMR(pin1, 1);
    SetPMR(pin2, 1);
}

void DeselectSPIPin(UINT32 spi_mod)
{
    UINT32 pin0 = SPI_PINS[spi_mod*3+0];
    UINT32 pin1 = SPI_PINS[spi_mod*3+1];
    UINT32 pin2 = SPI_PINS[spi_mod*3+2];
    SetPMR(pin0, 0);
    SetPMR(pin1, 0);
    SetPMR(pin2, 0);
}

void RX63N_SPI_Driver::SetSPIMod(UINT32 spi_mod)
{
    vp_rspi prspi = g_rspi[spi_mod];
    SYSTEM.PRCR.WORD = 0xA502;
    SetMSTP(spi_mod, 0);
    SYSTEM.PRCR.WORD = 0xA500;
#if 1
    SelectSPIPin(spi_mod);
#else
    switch (spi_mod)
    {
    case 0:
        PORTC.PMR.BIT.B5 = 0;
        PORTC.PMR.BIT.B6 = 0;
        PORTC.PMR.BIT.B7 = 0;
        MPC.PWPR.BIT.B0WI = 0;
        MPC.PWPR.BIT.PFSWE = 1;
        MPC.PC5PFS.BYTE = 13;   // RSPI RSPCKA
        MPC.PC6PFS.BYTE = 13;   // RSPI MOSIA
        MPC.PC7PFS.BYTE = 13;   // RSPI MISOA
        MPC.PWPR.BIT.PFSWE = 0;
        //PORTC.ODR0.BYTE = 0;
        //PORTC.ODR1.BYTE = 0;
        PORTC.PMR.BIT.B5 = 1;
        PORTC.PMR.BIT.B6 = 1;
        PORTC.PMR.BIT.B7 = 1;
        break;
    case 1:
        break;
    default:
        break;
    }
#endif
    SetIPR(spi_mod, 0);
    SetIER(spi_mod, 0);
    prspi->SPCR.BYTE = 0;        // Stop SPI
    prspi->SPSR.BYTE = 0xa0;     //
    prspi->SPPCR.BYTE = 0;       // Fixed idle value, disable loop-back mode
    prspi->SPSCR.BYTE = 0;       // Disable sequence control
    prspi->SPDCR.BYTE = 0x20;    // SPLW=1 long access
    prspi->SPCMD0.WORD = 0x0700; // LSBF=0, SPB=7, BRDV=0, CPOL=0, CPHA=0
    //prspi->SPBR.BYTE = BCLK/2/CLK_SLOW - 1;  //Bit rate
    prspi->SPCR.BYTE = 0xC9;     // Start SPI in master mode
    prspi->SPCR2.BYTE = 0;
}

void RX63N_SPI_Driver::ResetSPIMod(UINT32 spi_mod)
{
    SYSTEM.PRCR.WORD = 0xA502;
    SetMSTP(spi_mod, 1);
    DeselectSPIPin(spi_mod);
    SYSTEM.PRCR.WORD = 0xA500;
}

BOOL RX63N_SPI_Driver::Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    for (int i = 0; i < c_Max_SPI; i++) {
        g_RX63N_SPI_Driver.m_Enabled[i] = FALSE;
    }
    return TRUE;
}

void RX63N_SPI_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
}

// Dummy ISR()
// Not used.
void RX63N_SPI_Driver::ISR(void *Param)
{
    SPI_TRACE1(CH_BOTH, "ISR NG\r\n");
    ASSERT(0);
}

BOOL RX63N_SPI_Driver::Xaction_Start(const SPI_CONFIGURATION &Configuration)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    int spi_mod = Configuration.SPI_mod;
    if (spi_mod < 0 || spi_mod >=  c_Max_SPI) {
        return false;
    }
    if (!g_RX63N_SPI_Driver.m_Enabled[spi_mod]) {
        g_RX63N_SPI_Driver.m_Enabled[spi_mod] = TRUE;
        SetSPIMod(spi_mod);
        SetSPIClk(spi_mod, Configuration.Clock_RateKHz);
        SetSPIBits(spi_mod, Configuration.MD_16bits);
        CPU_GPIO_EnableOutputPin(Configuration.DeviceCS, Configuration.CS_Active);
        if (Configuration.CS_Setup_uSecs) {
            HAL_Time_Sleep_MicroSeconds_InterruptEnabled(Configuration.CS_Setup_uSecs);
        }
    } else {
        SPI_TRACE1(CH_BOTH, "Xaction_Start NG\r\n");
        return FALSE;
    }
    return TRUE;
}

BOOL RX63N_SPI_Driver::Xaction_Stop(const SPI_CONFIGURATION &Configuration)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    int spi_mod = Configuration.SPI_mod;
    if (spi_mod < 0 || spi_mod >=  c_Max_SPI) {
        return false;
    }
    if (g_RX63N_SPI_Driver.m_Enabled[spi_mod]) {
        ResetSPIMod(spi_mod);
        if(Configuration.CS_Hold_uSecs) {
            HAL_Time_Sleep_MicroSeconds_InterruptEnabled(Configuration.CS_Hold_uSecs);
        }
        CPU_GPIO_SetPinState(Configuration.DeviceCS, !Configuration.CS_Active);
        g_RX63N_SPI_Driver.m_Enabled[spi_mod] = FALSE;
    } else {
        ResetSPIMod(spi_mod);
        SPI_TRACE1(CH_BOTH, "Xaction_Stop NG\r\n");
        return FALSE;
    }
    return TRUE;
}

BOOL RX63N_SPI_Driver::Xaction_nWrite16_nRead16(SPI_XACTION_16 &Transaction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    UINT16 Data16;
    UINT32 spi_mod = Transaction.SPI_mod;
    vp_rspi prspi = g_rspi[spi_mod];

    if (!g_RX63N_SPI_Driver.m_Enabled[spi_mod]) {
        SPI_TRACE1(CH_BOTH, "Xaction_nWrite16_nRead16 NG\r\n");
        return FALSE;
    }

    UINT16  *Write16    = Transaction.Write16;
    INT32   WriteCount  = Transaction.WriteCount;
    UINT16  *Read16     = Transaction.Read16;
    INT32   ReadCount   = Transaction.ReadCount;
    INT32   ReadStartOffset = Transaction.ReadStartOffset;
    INT32   ReadTotal   = 0;

    if ((WriteCount <= 0) ||
        (Write16 == NULL) ||
        ((ReadCount > 0) && (Read16 == NULL))) {
        SPI_TRACE1(CH_BOTH, "Xaction_nWrite16_nRead16 NG\r\n");
        return FALSE;
    }
    if (ReadCount) {
        ReadTotal = ReadCount + ReadStartOffset;
    }
    INT32 loopCnt = ReadTotal;
    if (loopCnt < WriteCount)
        loopCnt = WriteCount;
    WriteCount -= 1;
    GLOBAL_LOCK(irq);
    while (loopCnt--) {
        SetSPRI(spi_mod, 0);
        prspi->SPDR.LONG = (UINT32)(*Write16);
        while (ChkSPRI(spi_mod, 0));
        Data16 = (UINT16)(prspi->SPDR.LONG);
        if (WriteCount) {
            WriteCount--;
            Write16++;
        }
        ReadTotal--;
        if ((ReadTotal>=0) && (ReadTotal < ReadCount)) {
            Read16[0] = Data16;
            Read16++;
        }
    }
    return TRUE;
}

BOOL RX63N_SPI_Driver::Xaction_nWrite8_nRead8(SPI_XACTION_8 &Transaction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    UINT8 Data8;
    UINT32 spi_mod = Transaction.SPI_mod;
    vp_rspi prspi = g_rspi[spi_mod];

    if (!g_RX63N_SPI_Driver.m_Enabled[spi_mod]) {
        SPI_TRACE1(CH_BOTH, "Xaction_nWrite8_nRead8 NG\r\n");
        return FALSE;
    }

    UINT8 *Write8 = Transaction.Write8;
    INT32 WriteCount = Transaction.WriteCount;
    UINT8 *Read8 = Transaction.Read8;
    INT32 ReadCount = Transaction.ReadCount;
    INT32 ReadStartOffset = Transaction.ReadStartOffset;
    INT32 ReadTotal = 0;
    if ((WriteCount <= 0) ||
        (Write8 == NULL) ||
        ((ReadCount > 0) && (Read8 == NULL))) {
        SPI_TRACE1(CH_BOTH, "Xaction_nWrite8_nRead8 NG\r\n");
        return FALSE;
    }
    if (ReadCount) {
        ReadTotal = ReadCount + ReadStartOffset;
    }
    INT32 loopCnt = ReadTotal;
    if (loopCnt < WriteCount)
        loopCnt = WriteCount;
    WriteCount -= 1;
    GLOBAL_LOCK(irq);
    while (loopCnt--) {
        SetSPRI(spi_mod, 0);
        prspi->SPDR.LONG = (UINT32)(*Write8);
        while (ChkSPRI(spi_mod, 0));
        Data8 = (UINT8)(prspi->SPDR.LONG);
        if (WriteCount) {
            WriteCount--;
            Write8++;
        }
        ReadTotal--;
        if ((ReadTotal>=0) && (ReadTotal < ReadCount)) {
            Read8[0] = Data8;
            Read8++;
        }
    }
    return TRUE;
}

BOOL RX63N_SPI_Driver::nWrite16_nRead16(const SPI_CONFIGURATION &Configuration, UINT16 *Write16, INT32 WriteCount, UINT16 *Read16, INT32 ReadCount, INT32 ReadStartOffset)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    if (Configuration.SPI_mod >= c_Max_SPI) {
        SPI_TRACE1(CH_BOTH, "nWrite16_nRead16 NG\r\n");
        return FALSE;
    }
    if (Xaction_Start(Configuration)) {
        SPI_XACTION_16 Transaction;
        Transaction.Read16 = Read16;
        Transaction.ReadCount = ReadCount;
        Transaction.ReadStartOffset = ReadStartOffset;
        Transaction.Write16 = Write16;
        Transaction.WriteCount = WriteCount;
        Transaction.SPI_mod = Configuration.SPI_mod;
        Transaction.BusyPin = Configuration.BusyPin;
        if (Xaction_nWrite16_nRead16(Transaction))
            return Xaction_Stop(Configuration);
        else
            return FALSE;
    } else
        return FALSE;
}

BOOL RX63N_SPI_Driver::nWrite8_nRead8(const SPI_CONFIGURATION &Configuration, UINT8 *Write8, INT32 WriteCount, UINT8 *Read8, INT32 ReadCount, INT32 ReadStartOffset)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    if (Configuration.SPI_mod >= c_Max_SPI) {
        SPI_TRACE1(CH_BOTH, "nWrite8_nRead8 NG\r\n");
        return FALSE;
    }
    if (Xaction_Start(Configuration)) {
        SPI_XACTION_8 Transaction;
        Transaction.Read8 = Read8;
        Transaction.ReadCount = ReadCount;
        Transaction.ReadStartOffset = ReadStartOffset;
        Transaction.Write8 = Write8;
        Transaction.WriteCount = WriteCount;
        Transaction.SPI_mod = Configuration.SPI_mod;
        Transaction.BusyPin = Configuration.BusyPin;
        if (Xaction_nWrite8_nRead8(Transaction))
            return Xaction_Stop(Configuration);
        else
            return FALSE;
    } else
        return FALSE;
}

// Returns the SPI clock, MISO and MOSI pin numbers for a specified SPI module.
// spi_mod:    Specifies the SPI module on entry.
// msk:     The number of the clock pin (SCLK) is returned here.
// miso:    The number of the Master Input Subordinate Output (MISO) pin is returned here.
// mosi:    The number of the Master Output Subordinate Input (MOSI) pin is returned here.
void RX63N_SPI_Driver::GetPins(UINT32 spi_mod, GPIO_PIN &msk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    msk = (GPIO_PIN)SPI_PINS[spi_mod*3];
    miso = (GPIO_PIN)SPI_PINS[spi_mod*3+2];
    mosi = (GPIO_PIN)SPI_PINS[spi_mod*3+1];
}
