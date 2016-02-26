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
//  Copyright (c) Kentaro Sekimoto All rights reserved.
//
////////////////////////////////////////////////////////////////////////////

#ifndef USB_DEBUG_PRINTF_H_
#define USB_DEBUG_PRINTF_H_

//#define USB_DEBUG
void usb_debug_printf_init();
int usb_debug_printf(char* format, ...);
void usb_memory_dump(unsigned long addr, int len);

#ifdef USB_DEBUG
#define USB_DEBUG_INIT              usb_debug_printf_init
#define USB_DEBUG1(p0)              usb_debug_printf((char *)p0)
#define USB_DEBUG2(p0, p1)          usb_debug_printf((char *)p0, p1)
#define USB_DEBUG3(p0, p1, p2)      usb_debug_printf((char *)p0, p1, p2)
#define USB_DEBUG4(p0, p1, p2, p3)  usb_debug_printf((char *)p0, p1, p2, p3)
#define USB_DEBUG5(p0, p1, p2, p3, p4)  usb_debug_printf((char *)p0, p1, p2, p3, p4)
#define USB_DEBUG6(p0, p1, p2, p3, p4, p5)  usb_debug_printf((char *)p0, p1, p2, p3, p4, p5)
#define USB_DEBUG7(p0, p1, p2, p3, p4, p5, p6)  usb_debug_printf((char *)p0, p1, p2, p3, p4, p5, p6)
//#define USB_DEBUG_INIT()
//#define USB_DEBUG1(p0)              lcd_printf((char *)p0)
//#define USB_DEBUG2(p0, p1)          lcd_printf((char *)p0, p1)
//#define USB_DEBUG3(p0, p1, p2)      lcd_printf((char *)p0, p1, p2)
//#define USB_DEBUG4(p0, p1, p2, p3)  lcd_printf((char *)p0, p1, p2, p3)
#else
#define USB_DEBUG_INIT()
#define USB_DEBUG1(p0)
#define USB_DEBUG2(p0, p1)
#define USB_DEBUG3(p0, p1, p2)
#define USB_DEBUG4(p0, p1, p2, p3)
#define USB_DEBUG5(p0, p1, p2, p3, p4)
#define USB_DEBUG6(p0, p1, p2, p3, p4, p5)
#define USB_DEBUG7(p0, p1, p2, p3, p4, p5, p6)
#endif

#endif /* USB_DEBUG_PRINTF_H_ */
