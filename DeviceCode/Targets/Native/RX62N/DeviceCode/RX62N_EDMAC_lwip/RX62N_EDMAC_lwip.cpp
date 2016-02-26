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

/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2008. Renesas Technology Corp., All Rights Reserved.
*******************************************************************************
* File Name    : r_ether.c
* Version      : 1.03
* Description  : Ethernet module device driver
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 15.02.2010 1.00    First Release
*         : 03.03.2010 1.01    Buffer size is aligned on the 32-byte boundary.
*         : 08.03.2010 1.02    Modification of receiving method
*         : 06.04.2010 1.03    RX62N chnages
******************************************************************************/

#include <tinyhal.h>

#include "..\iodefine.h"
#include "net_decl_lwip.h"
#include "RX62N_EDMAC_lwip.h"
#include "RX62N_EDMAC_lwip_adapter.h"

#include "lwip\netif.h"
#include "lwip\pbuf.h"
#include "lwip\mem.h"

//#define USE_ETHER_INT

extern struct netif g_RX62N_EDMAC_NetIF;
extern HAL_COMPLETION InterruptTaskCompletion;

#define ALIGN(X,Y) ( (X+Y-1)/Y*Y )
#define ALIGNED_BUFSIZE ALIGN(BUFSIZE,32)

void _eth_fifoInit(ethfifo p[], UINT32 status);
INT32 _eth_fifoWrite(ethfifo *p, UINT8 buf[], UINT32 size);
INT32 _eth_fifoRead(ethfifo *p, UINT8 buf[]);

ethfifo rxdesc[ENTRY] __attribute__((aligned(16)));
ethfifo txdesc[ENTRY] __attribute__((aligned(16)));
INT8 rxbuf[ENTRY][ALIGNED_BUFSIZE] __attribute__((aligned(16)));
INT8 txbuf[ENTRY][ALIGNED_BUFSIZE] __attribute__((aligned(16)));
INT8 tmpbuf[ALIGNED_BUFSIZE] __attribute__((aligned(16)));

/**
 * Ethernet device driver control structure initialization
 */
struct ei_device  le0 =
{
        (INT8 *)"eth0",       /* device name */
        0,            /* open */
        0,            /* Tx_act */
        0,            /* Rx_act */
        0,            /* txing */
        0,            /* irq lock */
        0,            /* dmaing */
        0,            /* current receive discripter */
        0,            /* current transmit discripter */
        0,            /* save irq */
        {
                0,          /* rx packets */
                0,          /* tx packets */
                0,          /* rx errors */
                0,          /* tx errors */
                0,          /* rx dropped */
                0,          /* tx dropped */
                0,          /* multicast */
                0,          /* collisions */

                0,          /* rx length errors */
                0,          /* rx over errors */
                0,          /* rx CRC errors */
                0,          /* rx frame errors */
                0,          /* rx fifo errors */
                0,          /* rx missed errors */

                0,          /* tx aborted errors */
                0,          /* tx carrier errors */
                0,          /* tx fifo errors */
                0,          /* tx heartbeat errors */
                0           /* tx window errors */
        },
        0,            /* MAC 0 */
        0,            /* MAC 1 */
        0,            /* MAC 2 */
        0,            /* MAC 3 */
        0,            /* MAC 4 */
        0             /* MAC 5 */
};

void  _eth_fifoInit(ethfifo p[], UINT32 status)
{
    ethfifo  *current = 0;
    INT32 i, j;

    for (i = 0 ;i < ENTRY; i++) {
        current = &p[i];
        if (status == 0)
            current->buf_p = (UINT8 *)&txbuf[i][0];
        else
            current->buf_p = (UINT8 *)&rxbuf[i][0];
        for (j = 0 ;j < ALIGNED_BUFSIZE; j++)
            current->buf_p[j] = 0;
        current->bufsize = ALIGNED_BUFSIZE;
        current->size = 0;
        current->status = status;
        current->next = &p[i+1];
    }
    current->status |= DL;
    current->next = &p[0];
}

INT32 _eth_fifoWrite(ethfifo *p, INT8 buf[], INT32 size)
{
    INT32 i;
    ethfifo  *current = p;

    if ((current->status & ACT) != 0) {
        /**
         * Current descriptor is active and ready to transmit or transmitting.
         **/
        return( -1 );
    }

    for (i = 0; i < size; i++) {
        if (i >= ALIGNED_BUFSIZE)
            break;
        else
            /* transfer packet data */
            current->buf_p[i] = buf[i];
    }
    current->bufsize = (UINT16)i;
    return i;
}

INT32 _eth_fifoRead(ethfifo *p, INT8 buf[])
{
    INT32 i, temp_size;
    ethfifo  *current = p;

    if ((current->status & ACT) != 0) {
        /**
         * Current descriptor is active and ready to receive or receiving.
         * This is not an error.
         **/
        return (-1);
    } else if ((current->status & FE) != 0) {
        /**
         * Frame error.
         * Must move to new descriptor as E-DMAC now points to next one.
         **/
        return (-2);
    } else {
        if ((current->status & FP0) == FP0) {
            /* This is the last descriptor.  Complete frame is received.   */
            temp_size = current->size;
            while (temp_size > ALIGNED_BUFSIZE) {
                temp_size -= ALIGNED_BUFSIZE;
            }
        } else {
            /**
             * This is either a start or continuos descriptor.
             * Complete frame is NOT received.
             **/
            temp_size = ALIGNED_BUFSIZE;
        }
        /* Copy received data from receive buffer to user buffer */
        for (i = 0; i < temp_size; i++) {
            buf[i] = current->buf_p[i];
        }
        /* Return data size received */
        return (temp_size);
    }
}

static void _phy_mii_write_1(void)
    {
    volatile INT32 j;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000006;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000007;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000007;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000006;
}

static void _phy_mii_write_0(void)
{
    volatile INT32 j;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000002;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000003;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000003;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000002;
}

static void _phy_ta_z0(void)
{
    volatile INT32 j;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000000;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000001;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000001;
    for (j = MDC_WAIT; j > 0; j--)
        ETHERC.PIR.LONG = 0x00000000;
}

static void _phy_ta_10(void)
{
    _phy_mii_write_1();
    _phy_mii_write_0();
}

static void _phy_preamble(void)
    {
    volatile INT16 i = 32;
    while (i-- > 0)
        _phy_mii_write_1();
}

static void _phy_reg_set(UINT16 phy_addr, UINT16 reg_addr, INT32 option)
{
    volatile INT32 i;
    UINT16 data = (PHY_ST << 14);   /* ST code */
    if( option == PHY_READ )
        data |= (PHY_READ << 12);  /* OP code(RD)  */
    else
        data |= (PHY_WRITE << 12);  /* OP code(WT)  */
    data |= (phy_addr << 7);    /* PHY Address  */
    data |= (reg_addr << 2);    /* Reg Address  */

    i = 14;
    while (i-- > 0) {
        if( (data & 0x8000) == 0 )
            _phy_mii_write_0();
        else
            _phy_mii_write_1();
        data <<= 1;
    }
}

static void _phy_reg_read(UINT16 *data)
{
    volatile INT32 i, j;
    UINT16   reg_data;

    reg_data = 0;
    i = 16;
    while (i-- > 0) {
        for(j = MDC_WAIT; j > 0; j--)
            ETHERC.PIR.LONG = 0x00000000;
        for(j = MDC_WAIT; j > 0; j--)
            ETHERC.PIR.LONG = 0x00000001;
        reg_data <<= 1;
        reg_data |= (UINT16)((ETHERC.PIR.LONG & 0x00000008) >> 3);  /* MDI read  */

        for(j = MDC_WAIT; j > 0; j--)
            ETHERC.PIR.LONG = 0x00000001;
        for(j = MDC_WAIT; j > 0; j--)
            ETHERC.PIR.LONG = 0x00000000;
        }
    *data = reg_data;
}

static void  _phy_reg_write(UINT16 data)
{
    volatile INT32 i;
    i = 16;
    while (i-- > 0) {
        if( (data & 0x8000) == 0 )
            _phy_mii_write_0();
        else
            _phy_mii_write_1();
        data <<= 1;
    }
}

static UINT16 _phy_read(UINT16 phy_addr, UINT16 reg_addr)
{
    UINT16 data;

    GLOBAL_LOCK(irq);
    _phy_preamble();
    _phy_reg_set(phy_addr, reg_addr, PHY_READ);
    _phy_ta_z0();
    _phy_reg_read(&data);
    _phy_ta_z0();

    return( data );
}

static void _phy_write(UINT16 phy_addr, UINT16 reg_addr, UINT16 data)
{
    GLOBAL_LOCK(irq);
    _phy_preamble();
    _phy_reg_set(phy_addr, reg_addr, PHY_WRITE);
    _phy_ta_10();
    _phy_reg_write(data);
    _phy_ta_z0();
}


static INT16 phy_init(UINT16 phy_addr)
    {
    UINT16 reg;
    UINT32 count;
    _phy_write(phy_addr, BASIC_MODE_CONTROL_REG, 0x8000);
    count = 0;
    do {
        reg = _phy_read(phy_addr, BASIC_MODE_CONTROL_REG);
        count++;
    } while (reg & 0x8000 && count < PHY_RESET_WAIT);

    if (count >= PHY_RESET_WAIT)
        return R_PHY_ERROR;
    else
        return R_PHY_OK;
}

static void phy_set_100full(UINT16 phy_addr)
{
    _phy_write(phy_addr, BASIC_MODE_CONTROL_REG, 0x2100);
    }

static void phy_set_10half( UINT16 phy_addr )
    {
    _phy_write(phy_addr, BASIC_MODE_CONTROL_REG, 0x0000);
    }

static INT16 phy_set_autonegotiate(UINT16 phy_addr)
    {
    UINT16 reg;
    volatile UINT32 count;
    _phy_write(phy_addr, AN_ADVERTISEMENT_REG, 0x01E1);
    _phy_write(phy_addr, BASIC_MODE_CONTROL_REG, 0x1200);
    count = 0;
    do
    {
        reg = _phy_read(phy_addr, BASIC_MODE_STATUS_REG);
        reg = _phy_read(phy_addr, BASIC_MODE_STATUS_REG);
        count++;
    } while (!(reg & 0x0020) && count < PHY_AUTO_NEGOTIATON_WAIT);

    if (count >= PHY_AUTO_NEGOTIATON_WAIT)
        return R_PHY_ERROR;
    else
        return ((INT16)_phy_read(phy_addr, AN_LINK_PARTNER_ABILITY_REG));
}

////////////////////////////////////////////////////////////////////////////

BOOL RX62N_EDMAC_lwip_WritePhy(UINT32 phy_addr, UINT32 reg_addr, UINT32 data, UINT32 retry)
{
    _phy_write((UINT16)phy_addr, (UINT16)reg_addr, (UINT16)data);
    return true;
}

BOOL RX62N_EDMAC_lwip_ReadPhy(UINT32 phy_addr, UINT32 reg_addr, UINT32 *data, UINT32 retry)
{
    *data = (UINT32)_phy_read((UINT16)phy_addr, (UINT16)reg_addr);
    return true;
}

void RX62N_EDMAC_lwip_SetLinkSpeed(BOOL speed, BOOL fullduplex)
{
    if (speed) {
        ETHERC.ECMR.BIT.RTM = 1;        // 100Mbps
        dbg_printf("Ethernet Link 100M/");
    } else {
        ETHERC.ECMR.BIT.RTM = 0;        // 10Mbps
        dbg_printf("Ethernet Link 10M/");
    }
    if (fullduplex) {
        ETHERC.ECMR.BIT.DM = 1;         // full duplex
        dbg_printf("Full Duplex.\r\n");
    } else {
        ETHERC.ECMR.BIT.DM = 0;         // half duplex
        dbg_printf("Half Duplex.\r\n");
    }
}

BOOL RX62N_EDMAC_LWIP_Init(struct netif *pNetIf)
{
    UINT32 mac_h,mac_l;
    volatile UINT32 i;
    BOOL flag;

    le0.open = 1;
    _eth_fifoInit(rxdesc, (UINT32)ACT);
    _eth_fifoInit(txdesc, (UINT32)0);
    le0.rxcurrent = &rxdesc[0];
    le0.txcurrent = &txdesc[0];
    le0.mac_addr[0] = pNetIf->hwaddr[0];
    le0.mac_addr[1] = pNetIf->hwaddr[1];
    le0.mac_addr[2] = pNetIf->hwaddr[2];
    le0.mac_addr[3] = pNetIf->hwaddr[3];
    le0.mac_addr[4] = pNetIf->hwaddr[4];
    le0.mac_addr[5] = pNetIf->hwaddr[5];

    EDMAC.EDMR.BIT.SWR = 1;                 // reset EDMAC and ETHERC
    for (i = 0; i < 0x00000100; i++) ;

    ETHERC.ECSR.LONG = 0x00000037;          // clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD
    ETHERC.ECSIPR.LONG = 0x00000020;        // disable ETHERC status change interrupt
    ETHERC.RFLR.LONG = 1518;                // ether payload is 1500+ CRC
    ETHERC.IPGR.LONG = 0x00000014;          // Intergap is 96-bit time

    mac_h = ((UINT32)pNetIf->hwaddr[0] << 24) | \
            ((UINT32)pNetIf->hwaddr[1] << 16) | \
            ((UINT32)pNetIf->hwaddr[2] << 8 ) | \
            (UINT32)pNetIf->hwaddr[3];
    mac_l = ((UINT32)pNetIf->hwaddr[4] << 8 ) | \
            (UINT32)pNetIf->hwaddr[5];
    if (mac_h == 0 && mac_l == 0) {
        //
    } else {
        ETHERC.MAHR = mac_h;
        ETHERC.MALR.LONG = mac_l;
    }

    /* EDMAC */
    EDMAC.EESR.LONG = 0x47FF0F9F;           // clear all EDMAC status bits
    EDMAC.RDLAR = le0.rxcurrent;            // initialize Rx Descriptor List Address
    EDMAC.TDLAR = le0.txcurrent;            // initialize Tx Descriptor List Address
    EDMAC.TRSCER.LONG = 0x00000000;         // copy-back status is RFE & TFE only
    EDMAC.TFTR.LONG = 0x00000000;           // threshold of Tx_FIFO
    EDMAC.FDR.LONG = 0x00000707;            // transmit fifo & receive fifo is 2048 bytes
    // Configure receiving method
    // b0        RNR - Receive Request Bit Reset - Continuous reception of multiple frames is possible.
    // b1        RNC - Receive Request Bit Non-Reset Mode - The RR bit is automatically reset.
    // b31:b2    Reserved set to 0
    EDMAC.RMCR.LONG   = 0x00000001;
    EDMAC.EDMR.BIT.DE = 1;

    // Initialize PHY
    flag = ENET_PHY_lwip_init();
    if (!flag)
        return false;
#if defined(USE_ETHER_INT)
    // Enable interrupt
    // Sets up interrupt when you use interrupt
    EDMAC.EESIPR.LONG = 0x00040000;
    // ICU.IER[4].BIT.IEN0 = 1;
    // ICU.IPR[8].BYTE = 4;    // Set priority level
    IPR(ETHER, EINT) = 4;
    IEN(ETHER, EINT) = 1;
#endif
    // Enable receive and transmit
    ETHERC.ECMR.BIT.RE = 1;
    ETHERC.ECMR.BIT.TE = 1;

    // Enable EDMAC receive
    EDMAC.EDRRR.LONG  = 0x00000001;
    for (i = 0; i < 0x00000100; i++) ;
    return true;
}

BOOL RX62N_EDMAC_LWIP_open(struct netif *pNetIf)
{
    return RX62N_EDMAC_LWIP_Init(pNetIf);
}

void RX62N_EDMAC_LWIP_close(struct netif *pNetIf)
{
    le0.open = 0;
    ETHERC.ECMR.LONG = 0x00000000;        // disable TE and RE
    le0.irqlock = 1;
}


err_t RX62N_EDMAC_LWIP_xmit(struct netif *pNetIf, struct pbuf *pPBuf)
{
    INT32 xmit;
    INT32 flag = FP1;
    INT8 *data;

    GLOBAL_LOCK(irq);
    if (!pNetIf || !pPBuf)
        return ERR_ARG;
    while (pPBuf) {
        // ToDo - Can't work when a packet is fragmented.
        INT32 len = pPBuf->len;
        data = (INT8 *)pPBuf->payload;
        for (xmit = 0; len > 0; len -= xmit) {
            while ((xmit = _eth_fifoWrite(le0.txcurrent, data, (INT32)len)) < 0);
            if (xmit == len)
                flag |= FP0;
            /* Clear previous settings */
            le0.txcurrent->status &= ~(FP1 | FP0);
            le0.txcurrent->status |= (flag | ACT);
            flag = 0;
            le0.txcurrent = le0.txcurrent->next;
            data += xmit;
        }
        pPBuf = pPBuf->next;
    }
    le0.stat.tx_packets++;
    if (EDMAC.EDTRR.LONG == 0x00000000)
        EDMAC.EDTRR.LONG = 0x00000001;
    return ERR_OK;
}

void RX62N_EDMAC_LWIP_recv(struct netif *pNetIf)
{
    INT32 receivesize = 0;
    INT32 recvd;
    INT32 flag = 1;
    UINT32 readcount = 0;
    struct pbuf *pPBuf;

    while (flag) {
        recvd = _eth_fifoRead(le0.rxcurrent, (INT8 *)&tmpbuf);
        readcount++;
        if (readcount >= 2 && receivesize == 0)
            break;
        if (recvd == -1) {
            /* No descriptor to process */
        } else if (recvd == -2) {
            /* Frame error.  Point to next frame.  Clear this descriptor. */
            le0.stat.rx_errors++;
            receivesize = 0;
            le0.rxcurrent->status &= ~(FP1 | FP0 | FE);
            le0.rxcurrent->status &= ~(RFOVER | RAD | RMAF | RRF | RTLF | RTSF | PRE | CERF);
            le0.rxcurrent->status |= ACT;
            le0.rxcurrent = le0.rxcurrent->next;
            if (EDMAC.EDRRR.LONG == 0x00000000L)
                /* Restart if stopped */
                EDMAC.EDRRR.LONG = 0x00000001L;
        } else {
            /* We have a good buffer. */
            if ((le0.rxcurrent->status & FP1) == FP1) {
                /* Beginning of a frame */
                receivesize = 0;
            }
            if ((le0.rxcurrent->status & FP0) == FP0) {
                /* Frame is complete */
                le0.stat.rx_packets++;
                flag = 0;
            }
            pPBuf = pbuf_alloc(PBUF_RAW, recvd, PBUF_RAM);
            if (pPBuf == NULL) {
                debug_printf("Can't alloc pPBuf\r\n");
                break;
            }
            memcpy(pPBuf->payload, &tmpbuf, recvd);
            pNetIf->input(pPBuf, pNetIf);
            receivesize += recvd;
            le0.rxcurrent->status &= ~(FP1 | FP0);
            le0.rxcurrent->status |= ACT;
            le0.rxcurrent = le0.rxcurrent->next;
            //data += recvd;
            if (EDMAC.EDRRR.LONG == 0x00000000L)
                /* Restart if stopped */
                EDMAC.EDRRR.LONG = 0x00000001L;
        }
    }
#ifndef HW_INTERRUPT
    InterruptTaskCompletion.EnqueueDelta64(INT_PERIOD);
#endif
}

BOOL RX62N_EDMAC_LWIP_GetLinkStatus(void)
{
    return true;
        }

BOOL RX62N_EDMAC_LWIP_AutoNegotiate(void)
{
    return ENET_PHY_lwip_init();
    }

void RX62N_EDMAC_LWIP_interrupt(struct netif *pNetIf)
{
    GLOBAL_LOCK(encIrq);
    Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
    RX62N_EDMAC_LWIP_recv(pNetIf);
    //lwip_interrupt_continuation();
}

void RX62N_EDMAC_interrupt(void)
{
    struct netif *pNetIf;
    pNetIf = &g_RX62N_EDMAC_NetIF;
    RX62N_EDMAC_LWIP_interrupt(pNetIf);
}

#if defined(USE_ETHER_INT)
#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_ETHER_EINT(void)
{
    RX62N_EDMAC_interrupt();
}
#ifdef __cplusplus
}
#endif
#endif /* USE_ETHER_INT */
