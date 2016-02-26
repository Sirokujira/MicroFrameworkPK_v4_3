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

#ifndef _RX62N_EDMAC_LWIP_ADAPTER_H_1
#define _RX62N_EDMAC_LWIP_ADAPTER_H_1 1

#include "tinyhal.h" 
#include "net_decl_lwip.h"
#include "lwip\dhcp.h"

#define RX62N_EDMAC_MAX_FRAME_SIZE 1536
#define UPTIME_PERIOD   1000000         // 1sec
#define INT_PERIOD      5000            // 5ms


struct RX62N_EDMAC_LWIP_DRIVER_CONFIG
{
    GPIO_PIN    PHY_PD_GPIO_Pin;    // phy power down control
};

struct RX62N_EDMAC_LWIP_Driver
{
//    struct dhcp m_currentDhcpSession;
    static int Open(int);
    static BOOL Close(void);
    static BOOL Bind(void);
};

#endif /* _RX62N_EDMAC_LWIP_ADAPTER_H_ */
