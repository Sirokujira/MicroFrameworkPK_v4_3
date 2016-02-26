////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_EddyDK21_linux_SELECTOR_H_
#define _PLATFORM_EddyDK21_linux_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_EddyDK21_linux)
#define HAL_SYSTEM_NAME                     "EddyDK21_linux"
#define PLATFORM_LINUX				1
#define PLATFORM_EDDY_LINUX					1
//
// processor and features
//
/////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//
//  EddyDK21_linux SOCKETS ENABLED PLATFORM -- TODO TODO TODO: REMOVE WHEN PLATFORM BUILDER AVAILABLE
//

#define NETWORK_INTERFACE_COUNT             1

#define PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH 8192

#define NETWORK_MEMORY_PROFILE__medium      1

#define NETWORK_MEMORY_POOL__INCLUDE_SSL    1
#include <pal\net\Network_Defines.h>

//#define NETWORK_USE_LOOPBACK                1
//#define NETWORK_USE_DHCP                    1

//--//

#define PLATFORM_DEPENDENT__NETWORK_DNS_MIN_DELAY  4 
#define PLATFORM_DEPENDENT__NETWORK_DNS_MAX_DELAY  32
#define PLATFORM_DEPENDENT__NETWORK_DNS_RETRIES    3

//
//  IMXS SOCKETS ENABLED PLATFORM -- TODO TODO TODO: REMOVE WHEN PLATFORM BUILDER AVAILABLE
//
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// constants
//

#define SYSTEM_CLOCK_HZ                 1000000000 // 1GHz, 1ns -- from linux Timeresolution:
#define SYSTEM_CYCLE_CLOCK_HZ           1000000    // 1MHz, 1us
#define CLOCK_COMMON_FACTOR             1000000    // 1MHz, 1us
#define SLOW_CLOCKS_PER_SECOND          1000000    // 1MHz, 1us

#define SLOW_CLOCKS_TEN_MHZ_GCD         1000
#define SLOW_CLOCKS_MILLISECOND_GCD     1000

//#define SRAM1_MEMORY_Base   0x00001000 // not used, but needed for stubs
//#define SRAM1_MEMORY_Size   0x00001000 // not used, but needed for stubs
#define FLASH_MEMORY_Base   0x10000000 // not used, but needed for stubs
#define FLASH_MEMORY_Size   0x01000000 // not used, but needed for stubs

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define INSTRUMENTATION_H_GPIO_PIN      0

#define DEBUG_TEXT_PORT    COM1
#define STDIO              COM1
#define DEBUGGER_PORT      COM1
#define MESSAGING_PORT     COM1

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
#define DEBUG_PRINTF(fmt, args...) debug_printf(fmt, ## args)
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#define DEBUG_PRINTF(fmt, args...)
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
//#define TOTAL_USART_PORT       2
#define TOTAL_USART_PORT       1
#define COM1                   ConvertCOM_ComHandle(0)
#define COM2                   ConvertCOM_ComHandle(1)

//#define TOTAL_USB_CONTROLLER   1
#define TOTAL_USB_CONTROLLER   0
#define USB1                   ConvertCOM_UsbHandle(0)

#define TOTAL_SOCK_PORT        0

#define TOTAL_DEBUG_PORT       1
#define COM_DEBUG              ConvertCOM_DebugHandle(0)

#define COM_MESSAGING          ConvertCOM_MessagingHandle(0)

#define USART_TX_IRQ_INDEX(x)       ( (x) ? 0 : 0 )     /* TODO set right indexes */
#define USART_DEFAULT_PORT          COM1
#define USART_DEFAULT_BAUDRATE      115200
//#define USART_DEFAULT_BAUDRATE      9600

#define USB_IRQ_INDEX               0  // TODO set right index

#define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE    512  // there is one TX for each usart port
#define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE    512  // there is one RX for each usart port
#define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT  2    // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes
//
// communicaiton facilities
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_EddyDK21_linux

#endif // _PLATFORM_EddyDK21_linux_SELECTOR_H_
