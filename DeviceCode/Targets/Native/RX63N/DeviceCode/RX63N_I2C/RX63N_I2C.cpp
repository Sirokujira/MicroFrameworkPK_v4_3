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
#include "..\RX63N.h"

#define DEBUG_I2C
//#define DEBUG_I2C_ERR
//#define DEBUG_I2C_NACK
//#define DEBUG_I2C_DETAIL
#define DEBUG_I2C_TX_DATA
#define DEBUG_I2C_RX_DATA

#define I2C_TIMEOUT_STOP_CONDITION 10000
#define I2C_TIMEOUT_BUS_BUSY 100000

#define RX63N_SCL RX63N_GPIO::P12;
#define RX63N_SDA RX63N_GPIO::P13;

RX63N_I2C_Driver g_RX63N_I2C_Driver;

//volatile struct st_riic *RIIC[] = {
//    *(volatile struct st_riic    *)0x88300,
//    *(volatile struct st_riic    *)0x88320
//};


BOOL RX63N_I2C_Driver::Initialize()
{
//    if (g_RX63N_I2C_Driver.m_initialized == FALSE) {
        g_RX63N_I2C_Driver.m_currentXAction = NULL;
        g_RX63N_I2C_Driver.m_currentXActionUnit = NULL;

        // CPU_GPIO_DisablePin((GPIO_PIN)RX63N_SCL, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);
        // CPU_GPIO_DisablePin((GPIO_PIN)RX63N_SDA, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);

        SYSTEM.PRCR.WORD = 0xA502;
        MSTP(RIIC0) = 0;
        SYSTEM.PRCR.WORD = 0xA500;

        RIIC0.ICCR1.BIT.ICE = 0;        // I2C disable
        RIIC0.ICCR1.BIT.IICRST = 1;     // I2C internal reset
        RIIC0.ICCR1.BIT.IICRST = 0;     // I2C clear reset
        RIIC0.ICCR1.BIT.ICE = 1;        // I2C enable
        RIIC0.ICSER.BYTE = 0x00;        // I2C reset bus status enable register
        //RIIC0.ICMR1.BIT.CKS = 2;      // I2C set speed (PCLK/4)
        //RIIC0.ICBRH.BIT.BRH = 7;      // Set High width of SCL
        //RIIC0.ICBRL.BIT.BRL = 16;     // Set Low width of SCL
        RIIC0.ICMR1.BIT.CKS = 3;        // I2C set speed (PCLK/8)
        RIIC0.ICBRH.BIT.BRH = 23;       // Set High width of SCL
        RIIC0.ICBRL.BIT.BRL = 28;       // Set Low width of SCL
        RIIC0.ICMR3.BIT.ACKWP = 1;      // I2C allow to write ACKBT (transfer acknowledge bit)
        RIIC0.ICIER.BYTE = 0xE0;        /* b0: Disable Time Out interrupt */
                                        /* b1: Disable Arbitration Lost interrupt */
                                        /* b2: Disable Start Condition Detection Interrupt */
                                        /* b3: Disable Stop condition detection interrupt */
                                        /* b4: Disable NACK reception interrupt */
                                        /* b5: Enable Receive Data Full Interrupt */
                                        /* b6: Enable Transmit End Interrupt */
                                        /* b7: Enable Transmit Data Empty Interrupt */
        while (0xE0 != RIIC0.ICIER.BYTE) ;

        IPR(RIIC0,EEI0) = 0x04;
        IPR(RIIC0,RXI0) = 0x04;
        IPR(RIIC0,TXI0) = 0x04;
        IPR(RIIC0,TEI0) = 0x04;
        IR(RIIC0,EEI0) = 0;
        IR(RIIC0,RXI0) = 0;
        IR(RIIC0,TXI0) = 0;
        IR(RIIC0,TEI0) = 0;
        IEN(RIIC0,EEI0) = 0;
        IEN(RIIC0,RXI0) = 0;
        IEN(RIIC0,TXI0) = 0;
        IEN(RIIC0,TEI0) = 0;

        MPC.PWPR.BIT.B0WI  = 0;
        MPC.PWPR.BIT.PFSWE = 1;
        MPC.P12PFS.BYTE = 0x0F;
        MPC.P13PFS.BYTE = 0x0F;
        MPC.PWPR.BYTE = 0x80;
        PORT1.PMR.BIT.B2 = 1;
        PORT1.PMR.BIT.B3 = 1;
        g_RX63N_I2C_Driver.m_initialized  = TRUE;
//    }
#if defined(DEBUG_I2C)
    lcd_printf("I2C Init\r\n");
#endif
    return TRUE;
}

BOOL RX63N_I2C_Driver::Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

//    if (g_RX63N_I2C_Driver.m_initialized == TRUE) {
//        g_RX63N_I2C_Driver.m_initialized = FALSE;
        // CPU_GPIO_EnableInputPin((GPIO_PIN)RX63N_SCL, FALSE, NULL, GPIO_INT_NONE, RESISTOR_DISABLED);
        // CPU_GPIO_EnableInputPin((GPIO_PIN)RX63N_SDA, NULL, GPIO_INT_NONE, RESISTOR_DISABLED);
        IEN(RIIC0, EEI0) = 0;
        IEN(RIIC0, RXI0) = 0;
        IEN(RIIC0, TXI0) = 0;
        IEN(RIIC0, TEI0) = 0;
        RIIC0.ICIER.BYTE = 0;           // I2C interrupt disable
        while (0x00 != RIIC0.ICIER.BYTE) {}
        IR(RIIC0, RXI0) = 0;			// ICRXI0 clear interrupt flag
        IR(RIIC0, TXI0) = 0;        	// ICTXI0 clear interrupt flag
        RIIC0.ICCR1.BIT.ICE = 0;        // I2C disable
//    }
#if defined(DEBUG_I2C)
    lcd_printf("I2C Uninit\r\n");
#endif
    return TRUE;
}

void RX63N_I2C_Driver::ReadLastByte(void)
{
    I2C_HAL_XACTION *xAction = g_RX63N_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_RX63N_I2C_Driver.m_currentXActionUnit;

    while ((RIIC0.ICSR2.BIT.RDRF) == 0);
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

void RX63N_I2C_Driver::StopCondition(void)
{
    RIIC0.ICSR2.BIT.STOP = 0;
    RIIC0.ICCR2.BIT.SP= 1;
    UINT32 timeout = I2C_TIMEOUT_STOP_CONDITION;
    while (timeout-- > 0) {
        if (RIIC0.ICSR2.BIT.STOP != 0)
            break;
    }
    RIIC0.ICSR2.BIT.NACKF = 0;
    RIIC0.ICSR2.BIT.STOP = 0;
}

void RX63N_I2C_Driver::Abort(void)
{
    I2C_HAL_XACTION *xAction = g_RX63N_I2C_Driver.m_currentXAction;
    StopCondition();
    RIIC0.ICIER.BYTE = 0x00;
    RIIC0.ICCR1.BIT.ICE = 0;
    xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted);
}

void RX63N_I2C_Driver::MasterXAction_Start(I2C_HAL_XACTION* xAction, bool repeatedStart)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    UINT8 address;
    volatile UINT32 timeout;

    if (g_RX63N_I2C_Driver.m_initialized == FALSE)
        return;
    g_RX63N_I2C_Driver.m_currentXAction = xAction;
    g_RX63N_I2C_Driver.m_currentXActionUnit = xAction->m_xActionUnits[xAction->m_current++];
    I2C_HAL_XACTION_UNIT *unit = g_RX63N_I2C_Driver.m_currentXActionUnit;

    RIIC0.ICCR1.BIT.ICE = 0;    // I2C disable
    RIIC0.ICMR1.BYTE = 0x30;    // I2C 9 bits, PCLK/8
    // TODO: set clock rate
    RIIC0.ICCR1.BIT.ICE = 1;    // I2C enable
    address  = 0xFE & (xAction->m_address << 1);
    address |= unit->IsReadXActionUnit() ? RX63N_I2C_Driver::c_DirectionRead : RX63N_I2C_Driver::c_DirectionWrite;
    RIIC0.ICCR2.BIT.MST = 1;    // I2C master
    RIIC0.ICCR2.BIT.TRS = 1;    // I2C transmit
#ifdef DEBUG_I2C
    if (repeatedStart) {
        lcd_printf("I2C RS SLA=%02X %02X %02X\r\n", address, RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
    } else {
        lcd_printf("I2C ST SLA=%02X %02X %02X\r\n", address, RIIC0.ICSR1.BYTE, RIIC0.ICSR2.BYTE);
    }
#endif
    timeout = I2C_TIMEOUT_BUS_BUSY;
    while (RIIC0.ICCR2.BIT.BBSY) {
        if (timeout-- == 0) {
            Abort();
            return;
        }
    }
    if (!repeatedStart) {
        RIIC0.ICCR2.BIT.ST = 1; // I2C start condition
    }
    else
        RIIC0.ICCR2.BIT.RS = 1; // I2C repeatedstart condition
    timeout = I2C_TIMEOUT_BUS_BUSY;
    while (1) {
        if ((RIIC0.ICSR2.BIT.NACKF == 1) || (timeout-- == 0)) {
#ifdef DEBUG_I2C
            lcd_printf("I2C NK %02X TO=%d\r\n", RIIC0.ICSR2.BYTE, timeout);
#endif
            Abort();
            return;
        }
        if (RIIC0.ICSR2.BIT.TDRE == 1)
            break;
    }
    RIIC0.ICDRT = address;      // I2C send slave address
                                // TODO: handling 10 bit address
    IEN(RIIC0,EEI0) = 1;
    IEN(RIIC0,RXI0) = 1;
    IEN(RIIC0,TXI0) = 1;
    IEN(RIIC0,TEI0) = 1;
    if (!unit->IsReadXActionUnit()) {
        while (RIIC0.ICSR2.BIT.TDRE == 0);  // wait for completing address transfer
        RIIC0.ICIER.BYTE |= (RX63N_I2C::ICIER_TIE || RX63N_I2C::ICIER_TEIE);
    } else {
        RIIC0.ICSR2.BIT.TEND = 0;
        RIIC0.ICCR2.BIT.TRS = 0;
        //while (RIIC0.ICSR2.BIT.RDRF != 1);
        RIIC0.ICIER.BYTE |= (RX63N_I2C::ICIER_RIE || RX63N_I2C::ICIER_NAKIE || RX63N_I2C::ICIER_SPIE);
        UINT8 dummy = RIIC0.ICDRR;
    }
}

void RX63N_I2C_Driver::MasterXAction_Stop()
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();

    RIIC0.ICIER.BYTE = 0x00;
    RIIC0.ICCR1.BIT.ICE = 0;
    g_RX63N_I2C_Driver.m_currentXAction = NULL;
    g_RX63N_I2C_Driver.m_currentXActionUnit = NULL;
#ifdef DEBUG_I2C
    lcd_printf("I2C Stop\r\n");
#endif
}

static UINT8 pclk_div[8] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

void RX63N_I2C_Driver::GetClockRate(UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2)
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

void RX63N_I2C_Driver::WriteToSubordinate(I2C_HAL_XACTION_UNIT *unit)
{
    UINT8 *queueData;
    queueData = unit->m_dataQueue.Pop();
    //ASSERT(queueData);
    if (queueData == NULL) {
        lcd_printf("I2C wque err\r\n");
        return;
    }
#ifdef DEBUG_I2C_TX_DATA
    //lcd_printf("w%02x", *queueData);
    lcd_printf("w");
#endif
    RIIC0.ICDRT = *queueData;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void RX63N_I2C_Driver::ReadFromSubordinate(I2C_HAL_XACTION_UNIT *unit)
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
    //lcd_printf("r%02x", data);
    lcd_printf("r");
#endif
    *queueData = data;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void RX63N_I2C_ICEEI_ISR()
{
    // Check Timeout
    if ((RIIC0.ICSR2.BIT.TMOF!=0) && (RIIC0.ICIER.BIT.TMOIE!=0)) {
        //IIC_EEI_IntTimeOut();
        lcd_printf("I2C TO\n");
    }
    // Check Arbitration Lost
    if ((RIIC0.ICSR2.BIT.AL!=0) && (RIIC0.ICIER.BIT.ALIE!=0)) {
        //IIC_EEI_IntAL();
        lcd_printf("I2C AL\n");
    }
    // Check stop condition detection
    if ((RIIC0.ICSR2.BIT.STOP!=0) && (RIIC0.ICIER.BIT.SPIE!=0)) {
        // IIC_EEI_IntSP();
        lcd_printf("I2C SC\n");
    }
    // Check NACK reception
    if ((RIIC0.ICSR2.BIT.NACKF != 0) && (RIIC0.ICIER.BIT.NAKIE!=0)) {
        //IIC_EEI_IntNack();
        lcd_printf("I2C NK\n");
    }
    // Check start condition detection
    if ((RIIC0.ICSR2.BIT.START != 0) && (RIIC0.ICIER.BIT.STIE!=0)) {
        //IIC_EEI_IntST();
        lcd_printf("I2C SC\n");
    }
}

void RX63N_I2C_ICRXI_ISR()
{
    I2C_HAL_XACTION_UNIT *unit = g_RX63N_I2C_Driver.m_currentXActionUnit;
    I2C_HAL_XACTION *xAction = g_RX63N_I2C_Driver.m_currentXAction;
    GLOBAL_LOCK(irq);
    if (unit->m_bytesToTransfer > 2) {
        if (unit->m_bytesToTransfer == 3)
            RIIC0.ICMR3.BIT.WAIT = 1;
        g_RX63N_I2C_Driver.ReadFromSubordinate(unit);
    } else if (unit->m_bytesToTransfer == 2) {
        RIIC0.ICMR3.BIT.ACKBT = 1;
        g_RX63N_I2C_Driver.ReadFromSubordinate(unit);
    } else {
        g_RX63N_I2C_Driver.ReadLastByte();
    }
}

void RX63N_I2C_ICTXI_ISR()
{
    I2C_HAL_XACTION_UNIT *unit = g_RX63N_I2C_Driver.m_currentXActionUnit;
    GLOBAL_LOCK(irq);
    if (unit->m_bytesToTransfer) {
        g_RX63N_I2C_Driver.WriteToSubordinate(unit);
    } else {
        RIIC0.ICIER.BIT.TIE = 0;
    }
}

void RX63N_I2C_ICTEI_ISR()
{
    I2C_HAL_XACTION *xAction = g_RX63N_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_RX63N_I2C_Driver.m_currentXActionUnit;

    RIIC0.ICIER.BIT.TEIE = 0;
    if (xAction->ProcessingLastUnit()) {
        g_RX63N_I2C_Driver.StopCondition();
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed );
    } else {
#if (IIC_READ_WRITE_DELAY == 1)
    HAL_Time_Sleep_MicroSeconds_InterruptEnabled(500);
#endif
        g_RX63N_I2C_Driver.MasterXAction_Start(xAction, true);
    }
}

void  RX63N_I2C_Driver::GetPins(GPIO_PIN& scl, GPIO_PIN& sda)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    scl = RX63N_SCL;
    sda = RX63N_SDA;
}

#ifdef __cplusplus
extern "C" {
#endif
void __attribute__ ((interrupt)) INT_Excep_RIIC0_EEI0(void) { RX63N_I2C_ICEEI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_RXI0(void) { RX63N_I2C_ICRXI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_TXI0(void) { RX63N_I2C_ICTXI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC0_TEI0(void) { RX63N_I2C_ICTEI_ISR(); }
void __attribute__ ((interrupt)) INT_Excep_RIIC1_EEI1(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC1_RXI1(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC1_TXI1(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC1_TEI1(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC2_EEI2(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC2_RXI2(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC2_TXI2(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC2_TEI2(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC3_EEI3(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC3_RXI3(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC3_TXI3(void){ }
void __attribute__ ((interrupt)) INT_Excep_RIIC3_TEI3(void){ }
#ifdef __cplusplus
}
#endif
