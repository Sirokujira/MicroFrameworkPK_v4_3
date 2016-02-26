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
#include "..\iodefine.h"
#include "..\RX63N.h"

#ifdef  PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE
#define TX_USART_BUFFER_SIZE    PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE
#else
#define TX_USART_BUFFER_SIZE    512
#endif

#ifdef  PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE
#define RX_USART_BUFFER_SIZE    PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE
#else
#define RX_USART_BUFFER_SIZE    1024
#endif

#define SCR_CKE_M   (1 << 0)
#define SCR_TEIE_M  (1 << 2)
#define SCR_MPIE_M  (1 << 3)
#define SCR_RE_M    (1 << 4)
#define SCR_TE_M    (1 << 5)
#define SCR_RIE_M   (1 << 6)
#define SCR_TIE_M   (1 << 7)
#define USART_PRIORITY    12

extern UINT8 TxBuffer_Com[TX_USART_BUFFER_SIZE * TOTAL_USART_PORT];
extern UINT8 RxBuffer_Com[RX_USART_BUFFER_SIZE * TOTAL_USART_PORT];

extern struct HAL_USART_STATE Hal_Usart_State[TOTAL_USART_PORT];

#define STBCR4    *((volatile unsigned char *)(0xFFFE040C))

volatile struct st_sci0 *SCI[] = {
    (volatile struct st_sci0 *)0x8A000,  /* sci 0 */
    (volatile struct st_sci0 *)0x8A020,  /* sci 1 */
    (volatile struct st_sci0 *)0x8A040,  /* sci 2 */
    (volatile struct st_sci0 *)0x8A060,  /* sci 3 */
#if defined(SCI_MAX_8)
    (volatile struct st_sci0 *)0x8A080,  /* sci 4 */
    (volatile struct st_sci0 *)0x8A0A0,  /* sci 5 */
    (volatile struct st_sci0 *)0x8A0C0,  /* sci 6 */
    (volatile struct st_sci0 *)0x8A0E0,  /* sci 7 */
#endif
#if defined(SCI_MAX_12)
    (volatile struct st_sci0 *)0x8A100,  /* sci 8 */
    (volatile struct st_sci0 *)0x8A120,  /* sci 9 */
    (volatile struct st_sci0 *)0x8A140,  /* sci 10 */
    (volatile struct st_sci0 *)0x8A160,  /* sci 11 */
    (volatile struct st_sci0 *)0x8B300   /* sci 12 */
#endif
};

// rx, tx, cts = none, rts = none
UINT8 SCI_PINS[] = {
    RX63N_GPIO::P21,    // SCI0 RX
    RX63N_GPIO::P20,    // SCI0 TX
    RX63N_GPIO::P16,    // SCI1 RX
    RX63N_GPIO::P15,    // SCI1 TX
    RX63N_GPIO::P12,    // SCI2 RX
    RX63N_GPIO::P13,    // SCI2 TX
    RX63N_GPIO::P23,    // SCI3 RX
    RX63N_GPIO::P25     // SCI3 TX
};

//--//

BOOL CPU_USART_Initialize(int ComPortNum, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue)
{
    unsigned char scsmr = 0;
    volatile struct st_sci0 *sci = SCI[ComPortNum];

    if (Parity == USART_PARITY_NONE)
        scsmr = 0;
    else if (Parity == USART_PARITY_ODD)
        scsmr = (1 << 5)  | (1 << 4);
    else if (Parity == USART_PARITY_EVEN)
        scsmr = (1 << 5);
    else return FALSE;

    if (DataBits == 7)
        scsmr |= (1 << 6);
    if (StopBits == USART_STOP_BITS_TWO)
        scsmr |= (1 << 3);

    if (ComPortNum == 0) {
        SYSTEM.PRCR.WORD = 0xA502;
        MSTP_SCI0 = 0;
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        MPC.P20PFS.BYTE = 0x0A;
        MPC.P21PFS.BYTE = 0x0A;
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        PORT2.PMR.BIT.B0 = 1;
        PORT2.PMR.BIT.B1 = 1;
        IPR(SCI0, RXI0) = USART_PRIORITY;
        IPR(SCI0, TXI0) = USART_PRIORITY;
        IEN(SCI0, RXI0) = 1;
        IEN(SCI0, TXI0) = 1;
    } else if (ComPortNum == 1) {
        SYSTEM.PRCR.WORD = 0xA502;
        MSTP_SCI1 = 0;
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        MPC.P15PFS.BYTE = 0x0A;     // RXD1
        MPC.P16PFS.BYTE = 0x0A;     // TXD1
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        PORT1.PMR.BIT.B5 = 1;
        PORT1.PMR.BIT.B6 = 1;
        IPR(SCI1, RXI1) = USART_PRIORITY;
        IPR(SCI1, TXI1) = USART_PRIORITY;
        IEN(SCI1, RXI1) = 1;
        IEN(SCI1, TXI1) = 1;
    } else if (ComPortNum == 2) {
        SYSTEM.PRCR.WORD = 0xA502;
        MSTP_SCI2 = 0;
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        MPC.P12PFS.BYTE = 0x0A;     // RXD2
        MPC.P13PFS.BYTE = 0x0A;     // TDX2
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        PORT1.PMR.BIT.B2 = 1;
        PORT1.PMR.BIT.B3 = 1;
        IPR(SCI2, RXI2) = USART_PRIORITY;
        IPR(SCI2, TXI2) = USART_PRIORITY;
        IEN(SCI2, RXI2) = 1;
        IEN(SCI2, TXI2) = 1;
    } else if (ComPortNum == 3) {
        SYSTEM.PRCR.WORD = 0xA502;
        MSTP_SCI3 = 0;
        SYSTEM.PRCR.WORD = 0xA500;
        MPC.PWPR.BIT.B0WI = 0;      /* Enable write to PFSWE */
        MPC.PWPR.BIT.PFSWE = 1;     /* Enable write to PFS */
        MPC.P23PFS.BYTE = 0x0A;     // TXD3
        MPC.P25PFS.BYTE = 0x0A;     // RXD3
        MPC.PWPR.BYTE = 0x80;       /* Disable write to PFSWE and PFS*/
        PORT2.PMR.BIT.B3 = 1;
        PORT2.PMR.BIT.B5 = 1;
        IPR(SCI3, RXI3) = USART_PRIORITY;
        IPR(SCI3, TXI3) = USART_PRIORITY;
        IEN(SCI3, RXI3) = 1;
        IEN(SCI3, TXI3) = 1;
    } else
        return FALSE;
    sci->SCR.BYTE = 0;
    sci->SMR.BYTE = scsmr;
    sci->BRR = (48000000 / 32 / BaudRate) - 1;
    sci->SCR.BYTE = (SCR_RE_M | SCR_RIE_M);
    return TRUE;
}


BOOL CPU_USART_Uninitialize(int ComPortNum)
{
    volatile struct st_sci0 *sci = SCI[ComPortNum];
    sci->SCR.BYTE = 0;
    switch (ComPortNum)
    {
        case 0:
            SYSTEM.PRCR.WORD = 0xA502;
            MSTP_SCI0 = 0;
            SYSTEM.PRCR.WORD = 0xA500;
            IEN(SCI0, RXI0) = 0;
            IEN(SCI0, TXI0) = 0;
            break;
        case 1:
            SYSTEM.PRCR.WORD = 0xA502;
            MSTP_SCI1 = 0;
            SYSTEM.PRCR.WORD = 0xA500;
            IEN(SCI1, RXI1) = 0;
            IEN(SCI1, TXI1) = 0;
            break;
        case 2:
            SYSTEM.PRCR.WORD = 0xA502;
            MSTP_SCI2 = 0;
            SYSTEM.PRCR.WORD = 0xA500;
            IEN(SCI2, RXI2) = 0;
            IEN(SCI2, TXI2) = 0;
            break;
        case 3:
            SYSTEM.PRCR.WORD = 0xA502;
            MSTP_SCI3 = 0;
            SYSTEM.PRCR.WORD = 0xA500;
            IEN(SCI3, RXI3) = 0;
            IEN(SCI3, TXI3) = 0;
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

BOOL CPU_USART_TxBufferEmpty(int ComPortNum)
{
    return ((*SCI[ComPortNum]).SSR.BIT.TEND != 0);
}

BOOL CPU_USART_TxShiftRegisterEmpty(int ComPortNum)
{
    return CPU_USART_TxBufferEmpty(ComPortNum);
}

void CPU_USART_WriteCharToTxBuffer(int ComPortNum, UINT8 c)
{
    while (FALSE == CPU_USART_TxShiftRegisterEmpty(ComPortNum));
    (*SCI[ComPortNum]).TDR = c;
}

void CPU_USART_TxBufferEmptyInterruptEnable(int ComPortNum, BOOL Enable)
{
    while (FALSE == CPU_USART_TxShiftRegisterEmpty(ComPortNum));
    if (Enable)
        (*SCI[ComPortNum]).SCR.BYTE |= (SCR_TE_M | SCR_TIE_M);
    else
        (*SCI[ComPortNum]).SCR.BYTE &=  ~(SCR_TE_M | SCR_TIE_M);
#if 0
    UINT8 bit = Enable ? 1:0;
    switch (ComPortNum)
    {
        case 0:
            IEN(SCI0, TXI0) = bit;
            break;
        case 1:
            IEN(SCI1, TXI1) = bit;
            break;
        case 2:
            IEN(SCI2, TXI2) = bit;
            break;
        case 3:
            IEN(SCI3, TXI3) = bit;
            break;
    }
#endif
}

BOOL CPU_USART_TxBufferEmptyInterruptState(int ComPortNum)
{
    return ((*SCI[ComPortNum]).SCR.BIT.TIE != 0);
}

void CPU_USART_RxBufferFullInterruptEnable(int ComPortNum, BOOL Enable)
{
    if (Enable)
        (*SCI[ComPortNum]).SCR.BIT.RIE = 1;
    else
        (*SCI[ComPortNum]).SCR.BIT.RIE = 0;
#if 0
    UINT8 bit = Enable ? 1:0;
    switch (ComPortNum)
    {
        case 0:
            IEN(SCI0, RXI0) = bit;
            break;
        case 1:
            IEN(SCI1, RXI1) = bit;
            break;
        case 2:
            IEN(SCI2, RXI2) = bit;
            break;
        case 3:
            IEN(SCI3, RXI3) = bit;
            break;
    }
#endif
}

BOOL CPU_USART_RxBufferFullInterruptState(int ComPortNum)
{
    return ((*SCI[ComPortNum]).SCR.BIT.RIE != 0);
}

void CPU_USART_ProtectPins(int ComPortNum, BOOL On)
{
    // ToDo
}

UINT32 CPU_USART_PortsCount()
{
    return TOTAL_USART_PORT;
}

void CPU_USART_GetPins(int ComPortNum, GPIO_PIN& rxPin, GPIO_PIN& txPin, GPIO_PIN& ctsPin, GPIO_PIN& rtsPin)
{
    rxPin = (GPIO_PIN)SCI_PINS[ComPortNum*2];
    txPin = (GPIO_PIN)SCI_PINS[ComPortNum*2+1];
    ctsPin= GPIO_PIN_NONE;
    rtsPin= GPIO_PIN_NONE;
    return;
}

BOOL CPU_USART_SupportNonStandardBaudRate (int ComPortNum)
{
    // ToDO
    return FALSE;
}

void CPU_USART_GetBaudrateBoundary(int ComPortNum, UINT32& maxBaudrateHz, UINT32& minBaudrateHz)
{
    maxBaudrateHz = 115200;
    minBaudrateHz = 2400;
}

BOOL CPU_USART_IsBaudrateSupported(int ComPortNum, UINT32 & BaudrateHz)
{
    // ToDO
    return FALSE;
}

BOOL CPU_USART_TxHandshakeEnabledState(int comPort)
{
    // ToDo
    return TRUE;
}

static void USART_RxISR(UINT32 port)
{
    char c;
    GLOBAL_LOCK(irq);
    c = (*SCI[port]).RDR;
    USART_AddCharToRxBuffer(port, c);
}

static void USART_TxISR(UINT32 port)
{
    char c;
    GLOBAL_LOCK(irq);
    if (USART_RemoveCharFromTxBuffer(port, c)) {
        CPU_USART_WriteCharToTxBuffer(port, c);
        Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
    } else {
        CPU_USART_TxBufferEmptyInterruptEnable(port, FALSE);
    }
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_SCI0_RXI0(void) { USART_RxISR(0);}
void __attribute__ ((interrupt)) INT_Excep_SCI1_RXI1(void) { USART_RxISR(1);}
void __attribute__ ((interrupt)) INT_Excep_SCI2_RXI2(void) { USART_RxISR(2);}
void __attribute__ ((interrupt)) INT_Excep_SCI3_RXI3(void) { USART_RxISR(3);}
#if defined(SCI_MAX_8)
void __attribute__ ((interrupt)) INT_Excep_SCI4_RXI4(void) { USART_RxISR(4);}
void __attribute__ ((interrupt)) INT_Excep_SCI5_RXI5(void) { USART_RxISR(5);}
void __attribute__ ((interrupt)) INT_Excep_SCI6_RXI6(void) { USART_RxISR(6);}
void __attribute__ ((interrupt)) INT_Excep_SCI7_RXI7(void) { USART_RxISR(7);}
#endif
void __attribute__ ((interrupt)) INT_Excep_SCI0_TXI0(void) { USART_TxISR(0);}
void __attribute__ ((interrupt)) INT_Excep_SCI1_TXI1(void) { USART_TxISR(1);}
void __attribute__ ((interrupt)) INT_Excep_SCI2_TXI2(void) { USART_TxISR(2);}
void __attribute__ ((interrupt)) INT_Excep_SCI3_TXI3(void) { USART_TxISR(3);}
#if defined(SCI_MAX_8)
void __attribute__ ((interrupt)) INT_Excep_SCI4_TXI4(void) { USART_TxISR(4);}
void __attribute__ ((interrupt)) INT_Excep_SCI5_TXI5(void) { USART_TxISR(5);}
void __attribute__ ((interrupt)) INT_Excep_SCI6_TXI6(void) { USART_TxISR(6);}
void __attribute__ ((interrupt)) INT_Excep_SCI7_TXI7(void) { USART_TxISR(7);}
#endif
#ifdef __cplusplus
}
#endif
