////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestFlash.cpp
 *
 *  Created on: 2011/12/24
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_FLASH
void wait(volatile UINT64 count);
void memory_dump(unsigned long addr, int len);

extern struct BLOCK_CONFIG  g_FM3_BS_Config;
extern struct IBlockStorageDevice g_FM3_Flash_DeviceTable;

#define FLASH_TEST_DATA_SIZE    0x200
#define FLASH_TEST_ADDR         0x00060000
#define FLASH_SECTOR_SIZE       0x20000
#define FLASH_READWRITE_FLAG    0

BYTE FlashTestData[FLASH_TEST_DATA_SIZE];

void dispFlag(int flag)
{
    if (flag)
        debug_printf("OK\r\n");
    else
        debug_printf("NG\r\n");
}

void TestFlashInitData()
{
    int i;
    for (i = 0; i < FLASH_TEST_DATA_SIZE; i++)
        FlashTestData[i] = (BYTE)(i & 0xff);
}

void TestFlashErase(UINT32 addr)
{
    int flag = true;
    flag = g_FM3_Flash_DeviceTable.EraseBlock(&g_FM3_BS_Config, addr);
    debug_printf("TestFlashErase addr=%x ", addr);
    dispFlag(flag);
    wait(1000000L);
    memory_dump(addr, 256);
    wait(1000000L);
}

void TestFlashWrite(UINT32 addr, UINT32 size)
{
    int flag;
    flag = g_FM3_Flash_DeviceTable.Write(&g_FM3_BS_Config, addr, size, FlashTestData, FLASH_READWRITE_FLAG);
    debug_printf("TestFlashWrite addr=%x size=%x ", addr, size);
    dispFlag(flag);
    wait(1000000L);
    memory_dump(addr & 0xffffff00, (size + 255) & 0xffffff00);
    wait(1000000L);
}

void TestFlash(void)
{
//  g_FM3_FLASH_DeviceTable.InitializeDevice(pBLOCK_CONFIG_NOR);
    TestFlashInitData();
    TestFlashErase(FLASH_TEST_ADDR);
    TestFlashWrite(FLASH_TEST_ADDR+0x10, 0x20);
//  TestFlashErase(FLASH_TEST_ADDR);
    TestFlashWrite(FLASH_TEST_ADDR+0x30, 0x20);
//  TestFlashErase(FLASH_TEST_ADDR);
    TestFlashWrite(FLASH_TEST_ADDR+0x50, 0x120);
}
#endif
