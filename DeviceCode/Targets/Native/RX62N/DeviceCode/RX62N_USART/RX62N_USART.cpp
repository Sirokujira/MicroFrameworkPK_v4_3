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
#include "..\RX62N.h"

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

#define USART_PRIORITY    12

extern UINT8 TxBuffer_Com[TX_USART_BUFFER_SIZE * TOTAL_USART_PORT];
extern UINT8 RxBuffer_Com[RX_USART_BUFFER_SIZE * TOTAL_USART_PORT];

extern struct HAL_USART_STATE Hal_Usart_State[TOTAL_USART_PORT];

#define STBCR4    *((volatile unsigned char *)(0xFFFE040C))

volatile struct st_sci* SCI[] =
{
    (volatile struct st_sci*)0x88240,
    (volatile struct st_sci*)0x88248,
    (volatile struct st_sci*)0x88250,
    (volatile struct st_sci*)0x88258
};

// rx, tx, cts = none, rts = none
UINT8 SCI_PINS[8] = {
    RX62N_GPIO::P21,    // SCI0 RX
    RX62N_GPIO::P20,    // SCI0 TX
    RX62N_GPIO::P30,    // SCI1 RX
    RX62N_GPIO::P26,    // SCI1 TX
    RX62N_GPIO::P12,    // SCI2 RX
    RX62N_GPIO::P13,    // SCI2 TX
    RX62N_GPIO::P25,    // SCI3 RX
    RX62N_GPIO::P23     // SCI3 TX
};

//--//

BOOL CPU_USART_Initialize(int ComPortNum, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue)
{
    unsigned char scsmr = 0;
    volatile struct st_sci *sci = SCI[ComPortNum];

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
        MSTP_SCI0 = 0;
        PORT2.ICR.BIT.B1 = 1;
        IPR(SCI0, ERI0) = USART_PRIORITY;
        IPR(SCI0, RXI0) = USART_PRIORITY;
        IPR(SCI0, TXI0) = USART_PRIORITY;
        IEN(SCI0, RXI0) = 1;
        IEN(SCI0, ERI0) = 1;
        IEN(SCI0, TXI0) = 1;
    } else if (ComPortNum == 1) {
        MSTP_SCI1 = 0;
        PORT3.ICR.BIT.B0 = 1;
        IPR(SCI1, ERI1) = USART_PRIORITY;
        IPR(SCI1, RXI1) = USART_PRIORITY;
        IPR(SCI1, TXI1) = USART_PRIORITY;
        IEN(SCI1, RXI1) = 1;
        IEN(SCI1, ERI1) = 1;
        IEN(SCI1, TXI1) = 1;
    } else if (ComPortNum == 2) {
        IOPORT.PFFSCI.BIT.SCI2S = 0;
        MSTP_SCI2 = 0;
        PORT1.ICR.BIT.B2 = 1;
        IPR(SCI2, ERI2) = USART_PRIORITY;
        IPR(SCI2, RXI2) = USART_PRIORITY;
        IPR(SCI2, TXI2) = USART_PRIORITY;
        IEN(SCI2, RXI2) = 1;
        IEN(SCI2, ERI2) = 1;
        IEN(SCI2, TXI2) = 1;
    } else if (ComPortNum == 3) {
        IOPORT.PFFSCI.BIT.SCI3S = 0;
        MSTP_SCI3 = 0;
        PORT1.ICR.BIT.B6 = 1;
        IPR(SCI3, ERI3) = USART_PRIORITY;
        IPR(SCI3, RXI3) = USART_PRIORITY;
        IPR(SCI3, TXI3) = USART_PRIORITY;
        IEN(SCI3, RXI3) = 1;
        IEN(SCI3, ERI3) = 1;
        IEN(SCI3, TXI3) = 1;
    } else
        return FALSE;
    sci->SCR.BYTE = 0;
    sci->SMR.BYTE = scsmr;
    sci->BRR = (48000000 / 32 / BaudRate) - 1;
    //delay_ms(2);
    SCI0.SCR.BYTE = 0xF0;
    return TRUE;
}


BOOL CPU_USART_Uninitialize(int ComPortNum)
{
    switch (ComPortNum)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    }
    return TRUE;
}

BOOL CPU_USART_TxBufferEmpty(int ComPortNum)
{
    return ((*SCI[ComPortNum]).SSR.BIT.TDRE != 0);
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
}

BOOL CPU_USART_TxBufferEmptyInterruptState(int ComPortNum)
{
    return ((*SCI[ComPortNum]).SCR.BIT.TIE != 0);
}

void CPU_USART_RxBufferFullInterruptEnable(int ComPortNum, BOOL Enable)
{
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
    if (USART_AddCharToRxBuffer(port, c))
        Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
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

static void USART_ErISR(UINT32 port)
{
    //(*SCI[port]).RDR;
    //(*SCI[port]).SSR.BYTE = 0x84;
    if ((*SCI[port]).SSR.BYTE & 0x20) {
        (*SCI[port]).SSR.BIT.ORER = 0;
        unsigned char dummy = (*SCI[port]).RDR;
    }
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_SCI0_RXI0(void) { USART_RxISR(0);}
void __attribute__ ((interrupt)) INT_Excep_SCI1_RXI1(void) { USART_RxISR(1);}
void __attribute__ ((interrupt)) INT_Excep_SCI2_RXI2(void) { USART_RxISR(2);}
void __attribute__ ((interrupt)) INT_Excep_SCI3_RXI3(void) { USART_RxISR(3);}
void __attribute__ ((interrupt)) INT_Excep_SCI0_TXI0(void) { USART_TxISR(0);}
void __attribute__ ((interrupt)) INT_Excep_SCI1_TXI1(void) { USART_TxISR(1);}
void __attribute__ ((interrupt)) INT_Excep_SCI2_TXI2(void) { USART_TxISR(2);}
void __attribute__ ((interrupt)) INT_Excep_SCI3_TXI3(void) { USART_TxISR(3);}
void __attribute__ ((interrupt)) INT_Excep_SCI0_ERI0(void) { USART_ErISR(0);}
void __attribute__ ((interrupt)) INT_Excep_SCI1_ERI1(void) { USART_ErISR(1);}
void __attribute__ ((interrupt)) INT_Excep_SCI2_ERI2(void) { USART_ErISR(2);}
void __attribute__ ((interrupt)) INT_Excep_SCI3_ERI3(void) { USART_ErISR(3);}
#ifdef __cplusplus
}
#endif
