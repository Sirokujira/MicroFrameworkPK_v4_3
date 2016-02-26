////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Kentaro Sekimoto  All rights reserved.
////////////////////////////////////////////////////////////////////////////
/*
 * TestMemory.cpp
 *
 *  Created on: 2012/06/15
 *      Author: Kentaro Sekimoto
 */
#ifndef __GNUC__
#define __GNUC__
#endif
#include <tinyhal.h>

#ifdef TEST_MEMORY
#define ADDR_ERAM_CS0_TOP   0x60000000
void memory_dump(unsigned long addr, int len);

void memory_fill(unsigned long addr, char c, int len)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        *p++ = c;
    }
}

void memory_fill_order(unsigned long addr, int len)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        *p++ = (char)(i & 0xff);
    }
}

int memory_compare(unsigned long addr, char c, int len)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        if (*p++ != c)
            return 0;
    }
    return 1;
}

int memory_compare_order(unsigned long addr, int len, unsigned long *err_addr)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        if (*p != (char)(i & 0xff)) {
            *err_addr = (unsigned long)p;
            return 0;
        }
        p++;
    }
    return 1;
}

int memory_check(unsigned long addr, char c, int len)
{
    memory_fill(addr, c, len);
    if (!memory_compare(addr, c, len)) {
        debug_printf("memory NG\r\n");
        memory_dump(addr, len);
        return 0;
    }
    return 1;
}

int memory_check_order(unsigned long addr, int len)
{
    unsigned long err_addr;
    memory_fill_order(addr, len);
    if (!memory_compare_order(addr, len, &err_addr)) {
        debug_printf("memory NG at %08x\r\n", err_addr);
        memory_dump((err_addr & 0xffffff00), 256);
        return 0;
    }
    return 1;
}

void TestMemory(void)
{
    int i;
    int flag;
    for (i=0; i<10; i++) {
#if 1
        flag = memory_check_order(ADDR_ERAM_CS0_TOP, 0x80000);
#else
        flag = memory_check(ADDR_ERAM_CS0_TOP, 0x00, 256);
        flag &= memory_check(ADDR_ERAM_CS0_TOP+256, 0x55, 256);
        flag &= memory_check(ADDR_ERAM_CS0_TOP+512, 0xaa, 256);
        flag &= memory_check(ADDR_ERAM_CS0_TOP+768, 0xff, 256);
#endif
        if (flag)
            debug_printf("%3d memory OK\r\n", i);
        else
            debug_printf("%3d memory NG\r\n", i);
    }
}
#endif
