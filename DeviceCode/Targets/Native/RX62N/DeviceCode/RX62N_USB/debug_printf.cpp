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
//  Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <tinyhal.h>
#include "debug_printf.h"

#ifdef USB_DEBUG
static int __isdigit (register int c)
{
    return ((c >= '0') && (c <= '9'));
}

static void *__memset(void *dst, int c, unsigned int len)
{
    char *p;
    for (p = (char *)dst; len > 0; len--)
        *(p++) = c;
    return (void *)dst;
}

static int __strlen(const char *s)
{
    int len;
    for (len = 0; *s; s++, len++)
        ;
    return len;
}

#define MORETHAN10    1
#define    MAX_DIGIT    20
const char sdigit_str[] = "0123456789abcdef";
const char bdigit_str[] = "0123456789ABCDEF";

//int __isdigit(char c)
//{
//  return ('0' <= c && c <= '9');
//}

// xput_s
// put a string into a buffer and fill with spaces
// buf:         pointer to buffer
// s:           pointer to source string
// width:       width to be stored
// buf_size     size of buffer
// minusflag    if 0, then right aligned
static unsigned int xput_s(char *buf, char *s, int width, unsigned int buf_size, int minusflag)
{
    unsigned int size = 0;
    if ((!minusflag) && (width > 0)) {
        // ToDo: if buf_size < width, then crash.
        width -= __strlen(s);    // decreased by string length
        if (width > 0) {
            __memset(buf, ' ', width);
            size += width;
            buf += width;
            buf_size -= width;
        }
    }
    for (; *s != 0; s++, width--) {
        if (buf_size-- == 0)
            return -1;
        *buf++ = *s;
        size++;
    }
    for (; width > 0; width--) {
        if (buf_size-- == 0)
            return -1;
        *buf++ = ' ';
        size++;
    }
    return size;
}

// xput_c
// put a character into buffer and fill with spaces
// buf:         pointer to buffer
// c:           a character to be stored
// width:       width to be stored
// buf_size     size of buffer
static unsigned int xput_c(char *buf, char c, int width, unsigned int buf_size)
{
    unsigned int size = 0;
    if (buf_size > 0) {
        if (width > 0) {
            width--;    // decrease for one character
            if (width > (int)buf_size)
                width = (int)buf_size;
            __memset(buf, ' ', width);
            buf += width;
            size += width;
        }
        *buf++ = c;
        size++;
    }
    return size;
}

// xput_n
// put a decimal number into buffer
// buf:         pointer to buffer
// fmt:         pointer to format string
// vv:          value to be stored
// width:       width to be stored
// zeroflag     flag for zero filling
// buf_size     size of buffer
#if X86
static unsigned int xput_n(char *buf, char *fmt, unsigned long long vv, int width, unsigned int zeroflag, unsigned int buf_size)
#else
static unsigned int xput_n(char *buf, char *fmt, unsigned long vv, int width, unsigned int zeroflag, unsigned int buf_size)
#endif
{
    long long vvd;
    char tmp[MAX_DIGIT];
    char *p, *t, z;
    unsigned int base;
    unsigned int cap = 0;
    unsigned int size = 0;
    unsigned int negativeflag = 0;

    p = tmp;
    if (zeroflag)
        z = '0';
    else
        z = ' ';
    switch (*fmt) {
    case 'b':
        base = 2;
        break;
    case 'd':
        base = 10;
        vvd = (long long)vv;
        if (vvd < 0) {
            vv = -vvd;
            negativeflag = 1;
        }
        break;
    case 'u':
        base = 10;
        break;
    case 'o':
        base = 8;
        break;
    case 'x':
        base = 16;
        break;
    case 'D':
        base = 10;
        cap = 1;
        break;
    case 'O':
        base = 8;
        cap = 1;
        break;
    case 'X':
        base = 16;
        cap = 1;
        break;
    default:
        return size;
    }
    t = (char *)sdigit_str;
    if (cap)
        t = (char *)bdigit_str;
    do {
        width--;
        *p++ = t[vv % base];
        vv /= base;
    } while (vv != 0);
    if (!zeroflag && negativeflag) {
        width--;
        *p++ = '-';
    }
    if (zeroflag && negativeflag) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = '-';
        size++;
        width--;
    }
    for (; width > 0; width--) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = z;
        size++;
    }
    while (p > tmp) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = *--p;
        size++;
    }
    return size;
}

#ifdef SUPPOR_FLOAT
// xput_f
// put a float number into buffer
// buf:         pointer to buffer
// fmt:         pointer to format string
// f:           float value to be stored
// width:       width for before dicimal point
// adp:         width for after dicimal point
// zeroflag     flag for zero filling
// buf_size     size of buffer
static int xput_f(char *buf, char *fmt, double f, int width, int adp, int zeroflag, unsigned int buf_size)
{
    char tmp[MAX_DIGIT];

    long long l;
    long long base;
    char z;
    int i;
    char *p, *t;
    int size = 0;
    int negativeflag = 0;

    if (zeroflag)
        z = '0';
    else
        z = ' ';
    base = 10L;
    if (f < 0) {
        f = -f;
        negativeflag = 1;
    }
    for (i = 0; i < adp; i++)
        f *= 10.0;
    l = (long long)f;
    p = tmp;
    t = (char *)sdigit_str;
    do {
        if (adp == 0)
            *p++ = '.';
        width--;
        adp--;
        *p++ = t[l % base];
        l /= base;
    } while (l != 0);
    if (!zeroflag && negativeflag) {
        width--;
        *p++ = '-';
    }
    if (zeroflag && negativeflag) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = '-';
        size++;
        width--;
    }
    for (; width > 0; width--) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = z;
        size++;
    }
    while (p > tmp) {
        if (buf_size == 0)
            return -1;
        buf_size--;
        *buf++ = *--p;
        size++;
    }
    return size;
}
#endif

// vxsnprintf
static int tvsnprintf(char *buf, int buf_size, char *fmt, va_list args)
{
    char p;
    int n, m;
    unsigned long v;
#ifdef X86
    unsigned long long vv;
#else
    unsigned long vv;
#endif
    int size;
    int len = 0;
    int zeroflag = 0;
    int minusflag = 0;
    int widthflag = 0;
    double f;

    for (; *fmt != '\0'; fmt++) {
        p = *fmt;
        if (p == '%') {
            size = 0;
            fmt++;
            p = *fmt;
            if (p == '%') {
                goto vxsnprintf_next;
            } else if (p == '*') {
                widthflag = 1;
                fmt++;
                p = *fmt;
                n = (unsigned long)va_arg(args, long);
            } else if (p == '-') {
                minusflag = 1;
                fmt++;
                p = *fmt;
            } else if (p == '0') {
                zeroflag = 1;
                fmt++;
                p = *fmt;
            }
            if (__isdigit(p)) {
                n = *fmt++ - '0';
#if MORETHAN10
                while (__isdigit(*fmt)) n = n * 10 + (*fmt++ - '0');
#endif
                if (*fmt == '.') {
                    fmt++;
                    if (__isdigit(*fmt)) {
                        m = *fmt++ - '0';
#if MORETHAN10
                        while (__isdigit(*fmt)) m = m * 10 + (*fmt++ - '0');
                    }
                }
                p = *fmt;
#endif
            } else
                n = -1;

            switch (p) {
#ifdef X86
            case 'I':
                if (('6' == *(fmt+1)) && ('4' == *(fmt+2))) {
                        vv = (unsigned long long)va_arg(args, long long);
                        fmt += 3;
                        size = xput_n(buf, fmt, vv, n, zeroflag, buf_size);
                }
                break;
#endif
            case 'l':
                {
                    char p1 = *(fmt+1);
                    char p2 = *(fmt+2);
                    if ((p1 == 'd') || (p1 == 'u') || (p1 == 'x') ||  (p1 == 'X')) {
#ifdef X86
                        vv = (unsigned long long)va_arg(args, long);
#else
                        vv = (unsigned long)va_arg(args, long);
#endif
                        fmt += 1;
                        size = xput_n(buf, fmt, vv, n, zeroflag, buf_size);
                    } else if ((p1 == 'l') && ((p2 == 'd') || (p2 == 'u') || (p2 == 'x') ||  (p2 == 'X'))) {
#ifdef X86
                        vv = (unsigned long long)va_arg(args, long long);
#else
                        vv = (unsigned long)va_arg(args, long long);
#endif
                        fmt += 2;
                        size = xput_n(buf, fmt, vv, n, zeroflag, buf_size);
                        size = xput_n(buf, fmt, vv, n, zeroflag, buf_size);
                    }
                }
                break;
            case 's':
                v = (unsigned long)va_arg(args, long);
                size = xput_s(buf, (char *)v, n, buf_size, minusflag);
                break;
            case 'c':
                v = (unsigned long)va_arg(args, long);
                size = xput_c(buf, (char)v, n, buf_size);
                break;
            case 'd':
                size = xput_n(buf, fmt, (long long)va_arg(args, long), n, zeroflag, buf_size);
                break;
            case 'b':
            case 'u':
            case 'o':
            case 'x':
            case 'e':
            case 'O':
            case 'X':
                v = (unsigned long)va_arg(args, long);
                size = xput_n(buf, fmt, (long long)v, n, zeroflag, buf_size);
                break;
#ifdef SUPPORT_FLOAT
            case 'f':
                f = (float)va_arg(args, double);
                size = xput_f(buf, fmt, f, n, m, zeroflag, buf_size);
                break;
#endif
            }
            if (size < 0)
                return -1;
            if (size) {
                buf += size;
                len += size;
                buf_size -= size;
            }
        }
        else {
vxsnprintf_next:
            if (buf_size == 0)
                return -1;
            *buf++ = p;
            len++;
            buf_size--;
        }
    }
    *buf++ = '\0';
    return len;
}

void SCI2_Init(void);
void SCI2_TxStr(unsigned char *);

void usb_debug_printf_init(void)
{
    SCI2_Init();
}

int usb_debug_printf(char* format, ...)
{
    char buf[256];
    int len;
    va_list arg_ptr;
    va_start(arg_ptr, format);
    len = tvsnprintf(buf, 255, format, arg_ptr);
    SCI2_TxStr((unsigned char *)buf);
    va_end(arg_ptr);
    return len;
}

void usb_memory_dump(unsigned long addr, int len)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        if ((i & 0xf) == 0) {
            usb_debug_printf((char *)"%08x ", (unsigned long)p);
        }
        usb_debug_printf((char *)"%02x ", (unsigned char)(*p & 0xff));
        if ((i & 0xf) == 0xf) {
            usb_debug_printf((char *)"\r\n");
        }
        p++;
    }
    usb_debug_printf((char *)"\r\n");
}
#endif
