////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_ToppersFMPonZYBO_SELECTOR_H_
#define _PLATFORM_ToppersFMPonZYBO_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//
#if defined(PLATFORM_ARM_ToppersFMPonZYBO)
#define HAL_SYSTEM_NAME                     "ToppersFMPonZYBO"

#define PLATFORM_ARM_MC9328                1
#define PLATFORM_ARM_MC9328MXS             1

//--//


#define HARDWARE_BOARD_i_MXS_DEMO_REV_V1_2 3
#define HARDWARE_BOARD_i_MXS_DEMO_REV_V1_3 4

#define HARDWARE_BOARD_TYPE                HARDWARE_BOARD_i_MXS_DEMO_REV_V1_3

//
// processor and features
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// constants
//
#define PLATFORM_SUPPORTS_SOFT_REBOOT   TRUE

#define SYSTEM_CLOCK_HZ                 30000000
#define CLOCK_COMMON_FACTOR             1000000
#define SLOW_CLOCKS_PER_SECOND          16000000
#define SLOW_CLOCKS_TEN_MHZ_GCD         2000000
#define SLOW_CLOCKS_MILLISECOND_GCD     1000

#define SRAM1_MEMORY_Base   0x08000000
#define SRAM1_MEMORY_Size   0x02000000

#define FLASH_MEMORY_Base   0x10000000
#define FLASH_MEMORY_Size   0x01000000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define GLOBAL_LOCK(x)                  SmartPtr_IRQ x
#define DISABLE_INTERRUPTS()            SmartPtr_IRQ::ForceDisabled()
#define ENABLE_INTERRUPTS()             SmartPtr_IRQ::ForceEnabled()
#define INTERRUPTS_ENABLED_STATE()      SmartPtr_IRQ::GetState()
#define GLOBAL_LOCK_SOCKETS(x)          SmartPtr_IRQ x

#if defined(_DEBUG)
#define ASSERT_IRQ_MUST_BE_OFF()        ASSERT(!SmartPtr_IRQ::GetState())
#define ASSERT_IRQ_MUST_BE_ON()         ASSERT( SmartPtr_IRQ::GetState())
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#endif

#if 0
    #define DEBUG_TEXT_PORT    USB1
    #define STDIO              USB1
    #define DEBUGGER_PORT      USB1
    #define MESSAGING_PORT     USB1
#elif 0
    #define DEBUG_TEXT_PORT    USB1
    #define STDIO              USB1
    #define DEBUGGER_PORT      USB1
    #define MESSAGING_PORT     USB1
#else
    #define DEBUG_TEXT_PORT    USB1
    #define STDIO              USB1
    #define DEBUGGER_PORT      USB1
    #define MESSAGING_PORT     USB1
#endif
//
// constants
/////////////////////////////////////////////////////////

#include <processor_selector.h>

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

// #define TOTAL_SOCK_PORT        0

#define TOTAL_DEBUG_PORT       1
#define COM_DEBUG              ConvertCOM_DebugHandle(0)

#define COM_MESSAGING          ConvertCOM_MessagingHandle(0)

#define USART_TX_IRQ_INDEX(x)       ( (x) ? 0 : 0 )     /* TODO set right indexes */
#define USART_DEFAULT_PORT          COM1
#define USART_DEFAULT_BAUDRATE      115200
//#define USART_DEFAULT_BAUDRATE      9600

#define USB_IRQ_INDEX               0  // TODO set right index

// #define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE    512  // there is one TX for each usart port
// #define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE    512  // there is one RX for each usart port
// #define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT  2    // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes


#endif // PLATFORM_ARM_ToppersFMPonZYBO

#endif // _PLATFORM_ToppersFMPonZYBO_SELECTOR_H_