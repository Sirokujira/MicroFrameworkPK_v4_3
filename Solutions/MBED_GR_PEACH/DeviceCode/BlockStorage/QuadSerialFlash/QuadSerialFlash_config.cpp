/*
 *    Copyright (C) 2010 Renesas Electronics America Inc. All rights reserved.
 */

////////////////////////////////////////////////////////////////////////////
// Portions Copyright (c) 2010-2011 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\MBED\DeviceCode\MBED_Blockstorage\RZA1_QuadSerialFlash\RZA1_QuadSerialFlash.h"

//--//

#define SERIALFLASH_MANUFACTURER_CODE               0x20
#define SERIALFLASH_DEVICE_CODE                     0x20
#define SERIALFLASH_BLOCK_COUNT                     16
#define SERIALFLASH_BLOCK_ERASE_TYPICAL_TIME_USEC   700000
#define SERIALFLASH_SECTOR_WRITE_TYPICAL_TIME_USEC  210
#define SERIALFLASH_BLOCK_ERASE_MAX_TIME_USEC       12000000
#define SERIALFLASH_SECTOR_WRITE_MAX_TIME_USEC      220

//--//

#define SERIALFLASH__CHIP_SELECT      0
#define SERIALFLASH__WAIT_STATES      0 
#define SERIALFLASH__RELEASE_COUNTS   0
#define SERIALFLASH__BIT_WIDTH        1 
#define SERIALFLASH__BASE_ADDRESS     0x18000000
#define SERIALFLASH__SIZE_IN_BYTES    8*1024*1024
#define SERIALFLASH__WP_GPIO_PIN      GPIO_PIN_NONE
#define SERIALFLASH__WP_ACTIVE        FALSE

// BlockDeviceInformation

#define SERIALFLASH__IS_REMOVABLE     FALSE
#define SERIALFLASH__SUPPORTS_XIP     FALSE
#define SERIALFLASH__WRITE_PROTECTED  FALSE

#define SERIALFLASH__NUM_REGIONS      1
#define BYTES_PER_SECTOR              0x10000
#define REGION_NUM_OF_BLOCKS          128
#define REGION_BYTES_PER_BLK          0x10000
#define REGION_XIP_ADDRESS            SERIALFLASH__BASE_ADDRESS
#define REGION_SECTORS_PER_BLOCK      REGION_BYTES_PER_BLK/BYTES_PER_SECTOR

//////////////////////////////////////////////////

#define REGION_SECTOR_START        0

const BlockRange g_SerialFlash_BlockRange[] =
{
    { BlockRange::BLOCKTYPE_BOOTSTRAP   ,    0,   1 },       // TinyBooter
    { BlockRange::BLOCKTYPE_CODE        ,    2,  30 },       // ER_FLASH  (18020000-180FEFFF)
    { BlockRange::BLOCKTYPE_CONFIG      ,   31,  31 },       // ER_Config (180F0000-180FFFFF)
    { BlockRange::BLOCKTYPE_DEPLOYMENT  ,   32,  47 },       // DEPLOYMENT(18200000-182FFFFF)
};

//--//

const BlockRegionInfo  g_SerialFlash_BlkRegion[SERIALFLASH__NUM_REGIONS] = 
{
    {
        REGION_XIP_ADDRESS,            // ByteAddress  Start;              // Starting Sector address
        REGION_NUM_OF_BLOCKS,          // UINT32       NumBlocks;          // total number of blocks in this region
        REGION_BYTES_PER_BLK,          // UINT32       BytesPerBlock;      // Total number of bytes per block (MUST be SectorsPerBlock * DataBytesPerSector)

        ARRAYSIZE_CONST_EXPR(g_SerialFlash_BlockRange),
        g_SerialFlash_BlockRange
    },
};

//--//


BlockDeviceInfo g_SerialFlash_DeviceInfo=
{
    {
        SERIALFLASH__IS_REMOVABLE,          // BOOL Removable;
        SERIALFLASH__SUPPORTS_XIP,          // BOOL SupportsXIP;
        SERIALFLASH__WRITE_PROTECTED        // BOOL WRiteProtected
    },
    SERIALFLASH_SECTOR_WRITE_MAX_TIME_USEC,        // UINT32 Duration_Max_WordWrite_uSec;
    SERIALFLASH_BLOCK_ERASE_MAX_TIME_USEC,         // UINT32 Duration_Max_SectorErase_uSec;
    BYTES_PER_SECTOR,                        // UINT32 BytesPerSector; // Bytes Per Sector

    SERIALFLASH__SIZE_IN_BYTES,                       // UINT32 Size;

    SERIALFLASH__NUM_REGIONS,               // UINT32 NumRegions;
    g_SerialFlash_BlkRegion,            // const BlockRegionInfo* pRegions;
};


struct MEMORY_MAPPED_SERIAL_BLOCK_CONFIG g_SerialFlash_BL_Config =
{
    { // BLOCK_CONFIG
        {
            SERIALFLASH__WP_GPIO_PIN,      // GPIO_PIN             Pin;
            SERIALFLASH__WP_ACTIVE,        // BOOL                 ActiveState;
        },

        &g_SerialFlash_DeviceInfo,    // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        SERIALFLASH__CHIP_SELECT,          // UINT8                CPU_MEMORY_CONFIG::ChipSelect;
        TRUE,                               // UINT8                CPU_MEMORY_CONFIG::ReadOnly;
        SERIALFLASH__WAIT_STATES,          // UINT32               CPU_MEMORY_CONFIG::WaitStates;
        SERIALFLASH__RELEASE_COUNTS,       // UINT32               CPU_MEMORY_CONFIG::ReleaseCounts;
        SERIALFLASH__BIT_WIDTH,            // UINT32               CPU_MEMORY_CONFIG::BitWidth;
        SERIALFLASH__BASE_ADDRESS,         // UINT32               CPU_MEMORY_CONFIG::BaseAddress;
        SERIALFLASH__SIZE_IN_BYTES,        // UINT32               CPU_MEMORY_CONFIG::SizeInBytes;
        0,                                  // UINT8                CPU_MEMORY_CONFIG::XREADYEnable 
        0,                                  // UINT8                CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                                  // UINT8                CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                                      // UINT32               ChipProtection;
    SERIALFLASH_MANUFACTURER_CODE,                // FLASH_WORD           ManufacturerCode;
    SERIALFLASH_DEVICE_CODE,                      // FLASH_WORD           DeviceCode;
};

//--//

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_SerialFlash_Device";
#endif

struct BlockStorageDevice g_SerialFlash_Device;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 

//--//

