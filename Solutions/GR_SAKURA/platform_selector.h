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

#ifndef _PLATFORM_GR_SAKURA_SELECTOR_H_
#define _PLATFORM_GR_SAKURA_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_RX63N_GR_SAKURA)
#define R5F563NB
//#define HAL_REDUCESIZE
#define HAL_SYSTEM_NAME                     "GR_SAKURA"

//
// processor and features
//
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
//
// constants
//

// CLKDEV(CKS) = 8, 32, 128, 512 -> Needs to change RX63N_TIMER.CPP
// IF SLOW CLOCK = PCLK/8   = 6000000 (ticks/s) -> 0xFFFFFFFF ticks =  715  sec = 11 minutes
// IF SLOW CLOCK = PCLK/32  = 1500000 (ticks/s) -> 0xFFFFFFFF ticks =  2863 sec = 47 minutes
// IF SLOW CLOCK = PCLK/128 =  375000 (ticks/s) -> 0xFFFFFFFF ticks = 11453 sec =  3 hours
// IF SLOW CLOCK = PCLK/512 =   93750 (ticks/s) -> 0xFFFFFFFF ticks = 45812 sec = 12 hours
// 32bit MAX 0xFFFFFFFF = 4294967295
// 64bit MAX 0xFFFFFFFFFFFFFFFF = 18446744073709551615

#define PCLK                            48000000
#define CLKDEV                          512                // CKS
//#define CLKDEV                        128                // CKS
#define SYSTEM_CYCLE_CLOCK_HZ           96000000        // ICLK
#define SYSTEM_CLOCK_HZ                 96000000        // ICLK
#define CLOCK_COMMON_FACTOR             100000
#define SLOW_CLOCKS_PER_SECOND          (PCLK/CLKDEV)
#define SLOW_CLOCKS_TEN_MHZ_GCD         1000
#define SLOW_CLOCKS_MILLISECOND_GCD     100

#define SRAM1_MEMORY_Base   0x00000000
#define SRAM1_MEMORY_Size   0x00020000
//#define SRAM1_MEMORY_Base   0x04000000
//#define SRAM1_MEMORY_Size   0x00200000
#define FLASH_MEMORY_Base   0xFFF00000
#define FLASH_MEMORY_Size   0x00100000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define INSTRUMENTATION_H_GPIO_PIN      0

#if 1
//#define DEBUG_TEXT_PORT    COM1     // for debugging
#define DEBUG_TEXT_PORT    USB1
#define STDIO              USB1
#define DEBUGGER_PORT      USB1
#define MESSAGING_PORT     USB1
#else
#define DEBUG_TEXT_PORT    COM1
#define STDIO              COM1
#define DEBUGGER_PORT      COM1
#define MESSAGING_PORT     COM1
#endif

//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// macros
//

#define GLOBAL_LOCK(x)             SmartPtr_IRQ x
#define DISABLE_INTERRUPTS()       SmartPtr_IRQ::ForceDisabled()
#define ENABLE_INTERRUPTS()        SmartPtr_IRQ::ForceEnabled()
#define INTERRUPTS_ENABLED_STATE() SmartPtr_IRQ::GetState()
#define GLOBAL_LOCK_SOCKETS(x)     SmartPtr_IRQ x

#if defined(_DEBUG)
#define ASSERT_IRQ_MUST_BE_OFF()   ASSERT(!SmartPtr_IRQ::GetState())
#define ASSERT_IRQ_MUST_BE_ON()    ASSERT( SmartPtr_IRQ::GetState())
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#endif


//
// macros
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// communicaiton facilities
//

// Port definitions
#define TOTAL_USART_PORT       1
#define COM1                   ConvertCOM_ComHandle(0)
#define COM2                   ConvertCOM_ComHandle(1)

#define TOTAL_USB_CONTROLLER   1
#define USB1                   ConvertCOM_UsbHandle(0)

#define TOTAL_SOCK_PORT        0

#define TOTAL_DEBUG_PORT       1
#define COM_DEBUG              ConvertCOM_DebugHandle(0)

#define COM_MESSAGING          ConvertCOM_MessagingHandle(0)

#define USART_TX_IRQ_INDEX(x)       ( (x) ? 0 : 0 )     /* TODO set right indexes */
#define USART_DEFAULT_PORT          COM1
#define USART_DEFAULT_BAUDRATE      115200

#define USB_IRQ_INDEX               0  // TODO set right index

#define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE    512  // there is one TX for each usart port
#define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE    512  // there is one RX for each usart port
#define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT  2    // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes
//
// communicaiton facilities
/////////////////////////////////////////////////////////

#define GPIO_PIN_NONE                   0xFFFFFFFF
//#define DRIVER_PAL_BUTTON_MAPPING                                                  \
//    { 8*6+7, BUTTON_B0 }, /* RX63N_GPIO::P67 */ \
//    { 8*6+6, BUTTON_B1 }, /* RX63N_GPIO::P66 */ \
//    { 8*4+5, BUTTON_B2 }, /* RX63N_GPIO::P45 */ \
//    { 8*4+4, BUTTON_B3 }, /* RX63N_GPIO::P44 */ \
//    { 8*4+7, BUTTON_B4 }, /* RX63N_GPIO::P47 */ \
//    { 8*4+6, BUTTON_B5 }, /* RX63N_GPIO::P46 */

//
// CLR runtime memory configuration
//
#define RUNTIME_MEMORY_PROFILE__extrasmall 1

#define NETWORK_INTERFACE_COUNT            1
#define NETWORK_MEMORY_PROFILE__small        1

#include <processor_selector.h>

#endif // PLATFORM_RX63N_GR_SAKURA

#endif // _PLATFORM_GR_SAKURA_SELECTOR_H_
