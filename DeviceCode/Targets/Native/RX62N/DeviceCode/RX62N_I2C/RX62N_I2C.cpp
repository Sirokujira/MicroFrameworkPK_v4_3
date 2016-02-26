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

#include <tinyhal.h>
#include "..\RX62N.h"

//#define DEBUG_I2C
//#define DEBUG_I2C_REG_DUMP
//#define DEBUG_I2C_CLK
//#define DEBUG_I2C_INT
//#define DEBUG_I2C_ERR
//#define DEBUG_I2C_NACK
//#define DEBUG_I2C_DETAIL
//#define DEBUG_I2C_TX_DATA
//#define DEBUG_I2C_RX_DATA

#define I2C_TIMEOUT_STOP_CONDITION 100000
#define I2C_TIMEOUT_BUS_BUSY 100000

#define RX62N_SCL RX62N_GPIO::P12;
#define RX62N_SDA RX62N_GPIO::P13;

RX62N_I2C_Driver g_RX62N_I2C_Driver;

//volatile struct st_riic *RIIC[] = {
//    *(volatile struct st_riic    *)0x88300,
//    *(volatile struct st_riic    *)0x88320
//};

static void delay(INT32 t)
{
    while (t-- > 0) {
        asm volatile ("nop");
    }
}

static void clear_i2c_line(void)
{
    if (RIIC0.ICCR1.BIT.SDAI == 0) {
        INT32 t = 10;
        while (t-- > 0) {
            RIIC0.ICCR1.BIT.CLO = 1;
            while (RIIC0.ICCR1.BIT.CLO == 1) ;
            if (RIIC0.ICCR1.BIT.SDAI == 1)
                break;
        }
    }
}

static void i2c_reg_dump(void)
{
    lcd_printf("CR1=%02X CR2=%02X MR1=%02X MR2=%02X MR3=%02X\r\n",
            RIIC0.ICCR1.BYTE, RIIC0.ICCR2.BYTE, RIIC0.ICMR1.BYTE, RIIC0.ICMR3.BYTE, RIIC0.ICMR3.BYTE);
    lcd_printf("FER=%02X SER=%02X IER=%02X SR1=%02X SR2=%02X\r\n",
            RIIC0.ICFER.BYTE, RIIC0.ICSER.BYTE, RIIC0.ICIER.BYTE, RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
}

BOOL RX62N_I2C_Driver::Initialize()
{
//    if (g_RX62N_I2C_Driver.m_initialized == FALSE) {
        g_RX62N_I2C_Driver.m_currentXAction = NULL;
        g_RX62N_I2C_Driver.m_currentXActionUnit = NULL;

        // CPU_GPIO_DisablePin((GPIO_PIN)RX62N_SCL, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);
        // CPU_GPIO_DisablePin((GPIO_PIN)RX62N_SDA, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);

        MSTP(RIIC0) = 0;

        IPR(RIIC0,ICEEI0) = 0x04;
        IPR(RIIC0,ICRXI0) = 0x04;
        IPR(RIIC0,ICTXI0) = 0x04;
        IPR(RIIC0,ICTEI0) = 0x04;

        IR(RIIC0,ICEEI0) = 0;
        IR(RIIC0,ICRXI0) = 0;
        IR(RIIC0,ICTXI0) = 0;
        IR(RIIC0,ICTEI0) = 0;

        IEN(RIIC0,ICEEI0) = 0;
        IEN(RIIC0,ICRXI0) = 0;
        IEN(RIIC0,ICTXI0) = 0;
        IEN(RIIC0,ICTEI0) = 0;

        RIIC0.ICCR1.BIT.ICE = 0;        // I2C disable
        RIIC0.ICCR1.BIT.IICRST = 1;     // I2C internal reset
        while(1 != RIIC0.ICCR1.BIT.IICRST) {}
        RIIC0.ICCR1.BIT.IICRST = 0;     // I2C clear reset
        PORT1.DDR.BIT.B3 = 0;           // P13: SDA0 input
        PORT1.DDR.BIT.B2 = 0;           // P12: SCL0 input
        PORT1.ICR.BIT.B3 = 1;           // P13: SDA0 enable input buffer
        PORT1.ICR.BIT.B2 = 1;           // P12: SCL0 enable input buffer
        RIIC0.ICMR1.BIT.CKS = 3;        // I2C CKS=3 then n=8, PCLK/n = 6000000
        RIIC0.ICBRH.BIT.BRH = 30;       // Set High width of SCL
        RIIC0.ICBRL.BIT.BRL = 30;       // Set Low width of SCL
        RIIC0.ICSER.BYTE = 0x00;        // I2C reset bus status enable register
        RIIC0.ICMR3.BIT.ACKWP = 1;      // I2C allow to write ACKBT (transfer acknowledge bit)
        RIIC0.ICIER.BYTE = 0xF0;        /* b0: Disable Time Out interrupt */
                                        /* b1: Disable Arbitration Lost interrupt */
                                        /* b2: Disable Start Condition Detection Interrupt */
                                        /* b3: Disable Stop condition detection interrupt */
                                        /* b4: Disable NACK reception interrupt */
                                        /* b5: Enable Receive Data Full Interrupt */
                                        /* b6: Enable Transmit End Interrupt */
                                        /* b7: Enable Transmit Data Empty Interrupt */
        RIIC0.ICCR1.BIT.ICE = 1;        // I2C enable
        clear_i2c_line();
        g_RX62N_I2C_Driver.m_initialized  = TRUE;
#if defined(DEBUG_I2C)
        lcd_printf("I2C Init\r\n");
#endif
//    }
    return TRUE;
}

BOOL RX62N_I2C_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

//    if (g_RX62N_I2C_Driver.m_initialized == TRUE) {
        g_RX62N_I2C_Driver.m_initialized = FALSE;
        // CPU_GPIO_EnableInputPin((GPIO_PIN)RX62N_SCL, FALSE, NULL, GPIO_INT_NONE, RESISTOR_DISABLED);
        // CPU_GPIO_EnableInputPin((GPIO_PIN)RX62N_SDA, NULL, GPIO_INT_NONE, RESISTOR_DISABLED);
        RIIC0.ICIER.BYTE = 0;           // I2C interrupt disable
        RIIC0.ICCR1.BIT.ICE = 0;        // I2C disable
#if defined(DEBUG_I2C)
        lcd_printf("I2C Uninit\r\n");
#endif
//    }
    return TRUE;
}

void RX62N_I2C_Driver::ReadLastByte(void)
{
    I2C_HAL_XACTION *xAction = g_RX62N_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_RX62N_I2C_Driver.m_currentXActionUnit;

    //while ((RIIC0.ICSR2.BIT.RDRF) == 0);
    if(xAction->ProcessingLastUnit()) {
        RIIC0.ICSR2.BIT.STOP = 0;
        RIIC0.ICCR2.BIT.SP = 1;
        ReadFromSubordinate(unit);
        RIIC0.ICMR3.BIT.WAIT = 0;
        UINT32 timeout = I2C_TIMEOUT_STOP_CONDITION;
        while (timeout-- > 0) {
            if (RIIC0.ICSR2.BIT.STOP != 0)
                break;
        }
        RIIC0.ICSR2.BIT.NACKF = 0;
        RIIC0.ICSR2.BIT.STOP = 0;
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed );//Complete
    } else {
        ReadFromSubordinate(unit);
        MasterXAction_Start(xAction, true);
    }
}

void RX62N_I2C_Driver::StopCondition(void)
{
    RIIC0.ICSR2.BIT.NACKF = 0;
    RIIC0.ICSR2.BIT.STOP = 0;
    RIIC0.ICCR2.BIT.SP= 1;
    UINT32 timeout = I2C_TIMEOUT_STOP_CONDITION;
    while (RIIC0.ICSR2.BIT.STOP == 0) {
        if (timeout-- == 0) {
#ifdef DEBUG_I2C
            lcd_printf("I2C SC TO\r\n");
#endif
            break;
        }
    }
    RIIC0.ICSR2.BIT.STOP = 0;
#ifdef DEBUG_I2C
    lcd_printf("I2C SC OK\r\n");
#endif
}

void RX62N_I2C_Driver::Abort(void)
{
    UINT8 dummy;
    I2C_HAL_XACTION *xAction = g_RX62N_I2C_Driver.m_currentXAction;
    StopCondition();
    RIIC0.ICIER.BYTE = 0x00;
    while (0x00 != RIIC0.ICIER.BYTE) ;
    IR(RIIC0, ICRXI0) = 0;        /* ICRXI0 clear interrupt flag */
    IR(RIIC0, ICTXI0) = 0;        /* ICTXI0 clear interrupt flag */
    dummy = RIIC0.ICDRR;
    RIIC0.ICCR1.BIT.ICE = 0;
    xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted);
}

void RX62N_I2C_Driver::MasterXAction_Start(I2C_HAL_XACTION* xAction, bool repeatedStart)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    UINT8 address;
    UINT32 timeout;

//    if (g_RX62N_I2C_Driver.m_initialized == FALSE)
//        return;
    g_RX62N_I2C_Driver.m_currentXAction = xAction;
    g_RX62N_I2C_Driver.m_currentXActionUnit = xAction->m_xActionUnits[xAction->m_current++];
    I2C_HAL_XACTION_UNIT *unit = g_RX62N_I2C_Driver.m_currentXActionUnit;

    RIIC0.ICCR1.BIT.ICE = 0;    // I2C disable
    RIIC0.ICMR1.BYTE = (RIIC0.ICMR1.BYTE & 0x8f) | (xAction->m_clockRate2 << 4);
    RIIC0.ICBRH.BIT.BRH = xAction->m_clockRate & 0x1f;
    RIIC0.ICBRL.BIT.BRL = xAction->m_clockRate & 0x1f;
    RIIC0.ICIER.BYTE = 0xF0;
    RIIC0.ICCR1.BIT.ICE = 1;    // I2C enable
#ifdef DEBUG_I2C_REG_DUMP
    i2c_reg_dump();
#endif

    address  = 0xFE & (xAction->m_address << 1);
    address |= unit->IsReadXActionUnit() ? RX62N_I2C_Driver::c_DirectionRead : RX62N_I2C_Driver::c_DirectionWrite;

    RIIC0.ICCR2.BIT.MST = 1;    // I2C master
    RIIC0.ICCR2.BIT.TRS = 1;    // I2C transmit
    if (repeatedStart) {
        while (RIIC0.ICCR2.BIT.BBSY == 0);
#ifdef DEBUG_I2C
        lcd_printf("I2C RS SLA=%02X %02X %02X\r\n", address, RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
        //lcd_printf("I2C RS SLA=%02X icmr1(cks)=%02X brh,brl=%02X\r\n", address, RIIC0.ICMR1.BYTE, RIIC0.ICBRH.BIT.BRH);
#endif
        RIIC0.ICCR2.BIT.RS = 1; // I2C repeatedstart condition
        while (RIIC0.ICCR2.BIT.RS == 1) ;
        while (RIIC0.ICSR2.BIT.TDRE == 0) ;
    } else {
        timeout = I2C_TIMEOUT_BUS_BUSY;
        while (RIIC0.ICCR2.BIT.BBSY) {
            if (timeout-- == 0) {
#ifdef DEBUG_I2C
                lcd_printf("I2C BBSY\r\n");
#endif
                Abort();
                return;
            }
        }
#ifdef DEBUG_I2C
        lcd_printf("I2C ST SLA=%02X %02X %02X\r\n", address, RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
        //lcd_printf("I2C ST SLA=%02X icmr1(cks)=%02X brh,brl=%02X\r\n", address, RIIC0.ICMR1.BYTE, RIIC0.ICBRH.BIT.BRH);
#endif
        RIIC0.ICCR2.BIT.ST = 1; // I2C start condition
        timeout = I2C_TIMEOUT_BUS_BUSY;
        while (1) {
            if ((RIIC0.ICSR2.BIT.NACKF == 1) || (timeout-- == 0)) {
#ifdef DEBUG_I2C
                lcd_printf("I2C NK %02X TO=%d\r\n", RIIC0.ICSR2.BYTE, timeout);
#endif
                Abort();
                return;
            }
            if (RIIC0.ICSR2.BIT.TDRE != 0)
                break;
        }
    }
    RIIC0.ICDRT = address;      // I2C send slave address
                                // TODO: handling 10 bit address
    if (!unit->IsReadXActionUnit()) {
        while (RIIC0.ICSR2.BIT.TDRE == 0) ;
        IEN(RIIC0,ICEEI0) = 1;
        IEN(RIIC0,ICRXI0) = 1;
        IEN(RIIC0,ICTXI0) = 1;
        IEN(RIIC0,ICTEI0) = 1;
        RIIC0.ICIER.BYTE |= (RX62N_I2C::ICIER_TIE || RX62N_I2C::ICIER_TEIE);
    } else {
        //RIIC0.ICSR2.BIT.TEND = 0;
        //RIIC0.ICCR2.BIT.TRS = 0;
        //delay(1000);
        while (RIIC0.ICSR2.BIT.RDRF == 0) ;
        IEN(RIIC0,ICEEI0) = 1;
        IEN(RIIC0,ICRXI0) = 1;
        IEN(RIIC0,ICTXI0) = 1;
        IEN(RIIC0,ICTEI0) = 1;
        RIIC0.ICIER.BYTE |= (RX62N_I2C::ICIER_RIE || RX62N_I2C::ICIER_NAKIE || RX62N_I2C::ICIER_SPIE);
        if (unit->m_bytesToTransfer == 1) {
            RIIC0.ICMR3.BIT.WAIT = 1;
            RIIC0.ICMR3.BIT.ACKBT = 1;
        }
        UINT8 dummy = RIIC0.ICDRR;
    }
}

void RX62N_I2C_Driver::MasterXAction_Stop()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

    IR(RIIC0,ICEEI0) = 0;
    IR(RIIC0,ICRXI0) = 0;
    IR(RIIC0,ICTXI0) = 0;
    IR(RIIC0,ICTEI0) = 0;
    IEN(RIIC0,ICEEI0) = 0;
    IEN(RIIC0,ICRXI0) = 0;
    IEN(RIIC0,ICTXI0) = 0;
    IEN(RIIC0,ICTEI0) = 0;
    RIIC0.ICIER.BYTE = 0x00;
    RIIC0.ICCR1.BIT.ICE = 0;
    g_RX62N_I2C_Driver.m_currentXAction = NULL;
    g_RX62N_I2C_Driver.m_currentXActionUnit = NULL;
#ifdef DEBUG_I2C
    lcd_printf("I2C Stop\r\n");
#endif
}

static UINT8 pclk_div[8] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

void RX62N_I2C_Driver::GetClockRate(UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2)
{
    UINT32 bgr;
    UINT8 div_idx = 3;
    if (rateKhz > 400)
        rateKhz = 400;
    if (rateKhz == 0)
        rateKhz = 1;
    rateKhz *= 1000;
    bgr = PCLK/rateKhz;
    if (bgr <= 480) {
        div_idx = 3;
    } else if (bgr <= 4800) {
        div_idx = 6;
    } else {
        div_idx = 7;
    }
    clockRate2 = (UINT8)div_idx;
    clockRate = (UINT8)(((bgr/pclk_div[div_idx]) & 0xff) / 2);
#ifdef DEBUG_I2C_CLK
    lcd_printf("I2C CLK=%d %d\r\n", clockRate, clockRate2);
#endif
}

void RX62N_I2C_Driver::WriteToSubordinate(I2C_HAL_XACTION_UNIT *unit)
{
    UINT8 *queueData;
    queueData = unit->m_dataQueue.Pop();
    //ASSERT(queueData);
    if (queueData == NULL) {
        lcd_printf("I2C wque err\r\n");
        return;
    }
#ifdef DEBUG_I2C_TX_DATA
    lcd_printf("w%02x", *queueData);
    //lcd_printf("w");
#endif
    RIIC0.ICDRT = *queueData;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void RX62N_I2C_Driver::ReadFromSubordinate(I2C_HAL_XACTION_UNIT *unit)
{
    UINT8* queueData;
    queueData = unit->m_dataQueue.Push();
    //ASSERT(queueData);
    if (queueData == NULL) {
        lcd_printf("I2C rque err\r\n");
        return;
    }
    UINT8 data = RIIC0.ICDRR;
#ifdef DEBUG_I2C_RX_DATA
    lcd_printf("r%02x", data);
    //lcd_printf("r");
#endif
    *queueData = data;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

static void RX62N_I2C_ICEEI_ISR()
{
#ifdef DEBUG_I2C_INT
    lcd_printf("I2C EEI %02X %02X\r\n", RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
#endif
    // Check Timeout
    if ((RIIC0.ICSR2.BIT.TMOF!=0) && (RIIC0.ICIER.BIT.TMOIE!=0)) {
#ifdef DEBUG_I2C_INT
        lcd_printf("I2C TO\r\n");
#endif
        RIIC0.ICSR2.BIT.TMOF = 0;
    }
    // Check Arbitration Lost
    if ((RIIC0.ICSR2.BIT.AL!=0) && (RIIC0.ICIER.BIT.ALIE!=0)) {
#ifdef DEBUG_I2C_INT
        lcd_printf("I2C AL\r\n");
#endif
        RIIC0.ICSR2.BIT.AL = 0;
    }
    // Check stop condition detection
    if ((RIIC0.ICSR2.BIT.STOP!=0) && (RIIC0.ICIER.BIT.SPIE!=0)) {
#ifdef DEBUG_I2C_INT
        lcd_printf("I2C SC\r\n");
#endif
        RIIC0.ICSR2.BIT.STOP = 0;
        // ToDo:
    }
    // Check NACK reception
    if ((RIIC0.ICSR2.BIT.NACKF != 0) && (RIIC0.ICIER.BIT.NAKIE!=0)) {
        UINT8 dummy;
#ifdef DEBUG_I2C_INT
        lcd_printf("I2C NK\r\n");
#endif
        RIIC0.ICSR2.BIT.NACKF = 0;
        g_RX62N_I2C_Driver.Abort();
    }
    // Check start condition detection
    if ((RIIC0.ICSR2.BIT.START != 0) && (RIIC0.ICIER.BIT.STIE!=0)) {
#ifdef DEBUG_I2C_INT
        lcd_printf("I2C SC\r\n");
#endif
        RIIC0.ICSR2.BIT.START = 0;
    }
}

static void RX62N_I2C_ICRXI_ISR()
{
    I2C_HAL_XACTION_UNIT *unit = g_RX62N_I2C_Driver.m_currentXActionUnit;
    I2C_HAL_XACTION *xAction = g_RX62N_I2C_Driver.m_currentXAction;
    GLOBAL_LOCK(irq);
#ifdef DEBUG_I2C_INT
    lcd_printf("I2C RXI\r\n");
#endif
    while (RIIC0.ICSR2.BIT.RDRF == 0) ;
    if (unit->m_bytesToTransfer > 2) {
        if (unit->m_bytesToTransfer == 3)
            RIIC0.ICMR3.BIT.WAIT = 1;
        g_RX62N_I2C_Driver.ReadFromSubordinate(unit);
    } else if (unit->m_bytesToTransfer == 2) {
        RIIC0.ICMR3.BIT.ACKBT = 1;
        g_RX62N_I2C_Driver.ReadFromSubordinate(unit);
    } else {
        g_RX62N_I2C_Driver.ReadLastByte();
    }
}

static void RX62N_I2C_ICTXI_ISR()
{
    I2C_HAL_XACTION_UNIT *unit = g_RX62N_I2C_Driver.m_currentXActionUnit;
    GLOBAL_LOCK(irq);
#ifdef DEBUG_I2C_INT
    lcd_printf("I2C TXI\r\n");
#endif
    if (unit->m_bytesToTransfer) {
        g_RX62N_I2C_Driver.WriteToSubordinate(unit);
    } else {
        RIIC0.ICIER.BIT.TIE = 0;
    }
}

static void RX62N_I2C_ICTEI_ISR()
{
    I2C_HAL_XACTION *xAction = g_RX62N_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_RX62N_I2C_Driver.m_currentXActionUnit;
#ifdef DEBUG_I2C_INT
    lcd_printf("I2C TEI\r\n");
#endif
    RIIC0.ICIER.BIT.TEIE = 0;
    if (xAction->ProcessingLastUnit()) {
        g_RX62N_I2C_Driver.StopCondition();
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed );
    } else {
#if (IIC_READ_WRITE_DELAY == 1)
    HAL_Time_Sleep_MicroSeconds_InterruptEnabled(500);
#endif
        g_RX62N_I2C_Driver.MasterXAction_Start(xAction, true);
    }
}

void  RX62N_I2C_Driver::GetPins(GPIO_PIN& scl, GPIO_PIN& sda)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    scl = RX62N_SCL;
    sda = RX62N_SDA;
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_RIIC0_EEI0(void) { RX62N_I2C_ICEEI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_RXI0(void) { RX62N_I2C_ICRXI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_TXI0(void) { RX62N_I2C_ICTXI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_TEI0(void) { RX62N_I2C_ICTEI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC1_EEI1(void) {}
void __attribute__ ((interrupt)) INT_Excep_RIIC1_RXI1(void) {}
void __attribute__ ((interrupt)) INT_Excep_RIIC1_TXI1(void) {}
void __attribute__ ((interrupt)) INT_Excep_RIIC1_TEI1(void) {}
#ifdef __cplusplus
}
#endif
