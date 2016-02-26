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
#include "RX63N_SPI.h"

BOOL CPU_SPI_Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::Initialize();
}

void CPU_SPI_Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    RX63N_SPI_Driver::Uninitialize();
}

BOOL CPU_SPI_nWrite16_nRead16( const SPI_CONFIGURATION& Configuration, UINT16* Write16, INT32 WriteCount, UINT16* Read16, INT32 ReadCount, INT32 ReadStartOffset )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::nWrite16_nRead16( Configuration, Write16, WriteCount, Read16, ReadCount, ReadStartOffset );
}

BOOL CPU_SPI_nWrite8_nRead8( const SPI_CONFIGURATION& Configuration, UINT8* Write8, INT32 WriteCount, UINT8* Read8, INT32 ReadCount, INT32 ReadStartOffset )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::nWrite8_nRead8( Configuration, Write8, WriteCount, Read8, ReadCount, ReadStartOffset );
}

BOOL CPU_SPI_Xaction_Start( const SPI_CONFIGURATION& Configuration )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::Xaction_Start( Configuration );
}

BOOL CPU_SPI_Xaction_Stop( const SPI_CONFIGURATION& Configuration )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::Xaction_Stop( Configuration );
}

BOOL CPU_SPI_Xaction_nWrite16_nRead16( SPI_XACTION_16& Transaction )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::Xaction_nWrite16_nRead16( Transaction );
}

BOOL CPU_SPI_Xaction_nWrite8_nRead8( SPI_XACTION_8& Transaction )
{
    NATIVE_PROFILE_HAL_PROCESSOR_SPI();
    return RX63N_SPI_Driver::Xaction_nWrite8_nRead8( Transaction );
}

UINT32 CPU_SPI_PortsCount()
{
    return RX63N_SPI_Driver::c_Max_SPI;
}

void CPU_SPI_GetPins( UINT32 spi_mod, GPIO_PIN& msk, GPIO_PIN& miso, GPIO_PIN& mosi )
{
    return RX63N_SPI_Driver::GetPins(spi_mod,msk,miso,mosi);
}

