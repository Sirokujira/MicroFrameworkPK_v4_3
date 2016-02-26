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
#include "..\RX62N.h"

#define    DEBUG_SPI_HAL

#if defined(DEBUG_SPI_HAL)
#define SPI_DEBUG_HAL_PRINTF(x)  hal_printf(x);
#else
#define SPI_DEBUG_HAL_PRINTF(x)
#endif

#if defined(DEBUG_SPI_LCD)
#define SPI_DEBUG_LCD_PRINTF(x)  lcd_printf(x);
#else 
#define SPI_DEBUG_LCD_PRINTF(x)
#endif 

#define BCLK        48000UL     // 48MHz: SPI input clock default
#define CLK_FAST    24000UL     // 24MHz
#define CLK_SLOW    400UL       // 400KHz
#define DIV_F       (BCLK/CLK_FAST/2-1)
#define DIV_S       (BCLK/CLK_SLOW/2-1)

RX62N_SPI_Driver g_RX62N_SPI_Driver;

void RX62N_SPI_Driver::SetSPIBits(UINT32 spi_mod, UINT32 spi_bits)
{
    if (spi_mod == 0 || spi_mod == 1) {
        if (spi_bits)
            RSPI0.SPCMD0.WORD = 0xEF80; // Command Reg: SPI mode: MODE0, 16bit, MSB first
        else
            RSPI0.SPCMD0.WORD = 0xE780; // Command Reg: SPI mode: MODE0, 8bit, MSB first
    } else {
        if (spi_bits)
            RSPI1.SPCMD0.WORD = 0xEF80; // Command Reg: SPI mode: MODE0, 16bit, MSB first
        else
            RSPI1.SPCMD0.WORD = 0xE780; // Command Reg: SPI mode: MODE0, 8bit, MSB first
    }
}

void RX62N_SPI_Driver::SetSPIClk(UINT32 spi_mod, UINT32 spi_clk)
{
    if (spi_clk == 0)
        return;
    if (spi_mod == 0 || spi_mod == 1) {
        RSPI0.SPCR.BIT.SPE = 0;
        RSPI0.SPBR.BYTE=BCLK/2/spi_clk-1;
        RSPI0.SPCR.BIT.SPE = 1;
    } else {
        RSPI1.SPCR.BIT.SPE = 0;
        RSPI1.SPBR.BYTE=BCLK/2/spi_clk-1;
        RSPI1.SPCR.BIT.SPE = 1;
    }
}

void RX62N_SPI_Driver::SetSPIMod(UINT32 spi_mod)
{
    switch (spi_mod)
    {
    case 0:
        PORTC.ICR.BIT.B7 = 1;
        IOPORT.PFGSPI.BYTE = 0x0E;
        MSTP_RSPI0 = 0;
        break;
    case 1:
        PORTA.ICR.BIT.B7 = 1;
        IOPORT.PFGSPI.BYTE = 0x0F;
        MSTP_RSPI0 = 0;
        break;
    case 2:
        PORT3.ICR.BIT.B0 = 1;
        IOPORT.PFHSPI.BYTE = 0x0E;
        MSTP_RSPI1 = 0;
        break;
    case 3:
        PORTE.ICR.BIT.B7 = 1;
        IOPORT.PFHSPI.BYTE = 0x0F;
        MSTP_RSPI1 = 0;
        break;
    }
    RSPI0.SPCR.BYTE = 0;        // Stop SPI
    RSPI0.SPPCR.BYTE = 0;       // Fixed idle value, disable loop-back mode
    RSPI0.SPSCR.BYTE = 0;       // Disable sequence control
    RSPI0.SPDCR.BYTE = 0x20;    // SPLW=1
    RSPI0.SPCMD0.WORD = 0x0700; // LSBF=0, SPB=7, BRDV=0, CPOL=0, CPHA=0
    RSPI0.SPBR.BYTE = BCLK/2/CLK_SLOW - 1;  //Bit rate
    RSPI0.SPCR.BYTE = 0x49;     // Start SPI in master mode
}

void RX62N_SPI_Driver::ResetSPIMod(UINT32 spi_mod)
{
    switch (spi_mod)
    {
    case 0:
        PORTC.ICR.BIT.B7 = 0;
        IOPORT.PFGSPI.BYTE = 0x00;
        MSTP_RSPI0 = 1;
        break;
    case 1:
        PORTA.ICR.BIT.B7 = 0;
        IOPORT.PFGSPI.BYTE = 0x00;
        MSTP_RSPI0 = 1;
        break;
    case 2:
        PORT3.ICR.BIT.B0 = 0;
        IOPORT.PFHSPI.BYTE = 0x00;
        MSTP_RSPI1 = 1;
        break;
    case 3:
        PORTE.ICR.BIT.B7 = 0;
        IOPORT.PFHSPI.BYTE = 0x00;
        MSTP_RSPI1 = 1;
        break;
    }
}

BOOL RX62N_SPI_Driver::Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    for (int i = 0; i < RX62N_SPI_Driver::c_Max_SPI; i++) {
        if (g_RX62N_SPI_Driver.m_initialized[i] == FALSE) {
            g_RX62N_SPI_Driver.m_initialized[i] = TRUE;
        }
    }
    return TRUE;
}

void RX62N_SPI_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    for (int i = 0; i < RX62N_SPI_Driver::c_Max_SPI; i++) {
        if (g_RX62N_SPI_Driver.m_initialized[i] == TRUE) {
            g_RX62N_SPI_Driver.m_initialized[i] = FALSE;
        }
    }
}

// Dummy ISR()
// Not used.
void RX62N_SPI_Driver::ISR(void *Param)
{
    SPI_DEBUG_HAL_PRINTF("SPI:error ISR\r\n");
    ASSERT(0);
}

BOOL RX62N_SPI_Driver::Xaction_Start(const SPI_CONFIGURATION &Configuration)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    int spi_mod = Configuration.SPI_mod;
    if (spi_mod < 0 || spi_mod >=  RX62N_SPI_Driver::c_Max_SPI) {
        return false;
    }
    if (!g_RX62N_SPI_Driver.m_Enabled[spi_mod]) {
        g_RX62N_SPI_Driver.m_Enabled[spi_mod] = TRUE;
        SetSPIMod(spi_mod);
        SetSPIClk(spi_mod, Configuration.Clock_RateKHz);
        SetSPIBits(spi_mod, Configuration.MD_16bits);
        CPU_GPIO_EnableOutputPin(Configuration.DeviceCS, Configuration.CS_Active);
        if (Configuration.CS_Setup_uSecs) {
            HAL_Time_Sleep_MicroSeconds_InterruptEnabled(Configuration.CS_Setup_uSecs);
        }
    } else {
        SPI_DEBUG_HAL_PRINTF("SPI: Collision at Xaction_Start\r\n");
        HARD_BREAKPOINT();
        return FALSE;
    }
    return TRUE;
}

BOOL RX62N_SPI_Driver::Xaction_Stop(const SPI_CONFIGURATION &Configuration)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    int spi_mod = Configuration.SPI_mod;
    if (spi_mod < 0 || spi_mod >=  RX62N_SPI_Driver::c_Max_SPI) {
        return false;
    }
    if (g_RX62N_SPI_Driver.m_Enabled[spi_mod]) {
        ResetSPIMod(spi_mod);
        if(Configuration.CS_Hold_uSecs) {
            HAL_Time_Sleep_MicroSeconds_InterruptEnabled(Configuration.CS_Hold_uSecs);
        }
        CPU_GPIO_SetPinState(Configuration.DeviceCS, !Configuration.CS_Active);
        g_RX62N_SPI_Driver.m_Enabled[spi_mod] = FALSE;
    } else {
        SPI_DEBUG_HAL_PRINTF("SPI: Collision at Xaction_Stop\r\n");
        HARD_BREAKPOINT();
        return FALSE;
    }
    return TRUE;
}

BOOL RX62N_SPI_Driver::Xaction_nWrite16_nRead16(SPI_XACTION_16 &Transaction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    UINT16 Data16;

    if (!g_RX62N_SPI_Driver.m_Enabled[Transaction.SPI_mod]) {
        SPI_DEBUG_HAL_PRINTF("SPI: disabled at Xaction_nWrite16_nRead16\r\n");
        ASSERT(FALSE);
        return FALSE;
    }

    UINT16  *Write16    = Transaction.Write16;
    INT32   WriteCount  = Transaction.WriteCount;
    UINT16  *Read16     = Transaction.Read16;
    INT32   ReadCount   = Transaction.ReadCount;
    INT32   ReadStartOffset = Transaction.ReadStartOffset;
    INT32   ReadTotal   = 0;

    if (WriteCount <= 0) {
        return FALSE;
    }
    if (Write16 == NULL) {
        return FALSE;
    }
    if ((ReadCount > 0) && (Read16 == NULL)) {
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
        while (RSPI0.SPSR.BIT.SPTEF == 0);
        RSPI0.SPDR.LONG = *Write16;
        while (RSPI0.SPSR.BIT.SPRF == 0);
        Data16 = RSPI0.SPDR.LONG;
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

BOOL RX62N_SPI_Driver::Xaction_nWrite8_nRead8(SPI_XACTION_8 &Transaction)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    UINT32 Data32;

    if (!g_RX62N_SPI_Driver.m_Enabled[Transaction.SPI_mod]) {
        SPI_DEBUG_HAL_PRINTF("SPI: disabled at Xaction_nWrite8_nRead8\r\n");
        return FALSE;
    }

    UINT8 *Write8 = Transaction.Write8;
    INT32 WriteCount = Transaction.WriteCount;
    UINT8 *Read8 = Transaction.Read8;
    INT32 ReadCount = Transaction.ReadCount;
    INT32 ReadStartOffset = Transaction.ReadStartOffset;
    INT32 ReadTotal = 0;
    if (WriteCount <= 0) {
        debug_printf("WriteCount <= 0\r\n");
        return FALSE;
    }
    if (Write8 == NULL) {
        debug_printf("Write8 == NULL\r\n");
        return FALSE;
    }
    if ((ReadCount > 0) && (Read8 == NULL)) {
        debug_printf("(ReadCount > 0) && (Read8 == NULL)\r\n");
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
        while (RSPI0.SPSR.BIT.SPTEF == 0);
        RSPI0.SPDR.LONG = *Write8;
        while (RSPI0.SPSR.BIT.SPRF == 0);
        Data32 = RSPI0.SPDR.LONG;
        if (WriteCount) {
            WriteCount--;
            Write8++;
        }
        ReadTotal--;
        if ((ReadTotal>=0) && (ReadTotal < ReadCount)) {
            Read8[0] = Data32;
            Read8++;
        }
    }
    return TRUE;
}

BOOL RX62N_SPI_Driver::nWrite16_nRead16(const SPI_CONFIGURATION &Configuration, UINT16 *Write16, INT32 WriteCount, UINT16 *Read16, INT32 ReadCount, INT32 ReadStartOffset)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    if (Configuration.SPI_mod >= RX62N_SPI_Driver::c_Max_SPI) {
        SPI_DEBUG_HAL_PRINTF("SPI: out of range at nWrite16_nRead16\r\n");
        HARD_BREAKPOINT();
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

BOOL RX62N_SPI_Driver::nWrite8_nRead8(const SPI_CONFIGURATION &Configuration, UINT8 *Write8, INT32 WriteCount, UINT8 *Read8, INT32 ReadCount, INT32 ReadStartOffset)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();

    if (Configuration.SPI_mod >= RX62N_SPI_Driver::c_Max_SPI) {
        SPI_DEBUG_LCD_PRINTF("SPI: out of range at nWrite8_nRead8\r\n");
        SPI_DEBUG_HAL_PRINTF("SPI: out of range at nWrite8_nRead8\r\n");
        HARD_BREAKPOINT();
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

// SPI MOD
// RSPI_CH == 00    RSPIA-A
//      RSPCKA-A    PC5
//      MOSIA-A     PC6
//      MISOA-A     PC7
//      SSLA0-A     PC4
// RSPI_CH == 01    RSPIA-B
//      RSPCKA-B    PA5
//      MOSIA-B     PA6
//      MISOA-B     PA7
//      SSLA0-B     PA4
// RSPI_CH == 10    RSPIB-A
//      RSPCKB-A    P27
//      MOSIB-A     P26
//      MISOB-A     P30
//      SSLB0-A     P31
// RSPI_CH == 11    RSPIB-B
//      RSPCKB-B    PE5
//      MOSIB-B     PE6
//      MISOB-B     PE7
//      SSLB0-B     PE4

UINT8 SPI_PINS[12] = {
    RX62N_GPIO::PC5,    // A-A
    RX62N_GPIO::PC6,    // A-A
    RX62N_GPIO::PC7,    // A-A
    RX62N_GPIO::PA5,    // A-B
    RX62N_GPIO::PA6,    // A-B
    RX62N_GPIO::PA7,    // A-B
    RX62N_GPIO::P27,    // B-A
    RX62N_GPIO::P26,    // B-A
    RX62N_GPIO::P30,    // B-A
    RX62N_GPIO::PE5,    // B-B
    RX62N_GPIO::PE6,    // B-B
    RX62N_GPIO::PE7     // B-B
};

// Returns the SPI clock, MISO and MOSI pin numbers for a specified SPI module.
// spi_mod:    Specifies the SPI module on entry.
// msk:     The number of the clock pin (SCLK) is returned here.
// miso:    The number of the Master Input Subordinate Output (MISO) pin is returned here.
// mosi:    The number of the Master Output Subordinate Input (MOSI) pin is returned here.
void RX62N_SPI_Driver::GetPins(UINT32 spi_mod, GPIO_PIN &msk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    msk = (GPIO_PIN)SPI_PINS[spi_mod*3];
    miso = (GPIO_PIN)SPI_PINS[spi_mod*3+2];
    mosi = (GPIO_PIN)SPI_PINS[spi_mod*3+1];
}
