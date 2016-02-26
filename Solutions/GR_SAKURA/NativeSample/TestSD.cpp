////////////////////////////////////////////////////////////////////////////
// Portions Copyright (c) 2011-2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestSD.c
 *
 *  Created on: 2011/12/17
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>
#define TEST_DEBUG
#include "TestDebug.h"

#ifdef TEST_SD
#include "..\..\..\..\DeviceCode\Targets\Native\RX63N\DeviceCode\RX63N.h"
#include "..\..\..\..\DeviceCode\Drivers\FS\FAT\FAT_FS.h"

extern struct BlockStorageDevice  g_SD_BS;
extern struct IBlockStorageDevice g_SD_BS_DeviceTable;
extern struct BLOCK_CONFIG        g_SD_BS_Config;

const WCHAR path[12] = {'\\', 'T', 'E', 'S', 'T', '0', '0', '.', 'M', 'P', '3', '\0'};

BOOL SDOpenFile()
{
    UINT32 handle = 0;
    FileSystemVolume *pFSVolume = NULL;

    if (BlockStorageList::AddDevice(&g_SD_BS, &g_SD_BS_DeviceTable, &g_SD_BS_Config, TRUE)) {
        FS_MountVolume("SD", 0, 0, &g_SD_BS);
        pFSVolume = FileSystemVolumeList::GetFirstVolume();
        if (pFSVolume) {
            TEST_TRACE2(CH_BOTH, "id:%d\r\n", pFSVolume->m_volumeId);
            TEST_TRACE2(CH_BOTH, "name space:%s\r\n", pFSVolume->m_nameSpace);
            TEST_TRACE2(CH_BOTH, "label:%s\r\n", pFSVolume->m_label);
            FAT_FS_Driver::Open(&pFSVolume->m_volumeId, (LPCWSTR)path, &handle);
            if (!handle) {
                TEST_TRACE1(CH_BOTH, "SD can't open\r\n");
                return FALSE;
            }
        } else {
            TEST_TRACE1(CH_BOTH, "SD Volume not found\r\n");
            return FALSE;
        }
        TEST_TRACE1(CH_BOTH, "SD File found\r\n");
        return TRUE;
    } else {
    	TEST_TRACE1(CH_BOTH, "SD can't add device\r\n");
        return FALSE;
    }
}

void TestSD(void)
{
    SDOpenFile();
}
#endif
