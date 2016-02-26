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

#ifndef RX63N_SPI_H_
#define RX63N_SPI_H_

#include <tinyhal.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" int _dbg_printf(int ch, const char* format, ...);
#else
int _dbg_printf(int ch, const char* format, ...);
#endif

#undef CH_COM
#undef CH_LCD
#undef CH_BOTH
#define CH_COM  1
#define CH_LCD  2
#define CH_BOTH (CH_COM | CH_LCD)

#ifdef SPI_DEBUG
#define SPI_ENTER(ch)               _dbg_printf(ch, "%s ENTER\n", __FUNCTION__)
#define SPI_EXIT(ch)                _dbg_printf(ch, "%s EXIT \n", __FUNCTION__)
#define SPI_TRACE1(ch, a)           _dbg_printf(ch, (const char *)a)
#define SPI_TRACE2(ch, a, b)        _dbg_printf(ch, (const char *)a, b)
#define SPI_TRACE3(ch, a, b, c)     _dbg_printf(ch, (const char *)a, b, c)
#else
#define SPI_ENTER(ch)
#define SPI_EXIT(ch)
#define SPI_TRACE1(ch, a)
#define SPI_TRACE2(ch, a, b)
#define SPI_TRACE3(ch, a, b, c)
#endif

#define BCLK        48000UL     // 48MHz: SPI input clock default
//#define CLK_FAST    24000UL     // 24MHz
//#define CLK_SLOW    400UL       // 400KHz

struct st_ir {
    unsigned char IR:1;
    unsigned char :7;
};
typedef volatile struct st_ir *vp_ir;
struct st_ier {
    unsigned char BYTE;
};
typedef volatile struct st_ier *vp_ier;
struct st_ipr {
    unsigned char IPR:4;
    unsigned char :4;
};
typedef volatile struct st_ipr *vp_ipr;
typedef volatile struct st_rspi *vp_rspi;

struct RX63N_SPI_Driver
{
    static const int c_Max_SPI = 3;
    BOOL m_Enabled[c_Max_SPI];
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

#endif /* RX63N_SPI_H_ */
