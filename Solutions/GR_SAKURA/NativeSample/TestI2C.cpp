////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestI2C.cpp
 *
 *  Created on: 2012/06/15
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_I2C
void memory_dump(unsigned long addr, int len);

#define I2C_STATIC_BUF
#define I2C_FRAM_CH    1
//#define I2C_FRAM_SIZE 0x800
#define I2C_FRAM_SIZE 0x200
#define I2C_buf_SIZE 0x100
#define I2C_FRAM_ADDR    0x50
#define I2C_FRAM_SPEED    1000
#define I2C_WRITE    0
#define I2C_READ    1
#define I2C_TIMEOUT    1000000

#ifdef I2C_STATIC_BUF
UINT8 qbuf[I2C_buf_SIZE+1];
#endif

BOOL FRAMWriteSub(UINT32 fram_addr, UINT8 *src, UINT32 len, BOOL fErase)
{
    struct I2C_USER_CONFIGURATION I2C_config;
    struct I2C_HAL_XACTION_UNIT xActionUnits[1];
    struct I2C_HAL_XACTION_UNIT *pxActionUnits[1];
    I2C_HAL_XACTION xAction;
    UINT8 buf[I2C_buf_SIZE+1];
#ifndef I2C_STATIC_BUF
    UINT8 qbuf[I2C_buf_SIZE+1];
#endif
    UINT8 status;
    UINT32 size;

    debug_printf("FRAMWrite\r\n");
    I2C_config.ClockRate = I2C_FRAM_SPEED;
    while (len > 0) {
        UINT32 timeout = I2C_TIMEOUT;
        I2C_config.Address = (UINT16)(I2C_FRAM_ADDR + (fram_addr >> 8));
        if (len < I2C_buf_SIZE)
            size = len;
        else
            size = I2C_buf_SIZE;
        buf[0] = (UINT8)fram_addr;
        debug_printf("addr=%04X\r\n", fram_addr);
        if (fErase)
            memset((void *)(buf+1), 0, size);
        else
            memcpy((void *)(buf+1), (void *)src, size);
        pxActionUnits[0] = &xActionUnits[0];
        I2C_InitializeTransactionUnit((I2C_HAL_XACTION_UNIT *)&xActionUnits[0], (I2C_WORD *)&buf, (I2C_WORD *)&qbuf, (size_t)(size+1), I2C_WRITE);
        I2C_InitializeTransaction(&xAction, I2C_config, (I2C_HAL_XACTION_UNIT **)&pxActionUnits[0], 1);
        I2C_Enqueue(&xAction);
        while (timeout-- > 0) {
            status = I2C_XAction_GetState(&xAction);
            if (status == I2C_HAL_XACTION::c_Status_Completed)
                break;
        }
        debug_printf("I2C state = %d\r\n", status);
        if (status != I2C_HAL_XACTION::c_Status_Completed)
            return FALSE;
        memory_dump((unsigned long)&qbuf[1], size);
        len -= size;
        fram_addr += size;
        src += size;
    }
    return TRUE;
}

BOOL FRAMReadSub(UINT32 fram_addr, UINT8 *dst, UINT32 len, BOOL fEraseCheck)
{
    struct I2C_USER_CONFIGURATION I2C_config;
    struct I2C_HAL_XACTION_UNIT xActionUnits[2];
    struct I2C_HAL_XACTION_UNIT *pxActionUnits[2];
    I2C_HAL_XACTION xAction;
    UINT8 buf;
#ifndef I2C_STATIC_BUF
    UINT8 qbuf[I2C_buf_SIZE+1];
#endif
    UINT8 status;
    UINT32 size;

    debug_printf("FRAMRead\r\n");
    I2C_config.ClockRate = I2C_FRAM_SPEED;
    while (len > 0) {
        UINT32 timeout = I2C_TIMEOUT;
        I2C_config.Address = (UINT16)(I2C_FRAM_ADDR + (fram_addr >> 8));
        if (len < I2C_buf_SIZE)
            size = len;
        else
            size = I2C_buf_SIZE;
        buf = (UINT8)fram_addr;
        debug_printf("addr=%04X\r\n", fram_addr);
        pxActionUnits[0] = &xActionUnits[0];
        pxActionUnits[1] = &xActionUnits[1];
        I2C_InitializeTransactionUnit((I2C_HAL_XACTION_UNIT *)&xActionUnits[0], (I2C_WORD *)&buf, (I2C_WORD *)&qbuf, 1, I2C_WRITE);
        if (fEraseCheck)
            // Erase Check
            I2C_InitializeTransactionUnit((I2C_HAL_XACTION_UNIT *)&xActionUnits[1], (I2C_WORD *)0, (I2C_WORD *)&qbuf, size, I2C_READ);
        else
            // Read
            I2C_InitializeTransactionUnit((I2C_HAL_XACTION_UNIT *)&xActionUnits[1], (I2C_WORD *)0, (I2C_WORD *)dst, size, I2C_READ);
        I2C_InitializeTransaction(&xAction, I2C_config, (I2C_HAL_XACTION_UNIT **)&pxActionUnits[0], 2);
        I2C_Enqueue(&xAction);
        while (timeout-- > 0) {
            status = I2C_XAction_GetState(&xAction);
            if (status == I2C_HAL_XACTION::c_Status_Completed)
                break;
        }
        debug_printf("I2C state = %d\r\n", status);
        if (status != I2C_HAL_XACTION::c_Status_Completed)
            return FALSE;
        if (fEraseCheck) {
            for (UINT32 i = 0; i < size; i++)
                if (qbuf[i] != 0) {
                    return FALSE;
                }
            memory_dump((unsigned long)qbuf, size);
        } else
            memory_dump((unsigned long)dst, size);
        len -= size;
        fram_addr += size;
        dst += size;
    }
    return TRUE;
}

BOOL FRAMWrite(UINT32 fram_addr, UINT8 *src, UINT32 len)
{
    return FRAMWriteSub(fram_addr, src, len, FALSE);
}

BOOL FRAMErase(UINT32 fram_addr, UINT32 len)
{
    return FRAMWriteSub(fram_addr, 0, len, TRUE);
}

BOOL FRAMRead(UINT32 fram_addr, UINT8 *dst, UINT32 len)
{
    return FRAMReadSub(fram_addr, dst, len, FALSE);
}

BOOL FRAMIsErased(UINT32 fram_addr, UINT32 len)
{
    return FRAMReadSub(fram_addr, 0, len, TRUE);
}

UINT8 w_data[I2C_FRAM_SIZE];
UINT8 r_data[I2C_FRAM_SIZE];

void TestI2C()
{
    UINT32 i = 0;
    for (i = 0; i < I2C_FRAM_SIZE; i++) {
        w_data[i] = i;
        r_data[i] = 0;
    }
#if 1
    FRAMErase((UINT32)0, (UINT32)I2C_FRAM_SIZE);
    if (FRAMIsErased((UINT32)0, (UINT32)I2C_FRAM_SIZE)) {
        debug_printf("TestI2C Erase OK\r\n");
    } else {
        debug_printf("TestI2C Erase NG\r\n");
    }
#endif
#if 1
    FRAMWrite((UINT32)0, (UINT8 *)&w_data, (UINT32)I2C_FRAM_SIZE);
    FRAMRead((UINT32)0, (UINT8 *)&r_data, (UINT32)I2C_FRAM_SIZE);
    if (memcmp(w_data, r_data, I2C_FRAM_SIZE) == 0) {
        debug_printf("TestI2C R/W OK\r\n");
    } else {
        debug_printf("TestI2C R/W NG\r\n");
    }
    memory_dump((unsigned long)w_data, I2C_FRAM_SIZE);
    memory_dump((unsigned long)r_data, I2C_FRAM_SIZE);
#endif
#if 0
    FRAMWrite((UINT32)0, (UINT8 *)&w_data, (UINT32)I2C_FRAM_SIZE);
    FRAMRead((UINT32)0, (UINT8 *)&r_data, (UINT32)I2C_FRAM_SIZE);
    if (memcmp(w_data, r_data, I2C_FRAM_SIZE) == 0) {
        debug_printf("TestI2C R/W OK\r\n");
    } else {
        debug_printf("TestI2C R/W NG\r\n");
    }
    memory_dump((unsigned long)w_data, I2C_FRAM_SIZE);
    memory_dump((unsigned long)r_data, I2C_FRAM_SIZE);
#endif
}
#endif
