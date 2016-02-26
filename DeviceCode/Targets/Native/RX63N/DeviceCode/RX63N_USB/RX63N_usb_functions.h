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

#ifndef RX63N_USB_FUNCTIONS_H_
#define RX63N_USB_FUNCTIONS_H_

#define USB_EP_TX_DIS       0
#define USB_EP_TX_STALL     1
#define USB_EP_TX_NAK       2
#define USB_EP_TX_VALID     3
#define USB_EP_RX_DIS       0
#define USB_EP_RX_STALL     1
#define USB_EP_RX_NAK       2
#define USB_EP_RX_VALID     3

#define USB_MAX_EP      6
#define MAX_EP0_SIZE    64
#define USB_MAX_DATA_PACKET_SIZE    64

#endif /* RX63N_USB_FUNCTIONS_H_ */
