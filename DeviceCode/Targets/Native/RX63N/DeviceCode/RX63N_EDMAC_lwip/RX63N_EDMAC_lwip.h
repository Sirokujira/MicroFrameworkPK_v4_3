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

#ifndef _RX63N_EDMAC_LWIP_H_
#define _RX63N_EDMAC_LWIP_H_

#include <tinyhal.h>
#include "lwip\err.h"
#include "RX63N_EDMAC.h"

#define ETH_RMII_MODE   0
#define ETH_MII_MODE    1
#define ETH_MODE_SEL    ETH_RMII_MODE
#define BUFSIZE         1536
#define ENTRY           3

extern NETWORK_CONFIG g_NetworkConfig;
extern BOOL ENET_PHY_lwip_init(void);

BOOL RX63N_EDMAC_lwip_WritePhy(UINT32 phy_addr, UINT32 reg_addr, UINT32 data, UINT32 retry);
BOOL RX63N_EDMAC_lwip_ReadPhy(UINT32 phy_addr, UINT32 reg_addr, UINT32 *data, UINT32 retry);
void RX63N_EDMAC_lwip_SetLinkSpeed(BOOL speed, BOOL fullduplex);

BOOL RX63N_EDMAC_LWIP_Init(struct netif *pNetIf);
BOOL RX63N_EDMAC_LWIP_open(struct netif *pNetIf);
void RX63N_EDMAC_LWIP_close(struct netif *pNetIf);
void RX63N_EDMAC_LWIP_interrupt(struct netif *pNetIf);
err_t RX63N_EDMAC_LWIP_xmit(struct netif *pNetIf, struct pbuf *pPBuf);
void RX63N_EDMAC_LWIP_recv (struct netif *pNetIf);
void RX63N_EDMAC_interrupt(void *param);
BOOL RX63N_EDMAC_LWIP_GetLinkStatus(void);
BOOL RX63N_EDMAC_LWIP_AutoNegotiate(void);

#endif /* _RX63N_EDMAC_LWIP_H_ */
