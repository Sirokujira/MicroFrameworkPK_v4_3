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

#include <tinyhal.h>
#include "net_decl_lwip.h"
#include "lwip\netif.h"
#include "lwip\tcp.h"
#include "lwip\tcpip.h"
#include "lwip\dhcp.h"
#include "lwip\pbuf.h"
#include "netif\etharp.h"

#include "RX62N_EDMAC_lwip.h"
#include "RX62N_EDMAC_lwip_Adapter.h"

#define DEBUG_EDMAC

struct netif g_RX62N_EDMAC_NetIF;

#ifndef HW_INTERRUPT
HAL_COMPLETION InterruptTaskCompletion;
#endif
HAL_COMPLETION LwipUpTimeCompletion;
static BOOL LwipNetworkStatus = 0;
static UINT32 LwipLastIpAddress = 0;

void RX62N_EDMAC__status_callback(struct netif *netif)
{
    if (LwipLastIpAddress != netif->ip_addr.addr) {
        Network_PostEvent(NETWORK_EVENT_TYPE_ADDRESS_CHANGED, 0);
        LwipLastIpAddress = netif->ip_addr.addr;
    }
#if defined(DEBUG)
    hal_printf("Link Update: \r\n");
    hal_printf(" IP: %d.%d.%d.%d\r\n",
        (netif->ip_addr.addr >>  0) & 0xFF,
        (netif->ip_addr.addr >>  8) & 0xFF,
        (netif->ip_addr.addr >> 16) & 0xFF,
        (netif->ip_addr.addr >> 24) & 0xFF);
    hal_printf(" GW: %d.%d.%d.%d\r\n",
        (netif->gw.addr >>  0) & 0xFF,
        (netif->gw.addr >>  8) & 0xFF,
        (netif->gw.addr >> 16) & 0xFF,
        (netif->gw.addr >> 24) & 0xFF);
#endif
}

err_t RX62N_EDMAC_ethhw_init(struct netif *myNetIf)
{ 
    myNetIf->flags = NETIF_FLAG_IGMP | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET; // Added in 4.3.1
    myNetIf->mtu = RX62N_EDMAC_MAX_FRAME_SIZE;
    myNetIf->output = etharp_output;
    myNetIf->linkoutput = RX62N_EDMAC_LWIP_xmit;
    myNetIf->status_callback = RX62N_EDMAC__status_callback;
    RX62N_EDMAC_LWIP_open(myNetIf);
    return (err_t)ERR_OK;
}

void lwip_network_uptime_completion(void *arg)
{
    NATIVE_PROFILE_PAL_NETWORK();

    BOOL status = RX62N_EDMAC_LWIP_GetLinkStatus();
    if (status != LwipNetworkStatus) {
        struct netif* pNetIf = (struct netif*)arg;
        if (status) {
            SOCK_NetworkConfiguration *pNetCfg = &g_NetworkConfig.NetworkInterfaces[0];
            RX62N_EDMAC_LWIP_AutoNegotiate();
            netif_set_up(pNetIf);

            if (pNetCfg->flags & SOCK_NETWORKCONFIGURATION_FLAGS_DHCP)
                dhcp_start(pNetIf);
            Network_PostEvent(NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, NETWORK_EVENT_FLAGS_IS_AVAILABLE);
        } else {
            netif_set_down((struct netif*)arg);
            Network_PostEvent(NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, 0);
        }
        LwipNetworkStatus = status;
    }
    LwipUpTimeCompletion.EnqueueDelta64(UPTIME_PERIOD);
}

void InitCompletions(struct netif *pNetIf)
{
#ifndef HW_INTERRUPT
    InterruptTaskCompletion.InitializeForISR((HAL_CALLBACK_FPN)RX62N_EDMAC_LWIP_recv, pNetIf);
    InterruptTaskCompletion.EnqueueDelta64(INT_PERIOD);
#endif
    LwipUpTimeCompletion.InitializeForUserMode((HAL_CALLBACK_FPN)lwip_network_uptime_completion, pNetIf);
    LwipUpTimeCompletion.EnqueueDelta64(UPTIME_PERIOD);
}

BOOL Network_Interface_Bind(int index)
{
    return RX62N_EDMAC_LWIP_Driver::Bind();
}

int  Network_Interface_Open(int index)
{
    return RX62N_EDMAC_LWIP_Driver::Open(index);
}

BOOL Network_Interface_Close(int index)
{
    return RX62N_EDMAC_LWIP_Driver::Close();
}

int RX62N_EDMAC_LWIP_Driver::Open(int index)
{
    struct ip_addr ipaddr, subnetmask, gateway;
    struct netif *pNetIF;
    int len;
    const SOCK_NetworkConfiguration *iface;

    iface = &g_NetworkConfig.NetworkInterfaces[index];
    len = g_RX62N_EDMAC_NetIF.hwaddr_len;
    if (len == 0 || iface->macAddressLen < len) {
        len = iface->macAddressLen;
        g_RX62N_EDMAC_NetIF.hwaddr_len = len;
    }
    memcpy(g_RX62N_EDMAC_NetIF.hwaddr, iface->macAddressBuffer, len);
    ipaddr.addr     = iface->ipaddr;
    gateway.addr    = iface->gateway;
    subnetmask.addr = iface->subnetmask;

    pNetIF = netif_add(&g_RX62N_EDMAC_NetIF, &ipaddr, &subnetmask, &gateway, NULL, RX62N_EDMAC_ethhw_init, ethernet_input);

    netif_set_default(pNetIF);
    LwipNetworkStatus = RX62N_EDMAC_LWIP_GetLinkStatus();
    if (LwipNetworkStatus)
        netif_set_up(pNetIF);
    InitCompletions(pNetIF);
    return g_RX62N_EDMAC_NetIF.num;
}

BOOL RX62N_EDMAC_LWIP_Driver::Close(void)
{
    int retVal = -1;

    LwipUpTimeCompletion.Abort();
#ifndef HW_INTERRUPT
    InterruptTaskCompletion.Abort();
#endif
    LwipNetworkStatus = 0;
    netif_set_down(&g_RX62N_EDMAC_NetIF);
    netif_remove(&g_RX62N_EDMAC_NetIF);
    RX62N_EDMAC_LWIP_close(&g_RX62N_EDMAC_NetIF);
    memset(&g_RX62N_EDMAC_NetIF, 0, sizeof g_RX62N_EDMAC_NetIF);
    return true;
}

BOOL RX62N_EDMAC_LWIP_Driver::Bind(void)
{
    return true;
}

BOOL RX62N_EMAC_LWIP_GetLinkStatus(void)
{
    return true;
}
