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

#ifndef RX63N_EDMAC_H_
#define RX63N_EDMAC_H_

#define ACT     0x80000000
#define DL      0x40000000
#define FP1     0x20000000
#define FP0     0x10000000
#define FE      0x08000000

#define RFOVER  0x00000200
#define RAD     0x00000100
#define RMAF    0x00000080
#define RRF     0x00000010
#define RTLF    0x00000008
#define RTSF    0x00000004
#define PRE     0x00000002
#define CERF    0x00000001

#define TAD     0x00000100
#define CND     0x00000008
#define DLC     0x00000004
#define CD      0x00000002
#define TRO     0x00000001

struct Descriptor
{
    // little endian
    UINT32  status;
    UINT16  size;
    UINT16  bufsize;
    UINT8   *buf_p;
    struct Descriptor   *next;
};

typedef struct Descriptor ethfifo;

struct enet_stats
{
  UINT32  rx_packets;       /* total packets received    */
  UINT32  tx_packets;       /* total packets transmitted  */
  UINT32  rx_errors;        /* bad packets received      */
  UINT32  tx_errors;        /* packet transmit problems    */
  UINT32  rx_dropped;       /* no space in buffers      */
  UINT32  tx_dropped;       /* no space available      */
  UINT32  multicast;        /* multicast packets received  */
  UINT32  collisions;

  /* detailed rx_errors: */
  UINT32  rx_length_errors;
  UINT32  rx_over_errors;   /* receiver ring buffer overflow  */
  UINT32  rx_crc_errors;    /* recved pkt with crc error  */
  UINT32  rx_frame_errors;  /* recv'd frame alignment error  */
  UINT32  rx_fifo_errors;   /* recv'r fifo overrun      */
  UINT32  rx_missed_errors; /* receiver missed packet    */

  /* detailed tx_errors */
  UINT32  tx_aborted_errors;
  UINT32  tx_carrier_errors;
  UINT32  tx_fifo_errors;
  UINT32  tx_heartbeat_errors;
  UINT32  tx_window_errors;
};

struct ei_device
{
  const INT8 *name;
  UINT8 open;
  UINT8 Tx_act;
  UINT8 Rx_act;
  UINT8 txing;              /* Transmit Active */
  UINT8 irqlock;            /* EDMAC's interrupt disabled when '1'. */
  UINT8 dmaing;             /* EDMAC Active */
  ethfifo *rxcurrent;       /* current receive discripter */
  ethfifo *txcurrent;       /* current transmit discripter */
  UINT8 save_irq;           /* Original dev->irq value. */
  struct enet_stats stat;
  UINT8 mac_addr[6];
};

/* Standard PHY Registers */
#define BASIC_MODE_CONTROL_REG      0
#define BASIC_MODE_STATUS_REG       1
#define PHY_IDENTIFIER1_REG         2
#define PHY_IDENTIFIER2_REG         3
#define AN_ADVERTISEMENT_REG        4
#define AN_LINK_PARTNER_ABILITY_REG 5
#define AN_EXPANSION_REG            6

/* Media Independent Interface */
#define PHY_ST      1
#define PHY_READ    2
#define PHY_WRITE   1
//#define  PHY_ADDR  0x1F

#define MDC_WAIT    2

/* PHY return definitions */
#define R_PHY_OK    0
#define R_PHY_ERROR -1

/* Auto-Negotiation Link Partner Status */
#define PHY_AN_LINK_PARTNER_100BASE 0x0180
#define PHY_AN_LINK_PARTNER_FULL    0x0140

#define PHY_AN_LINK_PARTNER_100FULL 0x0100
#define PHY_AN_LINK_PARTNER_100HALF 0x0080
#define PHY_AN_LINK_PARTNER_10FULL  0x0040
#define PHY_AN_LINK_PARTNER_10HALF  0x0020

//#define PHY_RESET_WAIT            0x00020000L
//#define PHY_AUTO_NEGOTIATON_WAIT  0x00800000L
#define PHY_RESET_WAIT              0x00002000L
#define PHY_AUTO_NEGOTIATON_WAIT    0x00080000L

#endif /* RX63N_EDMAC_H_ */
