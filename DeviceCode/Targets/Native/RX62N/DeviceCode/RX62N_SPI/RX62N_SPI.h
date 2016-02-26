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

#ifndef RX62N_SPI_H_
#define RX62N_SPI_H_

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

struct RX62N_SPI_Driver
{
    static const int c_Max_SPI = 4;

    BOOL m_initialized[c_Max_SPI];
    BOOL m_Enabled[c_Max_SPI];
    int m_Saved_CS[c_Max_SPI];

    static BOOL Initialize();
    static void Uninitialize();
    static void GetPins(UINT32 spi_mod, GPIO_PIN& msk, GPIO_PIN& miso, GPIO_PIN& mosi);
    static BOOL nWrite16_nRead16(const SPI_CONFIGURATION& Configuration, UINT16* Write16, INT32 WriteCount, UINT16* Read16, INT32 ReadCount, INT32 ReadStartOffset);
    static BOOL nWrite8_nRead8(const SPI_CONFIGURATION& Configuration, UINT8*  Write8 , INT32 WriteCount, UINT8*  Read8 , INT32 ReadCount, INT32 ReadStartOffset);
    static BOOL Xaction_Start(const SPI_CONFIGURATION& Configuration);
    static BOOL Xaction_Stop(const SPI_CONFIGURATION& Configuration);
    static BOOL Xaction_nWrite16_nRead16(SPI_XACTION_16& Transaction);
    static BOOL Xaction_nWrite8_nRead8(SPI_XACTION_8&  Transaction);
    static void SetSPIBits(UINT32 spi_mod, UINT32 spi_bits);
    static void SetSPIClk(UINT32 spi_mod, UINT32 spi_clk);
    static void SetSPIMod(UINT32 spi_mod);
    static void ResetSPIMod(UINT32 spi_mod);

private:
    static void ISR(void* Param);
};

#endif /* RX62N_SPI_H_ */
