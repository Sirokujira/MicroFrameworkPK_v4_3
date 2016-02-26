////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for the MountaineerEth board (STM32F4): Copyright (c) Oberon microsystems, Inc.
//
//  *** MountaineerEth Ethernet Configuration ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include <STM32F4_ETH_lwip_adapter.h>

BOOL Network_Interface_Bind(int index)
{
    return STM32F4_ETH_LWIP_Driver::Bind();
}

int  Network_Interface_Open(int index)
{
    return STM32F4_ETH_LWIP_Driver::Open(index);
}

BOOL Network_Interface_Close(int index)
{
    return STM32F4_ETH_LWIP_Driver::Close();
}
