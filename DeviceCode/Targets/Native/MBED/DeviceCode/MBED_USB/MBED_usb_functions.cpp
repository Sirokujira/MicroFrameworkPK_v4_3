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
//  Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
//  Portions Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Based on RENESAS usb_hal.cpp
////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#define TARGET_RZ_A1H
#include "mbed.h"
#include <Include\USB_decl.h>
#include <pal\com\usb\USB.h>
#include "MBED_usb_functions.h"

//*********************************************************************************
// NETMF default endpoint
// EP0  Control
// EP1  Bulk In
// EP2  Bulk Out
//*********************************************************************************

//#define DEBUG_USB

#if defined(DEBUG_USB)
#define USB_DEBUG1(x0)      dbg_printf(x0)
#define USB_DEBUG2(x0,x1)   dbg_printf(x0,x1)
#else
#define USB_DEBUG1(x0)
#define USB_DEBUG2(x0,x1)
#endif

USB_CONTROLLER_STATE g_MBED_USB_ControllerState;
static const USB_ENDPOINT_DESCRIPTOR  *USB_EPDesc[USB_MAX_EP];
Hal_Queue_KnownSize<USB_PACKET64,USB_QUEUE_PACKET_COUNT> QueueBuffers[USB_MAX_EP - 1];
volatile FIFOCTR *cfifoctr = (volatile FIFOCTR *)&USB200.CFIFOCTR;
volatile FIFOCTR *d0fifoctr = (volatile FIFOCTR *)&USB200.D0FIFOCTR;
volatile FIFOCTR *d1fifoctr = (volatile FIFOCTR *)&USB200.D1FIFOCTR;
volatile FIFOSEL *cfifosel = (volatile FIFOSEL *)&USB200.CFIFOSEL;
volatile FIFOSEL *d0fifosel = (volatile FIFOSEL *)&USB200.D0FIFOSEL;
volatile FIFOSEL *d1fifosel = (volatile FIFOSEL *)&USB200.D1FIFOSEL;
volatile PIPECFG *pipecfg = (volatile PIPECFG *)&USB200.PIPECFG;
volatile DCPCTR *dcpctr = (volatile DCPCTR *)&USB200.DCPCTR;

static UINT8 USB_EP0Buffer[MAX_EP0_SIZE];
static UINT16 USB_EPStatus[USB_MAX_EP];
static BOOL USB_PinsProtected = FALSE;
static UINT16 USB_EP_Type = 0;
static INT8 MBED_USB_Pipe[USB_MAX_EP];
static INT8 MBED_USB_Zero = 0;
static INT8 MBED_USB_FifoType[USB_MAX_PIPE];
static volatile PIPECTR *pPIPECTR[USB_MAX_PIPE] = {
    (volatile PIPECTR *)&USB200.DCPCTR,
    (volatile PIPECTR *)&USB200.PIPE1CTR,
    (volatile PIPECTR *)&USB200.PIPE2CTR,
    (volatile PIPECTR *)&USB200.PIPE3CTR,
    (volatile PIPECTR *)&USB200.PIPE4CTR,
    (volatile PIPECTR *)&USB200.PIPE5CTR,
    (volatile PIPECTR *)&USB200.PIPE6CTR,
    (volatile PIPECTR *)&USB200.PIPE7CTR,
    (volatile PIPECTR *)&USB200.PIPE8CTR,
    (volatile PIPECTR *)&USB200.PIPE9CTR
};

static volatile UINT16 *pFIFOSEL[USB_MAX_FIFO] =
{
    (volatile UINT16 *)&USB200.CFIFOSEL,
    (volatile UINT16 *)&USB200.D0FIFOSEL,
    (volatile UINT16 *)&USB200.D1FIFOSEL
};

static volatile UINT16 *pFIFOCTR[USB_MAX_FIFO] =
{
    (volatile UINT16 *)&USB200.CFIFOCTR,
    (volatile UINT16 *)&USB200.D0FIFOCTR,
    (volatile UINT16 *)&USB200.D1FIFOCTR
};

// H'E8010014
// H'E8010018
// H'E801001C
#if 1
static volatile UINT8 *pFIFO[USB_MAX_FIFO] =
{
    (volatile UINT8 *)0xe8010014,
    (volatile UINT8 *)0xe8010018,
    (volatile UINT8 *)0xe801001c
};
#else
static volatile UINT8 *pFIFO[USB_MAX_FIFO] =
{
    (volatile UINT8 *)&USB200.CFIFO.UINT8[LL],
    (volatile UINT8 *)&USB200.D0FIFO.UINT8[LL],
    (volatile UINT8 *)&USB200.D1FIFO.UINT8[LL]
};
#endif
static UINT32 pipe_bulk = PIPE_BULK_ST;
static UINT32 pipe_interrupt = PIPE_INT_ST;

GPIO_PIN en_usb = (GPIO_PIN)P4_1;

static void usb_memory_dump(unsigned int addr, int len)
{
    int i;
    char *p = (char *)addr;
    for (i = 0; i < len; i++) {
        if ((i & 0xf) == 0) {
            dbg_printf((char *)"%08x ", (unsigned int)p);
        }
        dbg_printf((char *)"%02x ", (unsigned char)(*p & 0xff));
        if ((i & 0xf) == 0xf) {
            dbg_printf((char *)"\r\n");
        }
        p++;
    }
    dbg_printf((char *)"\r\n");
}

static void wait_cnt(volatile UINT32 count)
{
    while (count-- > 0);
}

#define DUMMY_ACCESS (*(volatile unsigned long *)(OSTM0CNT))

void Userdef_USB_usb0_function_delay_1ms (void)
{
    volatile int i;
    volatile unsigned long tmp;

    /*
     * Wait 1ms (Please change for your MCU).
     */
    for (i = 0; i < 1440; ++i)
    {
        tmp = DUMMY_ACCESS;
    }
}

void Userdef_USB_usb0_function_delay_xms (UINT32 msec)
{
    volatile unsigned short i;

    for (i = 0; i < msec; ++i)
    {
        Userdef_USB_usb0_function_delay_1ms();
    }
}

void usb_int_brdy_enable(UINT16 pipe)
{
	UINT16 tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.BRDYENB |= (1 << pipe);
	USB200.INTENB0 = tmp;
}

void usb_int_brdy_disable(UINT16 pipe)
{
	UINT16  tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.BRDYENB &= ~(1 << pipe);
	USB200.INTENB0 = tmp;
}

void usb_int_nrdy_enable(UINT16 pipe)
{
	UINT16  tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.NRDYENB |= (1 << pipe);
	USB200.INTENB0 = tmp;
}

void usb_int_nrdy_disable(UINT16 pipe)
{
	UINT16 tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.NRDYENB &= ~(1 << pipe);
	USB200.INTENB0 = tmp;
}

void usb_int_bemp_enable(UINT16 pipe)
{
	UINT16 tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.BEMPENB |= (1 << pipe);
	USB200.INTENB0 = tmp;
}

void usb_int_bemp_disable(UINT16 pipe)
{
	UINT16 tmp;
	tmp = USB200.INTENB0;
	USB200.INTENB0 = (UINT16)(tmp & (UINT16)~(MASK_BEMPE|MASK_NRDYE|MASK_BRDYE));
	USB200.BEMPENB &= ~(1 << pipe);
	USB200.INTENB0 = tmp;
}

static void MBED_USB_Pipe_Initialize(void)
{
    UINT32 i;
    for (i = 0; i < USB_MAX_PIPE; i++) {
        MBED_USB_FifoType[i] = -1;
    }
    MBED_USB_FifoType[0] = FIFO_CFIFO;
    MBED_USB_FifoType[1] = FIFO_D0FIFO;
    MBED_USB_FifoType[2] = FIFO_D1FIFO;
    MBED_USB_FifoType[3] = FIFO_CFIFO;
    MBED_USB_FifoType[4] = FIFO_CFIFO;
    MBED_USB_FifoType[5] = FIFO_CFIFO;
    USB200.CFIFOSEL = 0;
    USB200.D0FIFOSEL = 1;
    USB200.D1FIFOSEL = 2;
}

UINT16 GetPipePid(UINT32 pipe)
{
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    return (pipectr->BIT.PID);
}

UINT16 Pipe_Inbuf(UINT16 pipe)
{
    UINT16 inbuf;
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    inbuf = pipectr->BIT.INBUFM;
    return inbuf;
}

#ifdef USB_DUMP_PID
void dump_pid(UINT32 pipe)
{
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    UINT16 pid = pipectr->BIT.PID;
    USB_DEBUG3("P%d:%d\r\n", pipe, pid);
}
#endif

void SetPipePidNAK(UINT32 pipe)
{
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    UINT16 cpid = (pipectr->BIT.PID);
    if (cpid == PID_STALL_2) {
        pipectr->BIT.PID = PID_STALL_1;
    }
    pipectr->BIT.PID = PID_NAK;
    if (cpid == PID_BUF) {
        while (pipectr->BIT.PBUSY == 1);
    }
    //USB_DEBUG2("K%d", pipe);
}

void SetPipePidBUF(UINT32 pipe)
{
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    pipectr->BIT.PID = PID_BUF;
    //USB_DEBUG2("F%d", pipe);
}

void ClearPipeFIFO(UINT32 pipe)
{
    if (cfifosel->BIT.CURPIPE == pipe)
        cfifosel->BIT.CURPIPE = 0;
    if (d0fifosel->BIT.CURPIPE == pipe)
        d0fifosel->BIT.CURPIPE = 0;
    if (d1fifosel->BIT.CURPIPE == pipe)
        d1fifosel->BIT.CURPIPE = 0;
}

#define FIFO_LOOP       4
#define FIFO_TIMEOUT    2000

void PipeSetFIFO(UINT16 pipe, UINT16 dir, UINT16 mbw)
{
    volatile UINT16 tmp;
    UINT8 type;
    if (pipe >= 0) {
        ClearPipeFIFO(pipe);
        type = MBED_USB_FifoType[pipe];
        volatile UINT16 *pfifosel = pFIFOSEL[type];
        volatile UINT16 *pfifoctr = pFIFOCTR[type];
        for (UINT32 i = 0; i < FIFO_LOOP; i++) {
            tmp = *pfifosel;
            if (type == FIFO_CFIFO) {
                tmp &= 0xFBD0;
                tmp |= (0x0100 | (mbw << 10) | (dir << 5) | pipe);
            } else {
                tmp &= 0xFBF0;
                tmp |= (0x0100 | (mbw << 10) | pipe);
            }
            *pfifosel = tmp;
            wait_cnt(FIFO_TIMEOUT);
            volatile UINT16 j = FIFO_TIMEOUT;
            while (j-- > 0) {
                if (*pfifosel == tmp)
                    break;
            }
            j = FIFO_TIMEOUT;
            while (j-- > 0) {  // FRDY
                if ((*pfifoctr & 0x2000) != 0)
                    return;
            }
        }
        lcd_printf("PipeSetFIFO err %d\r\n", pipe);
    }
}

void MBED_USB_Interrupt_Enable(void)
{
    //ICU.IER[0x04].BIT.IEN3  = 1u;   /* USBI0 enable */
    //ICU.IPR[35].BYTE    = 0x0E; /* USBI0 */
}

void MBED_USB_Pullup_Connect(void)
{
    // ToDo
    USB200.SYSCFG0 |= 0x0010;
}

void MBED_USB_Pullup_Disconnect(void)
{
    // ToDo
    USB200.SYSCFG0 &= 0xffef;
}

void MBED_USB_Interrupt_Initialize(void)
{
    // ToDo
    USB200.INTENB0 = 0x9F00;
    USB200.BRDYENB = 0x0000;
    USB200.BEMPENB = 0x0001;
    //USB200.SOFCFG = 0x40;
    //USB200.SOFCFG = 0x20;
    //USB200.INTENB0.BIT.VBSE = 1;          // b15
    //USB200.INTENB0.BIT.DVSE = 1;          // b12
    //USB200.INTENB0.BIT.CTRE = 1;          // b11
    //USB200.INTENB0.BIT.BEMPE = 1;         // b10
    //USB200.INTENB0.BIT.NRDYE = 1;         // b9
    //USB200.INTENB0.BIT.BRDYE = 1;         // b8

    //USB200.BEMPENB.BIT.PIPE0BEMPE = 1;    // pipe 0 bemp interrupt enable
    //USB200.BRDYENB.BIT.PIPE0BRDYE = 0;    // pipe 0 brdy inerrrupt disable
}

void MBED_USB_Interrupt_Uninitialize(void)
{
    // ToDo
    USB200.INTENB0 = 0x0000;
    USB200.BRDYENB = 0x0000;
    USB200.BEMPENB = 0x0000;
    //USB200.SOFCFG = 0x00;
    //USB200.SOFCFG = 0x20;
    //USB200.INTENB0.BIT.VBSE = 0;          // b15
    //USB200.INTENB0.BIT.DVSE = 0;          // b12
    //USB200.INTENB0.BIT.CTRE = 0;          // b11
    //USB200.INTENB0.BIT.BEMPE = 0;         // b10
    //USB200.INTENB0.BIT.NRDYE = 0;         // b9
    //USB200.INTENB0.BIT.BRDYE = 0;         // b8
}

void usb0_function_reset_module (UINT16 clockmode)
{
    /* UPLLE bit is only USB0 */
    if (RZA_IO_RegRead_16(&USB200.SYSCFG0,
                                USB_SYSCFG_UPLLE_SHIFT,
                                USB_SYSCFG_UPLLE) == 1)
    {
        if ((USB200.SYSCFG0 & USB_FUNCTION_BITUCKSEL) != clockmode)
        {
            RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                                0,
                                USB_SUSPMODE_SUSPM_SHIFT,
                                USB_SUSPMODE_SUSPM);
            USB200.SYSCFG0 = 0;
            USB200.SYSCFG0 = (USB_FUNCTION_BITUPLLE | clockmode);
            Userdef_USB_usb0_function_delay_xms(1u);
            RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                                1,
                                USB_SUSPMODE_SUSPM_SHIFT,
                                USB_SUSPMODE_SUSPM);
        }
        else
        {
            RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                                0,
                                USB_SUSPMODE_SUSPM_SHIFT,
                                USB_SUSPMODE_SUSPM);
            Userdef_USB_usb0_function_delay_xms(1u);
            RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                                1,
                                USB_SUSPMODE_SUSPM_SHIFT,
                                USB_SUSPMODE_SUSPM);
        }
    }
    else
    {
        RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                            0,
                            USB_SUSPMODE_SUSPM_SHIFT,
                            USB_SUSPMODE_SUSPM);
        USB200.SYSCFG0 = 0;
        USB200.SYSCFG0 = (USB_FUNCTION_BITUPLLE | clockmode);
        Userdef_USB_usb0_function_delay_xms(1u);
        RZA_IO_RegWrite_16(&USB200.SUSPMODE,
                            1,
                            USB_SUSPMODE_SUSPM_SHIFT,
                            USB_SUSPMODE_SUSPM);
    }

    USB200.BUSWAIT = (UINT16)(USB_FUNCTION_BUSWAIT_05 & USB_FUNCTION_BITBWAIT);
}

void USB0_HW_Initialize(void)
{
    // ToDo
    CPU_GPIO_EnableOutputPin(en_usb, 1);
    UINT16 clock_mode = USBFCLOCK_X1_48MHZ;
    UINT16 mode = USB_FUNCTION_HIGH_SPEED;

    CPG.STBCR7 &= ~(CPG_STBCR7_MSTP71);
    volatile uint8_t dummy8;
    dummy8 = CPG.STBCR7;
    /* reset USB module with setting tranciever and HSE=1 */
    usb0_function_reset_module(clock_mode);
    /* clear variables */
    //usb0_function_init_status();
    /* select USB Function and Interrupt Enable */
    /* Detect USB Device to attach or detach    */
    USB200.SYSCFG0 = (USB200.SYSCFG0 & 0xffbf) | (0 << 6);  //DCFM
    USB200.SYSCFG0 = (USB200.SYSCFG0 & 0xfffe) | (1 << 0);  //USBE
    if (mode == USB_FUNCTION_HIGH_SPEED)
        USB200.SYSCFG0 = (USB200.SYSCFG0 & 0xff7f) | (1 << 7);  //HSE
    else
        USB200.SYSCFG0 = (USB200.SYSCFG0 & 0xff7f) | (0 << 7);  //HSE
    USB200.BUSWAIT = 20;
    {
        UINT16 buf;
        buf  = USB200.INTENB0;
        buf |= USB_INTENB0_SOFE;
        USB200.INTENB0 = buf;
    }
}

void MBED_USB_HW_Initialize(void)
{
    USB0_HW_Initialize();
    MBED_USB_Pipe_Initialize();
    MBED_USB_Interrupt_Initialize();
    MBED_USB_Interrupt_Enable();
}

void MBED_USB_EP_Initialize(UINT32 pipe, UINT32 ep, UINT32 dir, UINT32 attr, UINT32 size)
{
    ClearPipeFIFO(pipe);
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    USB200.PIPESEL = pipe;
    SetPipePidNAK(pipe);
    pipecfg->WORD = 0;
    pipecfg->BIT.EPNUM = ep;        // Endpoint
    pipecfg->BIT.DIR = dir;         // Direction
    pipecfg->BIT.DBLB = 1;          // Double Buffer
    pipecfg->BIT.BFRE = 0;          // Interrupt
    //pipecfg->BIT.SHTNAK = 1;
    if (attr == USB_ENDPOINT_ATTRIBUTE_BULK) {
        pipecfg->BIT.TYPE = 1;      // BULK
        if (dir == 0) { // OUT
            //pipectr->BIT.ATREPM = 1;
            pipectr->BIT.SQCLR = 1;
            pipectr->BIT.ACLRM = 1;     // Clear Buffer
            pipectr->BIT.ACLRM = 0;     // Clear Buffer
            pipectr->BIT.PID = PID_BUF;
            //PipeSetFIFO((UINT16)pipe, dir, MBW_BYTE);
            usb_int_bemp_disable(pipe);
            usb_int_brdy_disable(pipe);
            //usb_int_brdy_enable(pipe);
        } else {        // IN
            //pipectr->BIT.ATREPM = 1;
            pipectr->BIT.SQCLR = 1;
            pipectr->BIT.ACLRM = 1;     // Clear Buffer
            pipectr->BIT.ACLRM = 0;     // Clear Buffer
            //PipeSetFIFO((UINT16) pipe, dir, MBW_BYTE);
            usb_int_bemp_disable(pipe);
            usb_int_brdy_disable(pipe);
        }
        USB200.PIPEMAXP = (USB200.PIPEMAXP & 0x03ff) | size;  // Packet Size
    } else if (attr == USB_ENDPOINT_ATTRIBUTE_INTERRUPT) {
        pipecfg->BIT.TYPE = 2;      // Transfer Type
        USB200.PIPEMAXP = (USB200.PIPEMAXP & 0x03ff) | size;  // Packet Size        //usb_int_bemp_disable(pipe);
        //usb_int_brdy_disable(pipe);
    } else if (attr == USB_ENDPOINT_ATTRIBUTE_ISOCHRONOUS) {
        pipecfg->BIT.TYPE = 3;      // Transfer Type
        USB200.PIPEMAXP = (USB200.PIPEMAXP & 0x03ff) | size;  // Packet Size        //usb_int_bemp_disable(pipe);
        //usb_int_brdy_disable(pipe);
    }
    USB200.PIPESEL = 0;       // Unselect Pipe
}

void USB_Control_Status(void)
{
    dcpctr->BIT.PID = PID_BUF;
    dcpctr->BIT.CCPL = 1;
}

void USB_Stall_Clear(INT32 pipe)
{
    volatile PIPECTR *pipectr = pPIPECTR[pipe];
    pipectr->BIT.PID = PID_NAK;
    while (pipectr->BIT.PBUSY == 1){;}
    pipectr->BIT.SQCLR = 1;
    pipectr->BIT.ACLRM = 1;
    pipectr->BIT.ACLRM = 0;
    USB200.PIPESEL = pipe & 0x000f;
    if (pipecfg->BIT.DIR == 0) {
        // receive direction
        pipectr->BIT.PID = PID_BUF;
    }
}

void Pipe_Config(void)
{
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    const USB_ENDPOINT_DESCRIPTOR *pep = (USB_ENDPOINT_DESCRIPTOR *)NULL;
    const USB_INTERFACE_DESCRIPTOR *pif  = (USB_INTERFACE_DESCRIPTOR *)NULL;
    while (USB_NextEndpoint(State, pep, pif)) {
        UINT32 endpoint = pep->bEndpointAddress & 0x7F;
        UINT32 dir = (pep->bEndpointAddress & 0x80)? 1:0;
        MBED_USB_EP_Initialize((UINT32)MBED_USB_Pipe[endpoint],
                (UINT32)endpoint, (UINT32)dir, pep->bmAttributes, pep->wMaxPacketSize);
    }
}

void MBED_USB_EP_Reset(void)
{
    UINT32 i;
    INT32 pipeno;
    for (i = 0; i < USB_MAX_EP; i++) {
        pipeno = MBED_USB_Pipe[i];
        if (pipeno >= 0)
            USB_Stall_Clear((UINT32)pipeno);
    }
}

void MBED_USB_Reset(void)
{
    USB_DEBUG1("Rt\r\n");
    UINT32 i;
    INT32 pipeno;
    Pipe_Config();
    MBED_USB_EP_Reset();
    MBED_USB_Interrupt_Initialize();
}

void Pipe_Read(UINT32 pipe, volatile UINT8 *buf, UINT32 size, volatile UINT32 *count)
{
    UINT8 dummy;
    UINT32 len;
    UINT8 type = MBED_USB_FifoType[pipe];
    GLOBAL_LOCK(irq);
    PipeSetFIFO((UINT16)pipe, DIR_OUT, MBW_BYTE);
    volatile UINT16 *pfifosel = (volatile UINT16 *)pFIFOSEL[type];
    volatile UINT16 *pfifoctr = (volatile UINT16 *)pFIFOCTR[type];
    volatile UINT8 *pfifo = (volatile UINT8 *)pFIFO[type];
    *pfifosel |= 0x8000;                // RCNT = 1
    len = *pfifoctr & 0x01FF;           // DTLN
    if (len == 0) {
        *pfifoctr |= 0x4000;            // BCLR = 1
    }
    if (buf != NULL) {
        while ((len!= 0) && (size > *count)) {
            *buf++ = *pfifo;
            (*count)++;
            len--;
        }
    }
    SetPipePidNAK(pipe);
    if ((*count == 0) && (0 != (*pfifoctr & 0x01FF))) {
        lcd_printf("fr error\r\n");
        while (0 != (*pfifoctr & 0x01FF)) {
            dummy = *pfifo;
        }
    }
    USB_DEBUG2("Pr%02d", *count);
}

void Pipe_Write(UINT32 pipe, volatile UINT8 *buf, UINT32 size, volatile UINT32 len)
{
    USB_DEBUG2("w%02d", len);
    //usb_memory_dump((unsigned int)buf, len);
    GLOBAL_LOCK(irq);
    PipeSetFIFO((UINT16)pipe, DIR_IN, MBW_BYTE);
    UINT32 count = 0;
    UINT8 type = MBED_USB_FifoType[pipe];
    volatile UINT16 *pfifoctr = (volatile UINT16 *)pFIFOCTR[type];
    volatile UINT8 *pfifo = (volatile UINT8 *)pFIFO[type];
    *pfifoctr = 0x4000;     // BCLR = 1
    if (len != 0) {
        if (buf != NULL) {
            while ((len != 0) && (size > count)) {
                *pfifo = (UINT8)*buf;
                buf++;
                count++;
                len--;
            }
        }
    }
    if (count != size) {
        *pfifoctr = 0x8000;     // BVAL = 1
    }
    SetPipePidBUF(pipe);
    usb_int_brdy_disable(pipe); // disable interrupt
    usb_int_bemp_enable(pipe);  // enable interrupt
    usb_int_nrdy_enable(pipe);  // enable interrupt
}

void Pipe0_Read(UINT8 *buf, UINT32 size, UINT32 *len)
{
    volatile UINT8 dummy = 0;
    UINT32 count = 0;
    PipeSetFIFO(0, 0, MBW_BYTE);
    cfifosel->BIT.RCNT = 1;
    while ((size > *len) && (0 != cfifoctr->BIT.DTLN)) {
        if (buf != NULL) {
            *buf = (UINT8)USB200.CFIFO.UINT8[LL];
            buf++;
            count++;
        } else {
            dummy = (UINT8)USB200.CFIFO.UINT8[LL];
        }
        (*len)++;
    }
    if (((0 != size) && (size == *len)) || (count != size)) {
        dcpctr->BIT.PID = PID_BUF;
        dcpctr->BIT.CCPL = 1;
    }
    USB_DEBUG2("Cr%d", count);
    if (0 != cfifoctr->BIT.DTLN) {
        lcd_printf("cr error\r\n");
    }
    (void) dummy;
}

void HandleSetupCmd(void)
{
    USB200.INTSTS0 &= ~USB_INTSTS0_VALID;
    
    UINT16 *buf = (UINT16 *)&USB_EP0Buffer[0];
    buf[0] = USB200.USBREQ;
    buf[1] = USB200.USBVAL;
    buf[2] = USB200.USBINDX;
    buf[3] = USB200.USBLENG;
    
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    
    State->Data = (UINT8 *)buf;
    State->DataSize = 8;
    UINT8 result = USB_ControlCallback(State);
    
    switch (result) {
    case USB_STATE_DATA:
        break;
    case USB_STATE_ADDRESS:
        break;
    case USB_STATE_DONE:
        State->DataCallback = NULL;
        break;
    case USB_STATE_STALL:
        return;
    case USB_STATE_STATUS:
        break;
    case USB_STATE_CONFIGURATION:
        break;
    case USB_STATE_REMOTE_WAKEUP:
        break;
    default:
        break;
    }
    
    if (State->DataCallback != NULL) {
        State->DataCallback(State);
        if (State->DataSize != 0) {
            Pipe_Write(0, (UINT8 *)State->Data, MAX_EP0_SIZE, State->DataSize);
        }
    }
    if (State->DataCallback == NULL) {
        USB_Control_Status();
    }
}

void HandleVBus(void)
{
    USB_DEBUG1("V");
    UINT16 vbsts_value;
    UINT8 count;
    #define CHATTER_COUNT 5
    do {
        count = CHATTER_COUNT;
        vbsts_value = USB200.INTSTS0 & 0x0080;
        while (count != 0) {
            if ((USB200.INTSTS0 & 0x0080) != vbsts_value) {
                break;
            } else {
                count--;
            }
        }
    } while (count != 0);
    if (0 != vbsts_value) { // connected;
        MBED_USB_Reset();
    } else {                // disconnected;
        MBED_USB_Reset();
    }
}

void HandleDVST(void)
{
    switch((USB200.INTSTS0 & 0x0070) >> 4)
    {
        case 0:         // Powered State
            USB_DEBUG1("D0\r\n");
            break;
        case 1:         // Default State
            USB_DEBUG1("D1\r\n");
            if ((USB200.DVSTCTR0 & 0x0007) == 4) {  // BUS RESET
                MBED_USB_EP_Reset();
            }
            break;
        case 2:         // Address State
            USB_DEBUG1("D2\r\n");
            break;
        case 3:         // Configured State
            USB_DEBUG1("D3\r\n");
            MBED_USB_EP_Reset();
            break;
        case 4:
        default:        // Suspended State
            USB_DEBUG1("D4\r\n");
            break;
    };
}

void HandleCTRT(void)
{
    switch(USB200.INTSTS0 & 0x0007)
    {
        case 0:         // IDLE or Setup
            USB_DEBUG1("C0\r\n");
            break;
        case 1:         // Control Read Data
            USB_DEBUG1("C1");
            HandleSetupCmd();
            break;
        case 2:         // Control Read Status
            USB_DEBUG1("C2");
            USB_Control_Status();
            break;
        case 3:         // Control Write Data
            USB_DEBUG1("C3");
            HandleSetupCmd();
            break;
        case 4:         // Control Write Status
            USB_DEBUG1("C4");
            break;
        case 5:         // Control Write(no data) Status
            USB_DEBUG1("C5");
            HandleSetupCmd();
        break;
        case 6:         // Control Transfer Error
            USB_DEBUG1("C6");
        break;
        default:        // INVALID
            USB_DEBUG1("C7");
        break;
    };
}

void MBED_USB_Send(USB_CONTROLLER_STATE *State, UINT32 endpoint)
{
    GLOBAL_LOCK(irq);
    USB_PACKET64* Packet64 = USB_TxDequeue(State, endpoint, TRUE);
    UINT32 pipeno = MBED_USB_Pipe[endpoint];
    const USB_ENDPOINT_DESCRIPTOR *pep = USB_EPDesc[endpoint];
    if (Packet64) {
        Pipe_Write(pipeno, (UINT8 *)Packet64->Buffer, pep->wMaxPacketSize, (UINT32)Packet64->Size);
#ifdef USB_USE_TXZERO_INT
        if (pep->wMaxPacketSize == Packet64->Size)
            MBED_USB_Zero = 1;
#endif
    } else {
        USB_DEBUG1("brw0\r\n");
        //MBED_USB_Zero = 2;
        //Pipe_Write(pipeno, 0, pep->wMaxPacketSize, 0);
    }
}

void MBED_USB_Receive(USB_CONTROLLER_STATE *State, UINT32 endpoint)
{
    GLOBAL_LOCK(irq);
    BOOL full;
    UINT32 pipeno = MBED_USB_Pipe[endpoint];
    USB_PACKET64* Packet64 = USB_RxEnqueue(State, endpoint, full);
    if (Packet64) {
        const USB_ENDPOINT_DESCRIPTOR *pep = USB_EPDesc[endpoint];
            UINT32 size = 0;
            Pipe_Read(pipeno, (UINT8 *)Packet64->Buffer, pep->wMaxPacketSize, &size);
            Packet64->Size = (UINT8)size;
            usb_int_brdy_disable(pipeno);
    } else {
        USB_DEBUG1("brr0\r\n");
        //usb_int_brdy_disable(pipeno);
    }

}

// BEMP Interrupt Handler
// TX: when TX completion
// RX: when receiving data > MaxPackectSize
void HandleBEMP(void)
{
    GLOBAL_LOCK(irq);
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    UINT16 bempsts = (UINT16)USB200.BEMPSTS;
    UINT16 bempenb = (UINT16)USB200.BEMPENB;
    if (1 == (USB200.BEMPSTS & 0x0001)) {
        USB_DEBUG1("e0");
        USB200.BEMPSTS = ~0x0001;
        if (State->DataCallback != NULL) {
            State->DataCallback(State);
            Pipe_Write(0, (UINT8 *)State->Data, (UINT32)MAX_EP0_SIZE, (UINT32)State->DataSize);
        } else {
            SetPipePidNAK(0);
        }
    } else {
        USB200.BEMPSTS = ~bempsts;
        for (UINT32 endpoint = 1; endpoint < USB_MAX_EP; endpoint++) {
            INT8 pipeno = MBED_USB_Pipe[endpoint];
            if (pipeno >= 0) {
                UINT16 mask = (1 << pipeno);
                if ((bempsts & mask) && (bempenb & mask)) {
                    UINT16 pid = GetPipePid((UINT32)pipeno);
                    if ((pid == PID_STALL_1) || (pid == PID_STALL_2)) {
                        USB_DEBUG2("St%d", pid);
                    } else {
                        if (Pipe_Inbuf((UINT32)pipeno) == 0) {
                            usb_int_bemp_disable((UINT32)pipeno);
                            SetPipePidNAK((UINT32)pipeno);
                        }
                    }
                }
            }
        }
    }
}

void HandleNRDY(void)
{
    GLOBAL_LOCK(irq);
    UINT16 nrdysts = (UINT16)USB200.NRDYSTS;
    UINT16 nrdyenb = (UINT16)USB200.NRDYENB;
    USB200.NRDYSTS = ~nrdysts;
    for (UINT32 endpoint = 1; endpoint < USB_MAX_EP; endpoint++) {
        INT8 pipeno = MBED_USB_Pipe[endpoint];
        if (pipeno >= 0) {
            UINT16 mask = (1 << pipeno);
            if ((nrdysts & mask) && (nrdyenb & mask)) {
                USB200.NRDYSTS = ~mask;
                usb_int_nrdy_disable((UINT32)pipeno);
            }
        }
    }
}

void HandleBRDY(void)
 {
	UINT16 brdysts = (UINT16)USB200.BRDYSTS;
	UINT16 brdyenb = (UINT16)USB200.BRDYENB;
	USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
	if (((USB200.BRDYSTS & 0x0001) == 1)
	        && ((USB200.BRDYENB & 0x0001) == 1)) {
		USB_DEBUG1("b0");
		USB200.BRDYSTS = ~0x0001;
		State->Data = (UINT8 *)&USB_EP0Buffer[0];
		UINT32 size = 0;
		Pipe0_Read((UINT8 *)State->Data, MAX_EP0_SIZE, &size);
		State->DataSize = (UINT8)size;
	} else {
		for (UINT32 endpoint = 1; endpoint < USB_MAX_EP; endpoint++) {
			INT8 pipeno = MBED_USB_Pipe[endpoint];
			if (pipeno >= 0) {
			    UINT16 mask = (1 << pipeno);
			    if ((brdysts & mask) && (brdyenb & mask)) {
			        USB200.BRDYSTS = ~mask;
			        USB200.PIPESEL = pipeno & 0x000f;
			        if (pipecfg->BIT.DIR == 0) {
			            MBED_USB_Receive(State, endpoint);  // OUT
			        } else {
			            MBED_USB_Send(State, endpoint);     // IN
			        }
			    }
			}
		}
	}
 }

static void USBInterruptHandler(void)
{
    if (0 != (USB200.INTSTS0 & 0x8000)) { // VBINT
        USB200.INTSTS0 &= ~0x8000;
        HandleVBus();
    }
    if (0 != (USB200.INTSTS0 & 0x1000)) { // DVST
        USB200.INTSTS0 &= ~0x1000;
        HandleDVST();
    }
    if (0 != (USB200.INTSTS0 & 0x0800)) { // CTRT
        HandleCTRT();
        USB200.INTSTS0 &= ~0x0800;
    }
    if (0 != (USB200.INTSTS0 & 0x0400)) { // BEMP
        HandleBEMP();
    }
    if (0 != (USB200.INTSTS0 & 0x0200)) { // NRDY
        HandleNRDY();
    }
    if (0 != (USB200.INTSTS0 & 0x0100)) { // BRDY
        HandleBRDY();
    }	
}

USB_CONTROLLER_STATE * CPU_USB_GetState(int Controller)
{
    if (Controller != 0)
        return NULL;
    return (USB_CONTROLLER_STATE *)&g_MBED_USB_ControllerState;
}

// CPU_USB_Initialize Function
//  Initializes the client module at the driver layer.
HRESULT CPU_USB_Initialize(int Controller)
{
#if defined(DEBUG_USB)
    DebuggerPort_Initialize(COM1);
#endif
    GLOBAL_LOCK(irq);
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    if (Controller != 0) return S_FALSE;
#ifdef RENESAS_FW
    USB_Init();
#endif
    MBED_USB_HW_Initialize();
    USB_PinsProtected = FALSE;
    CPU_USB_ProtectPins(Controller, TRUE);;
    State->DeviceState = USB_DEVICE_STATE_DETACHED;
    USB_StateCallback(State);
    USB_PinsProtected = TRUE;
    State->EndpointCount = USB_MAX_EP;
    State->EndpointStatus = &USB_EPStatus[0];
    State->PacketSize = MAX_EP0_SIZE;
    UINT32 endpoint;
    for (endpoint = 1; endpoint < USB_MAX_EP; endpoint++) {
        State->CurrentPacketOffset[endpoint] = 0;
        State->IsTxQueue[endpoint] = FALSE;
        State->MaxPacketSize[endpoint] = USB_MAX_DATA_PACKET_SIZE;
        MBED_USB_Pipe[endpoint] = -1;
    }
    USB_EP_Type = 0;
    MBED_USB_Zero = 0;
    pipe_interrupt = PIPE_INT_ST;
    pipe_bulk = PIPE_BULK_ST;
    const USB_ENDPOINT_DESCRIPTOR *pep = (USB_ENDPOINT_DESCRIPTOR *)NULL;
    const USB_INTERFACE_DESCRIPTOR *pif  = (USB_INTERFACE_DESCRIPTOR *)NULL;
    while (USB_NextEndpoint(State, pep, pif)) {
        endpoint = pep->bEndpointAddress & 0x7F;
        UINT8 endpointSize = pep->wMaxPacketSize;
        State->MaxPacketSize[endpoint] = endpointSize;
        QueueBuffers[endpoint-1].Initialize();
        State->Queues[endpoint] = &QueueBuffers[endpoint-1];
        USB_EPDesc[endpoint] = pep;
        switch (pep->bmAttributes) {
        case USB_ENDPOINT_ATTRIBUTE_BULK:
            MBED_USB_Pipe[endpoint] = pipe_bulk;
            if (pep->bEndpointAddress & 0x80) {
                MBED_USB_EP_Initialize((UINT32)pipe_bulk++,
                        (UINT32)endpoint, (UINT32)1, pep->bmAttributes, endpointSize);
                State->IsTxQueue[endpoint] = TRUE;
            } else {
                MBED_USB_EP_Initialize((UINT32)pipe_bulk++,
                        (UINT32)endpoint, (UINT32)0, pep->bmAttributes, endpointSize);
            }
            break;
        case USB_ENDPOINT_ATTRIBUTE_INTERRUPT:
            MBED_USB_Pipe[endpoint] = pipe_interrupt;
            if (pep->bEndpointAddress & 0x80) {
                MBED_USB_EP_Initialize(pipe_interrupt++,
                        endpoint, 1, pep->bmAttributes, endpointSize);
                State->IsTxQueue[endpoint] = TRUE;
            } else {
                MBED_USB_EP_Initialize(pipe_interrupt++,
                        endpoint, 0, pep->bmAttributes, endpointSize);
            }
            break;
        case USB_ENDPOINT_ATTRIBUTE_ISOCHRONOUS:
            // ToDo
        default:
            break;
        }
        USB_EP_Type |= (pep->bmAttributes & 3) << (endpoint * 2);
    }
    if (USB_EP_Type == 0)
        return S_FALSE;
    HAL_Time_Sleep_MicroSeconds(1000);
    MBED_USB_Interrupt_Enable();
    CPU_USB_ProtectPins(Controller, FALSE);
    InterruptHandlerRegister(USBI0_IRQn, USBInterruptHandler);
    GIC_SetPriority(USBI0_IRQn, 5);
    GIC_EnableIRQ(USBI0_IRQn);
    USB_DEBUG1("*I*");
    return S_OK;
}

// CPU_USB_Uninitialize Function
//  Uninitializes (shuts down) USB communications at the driver layer.
HRESULT CPU_USB_Uninitialize(int Controller)
{
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    GLOBAL_LOCK(irq);
    memset(&g_MBED_USB_ControllerState, 0, sizeof(g_MBED_USB_ControllerState));
    CPU_USB_ProtectPins(Controller, TRUE);
    USB_EP_Type = 0;
    State->DeviceState = USB_DEVICE_STATE_DETACHED;
    MBED_USB_Interrupt_Uninitialize();
    GIC_DisableIRQ(USBI0_IRQn);
    USB_DEBUG1("*U*");
    return S_OK;
}

// CPU_USB_StartOutput Function
//  Begins sending output to a specified USB port.
BOOL CPU_USB_StartOutput(USB_CONTROLLER_STATE* State, int endpoint)
{
    //GLOBAL_LOCK(irq);
    USB_DEBUG2("\r\nSo%d ", endpoint);
    if (State == NULL || endpoint >= USB_MAX_EP)
        return FALSE;
    if (State->Queues[endpoint] == NULL || !State->IsTxQueue[endpoint])
        return FALSE;
    if (USB_EPStatus[endpoint] & USB_STATUS_ENDPOINT_HALT) {
        while (USB_TxDequeue(State, endpoint, TRUE) != NULL);  // clear TX queue
        return TRUE;
    }
    UINT32 pipeno = MBED_USB_Pipe[endpoint];
    if (Pipe_Inbuf(pipeno) == 0) {
        SetPipePidNAK(pipeno);
        MBED_USB_Send(State, endpoint);
        return TRUE;
    } else
        return FALSE;
}

// CPU_USB_RxEnable Function
//  Enables a specified USB port to receive data.
BOOL CPU_USB_RxEnable(USB_CONTROLLER_STATE* State, int endpoint)
{
    GLOBAL_LOCK(irq);
    USB_DEBUG2("\r\nRe%d ", endpoint);
    UINT32 pipeno = MBED_USB_Pipe[endpoint];
    SetPipePidBUF(pipeno);
    usb_int_brdy_enable(pipeno);
    return TRUE;
}

// CPU_USB_GetInterruptState Function
//  Determines whether there are currently interrupts pending for the USB port.
BOOL CPU_USB_GetInterruptState()
{
    return FALSE;
}

// Puts USB pins into a known state to prevent spurious inputs.
// Parameters
// On
// A value you can set to true to protect the USB pins; set it to false to disable protection.
// Return Value
// true if the operation succeeded; otherwise, false.
//
// Setting the On parameter to true detaches the USB module from the host.
// This protects the USB pins by stopping all USB communications.
// Setting On to false enables the pins, thus enabling the host to detect the USB client.
// As soon as the USB client is detected, the enumeration process begins.
// USB communications resume when the enumeration process has finished.
BOOL CPU_USB_ProtectPins(int Controller, BOOL On)
{
    USB_CONTROLLER_STATE *State = &g_MBED_USB_ControllerState;
    GLOBAL_LOCK(irq);
    if (On == TRUE) {
        if (USB_PinsProtected == FALSE) {
            USB_DEBUG1("+");
            USB_PinsProtected = TRUE;
            MBED_USB_Pullup_Disconnect();
            MBED_USB_Reset();
            for(int ep = 1; ep < USB_MAX_EP; ep++) {
                if (State->Queues[ep] && State->IsTxQueue[ep]) {
                    while (USB_TxDequeue(State, ep, TRUE) != NULL);
                }
            }
            State->DeviceState = USB_DEVICE_STATE_DETACHED;
            USB_StateCallback(State);
        }
    } else {
        if (USB_PinsProtected == TRUE) {
            USB_DEBUG1("-");
            USB_PinsProtected = FALSE;
            MBED_USB_Pullup_Connect();
            MBED_USB_Reset();
            State->DeviceState = USB_DEVICE_STATE_ATTACHED;
            USB_StateCallback(State);
        }
    }
    return TRUE;
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_USB0_D0FIFO0(void) { }
void __attribute__ ((interrupt)) INT_Excep_USB0_D1FIFO0(void) { }
void __attribute__ ((interrupt)) INT_Excep_USB0_USBI0(void)
{
    USBInterruptHandler();
}
#ifdef __cplusplus
}
#endif
