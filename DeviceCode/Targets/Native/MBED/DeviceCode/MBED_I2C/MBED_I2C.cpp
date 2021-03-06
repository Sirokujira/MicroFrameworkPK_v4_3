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
#include "..\MBED.h"
#include "mbed.h"
#include "system_MBRZA1H.h"
#include "mbed_assert.h"
#include "i2c_api.h"
#include "cmsis.h"
#include "pinmap.h"
#include "r_typedefs.h"
#include "riic_iodefine.h"
#include "RZ_A1_Init.h"
#include "MBRZA1H.h"
#include "MBED_I2C.h"

//#define DEBUG_I2C
//#define DEBUG_I2C_INT
//#define DEBUG_I2C_ERR
//#define DEBUG_I2C_NACK
//#define DEBUG_I2C_TIMEOUT
//#define DEBUG_I2C_DETAIL
//#define DEBUG_I2C_TX_DATA
//#define DEBUG_I2C_RX_DATA
//#define I2C_RESET_ERROR
//#define DEBUG_I2C_DUMP_REGS

#define ENABLE_NACKF
#define I2C_TIMEOUT_1S (3600000)
#define I2C_TIMEOUT_100MS (360000)
#define I2C_TIMEOUT_1MS (36000)
#define I2C_TIMEOUT_NACK I2C_TIMEOUT_1MS
#define I2C_TIMEOUT_STOP_CONDITION I2C_TIMEOUT_100MS
#define I2C_TIMEOUT_BUS_BUSY I2C_TIMEOUT_100MS
#define I2C_CLEAR_LINE

#if defined(__ARMCC_VERSION)
#define float   float
#elif defined(__GNUC__)
#define float   float
#endif

volatile struct st_riic *RIIC[] = RIIC_ADDRESS_LIST;

static const PinMap PinMap_I2C_SDA[] = {
    {P1_1 , I2C_0, 1},
    {P1_3 , I2C_1, 1},
    {P1_7 , I2C_3, 1},
    {NC   , NC   , 0}
};

static const PinMap PinMap_I2C_SCL[] = {
    {P1_0 , I2C_0, 1},
    {P1_2 , I2C_1, 1},
    {P1_6 , I2C_3, 1},
    {NC   , NC,    0}
};

#define NUM_OF_I2C  3

IRQn_Type I2C_TEI[NUM_OF_I2C] = {INTIICTEI0_IRQn, INTIICTEI1_IRQn, INTIICTEI2_IRQn};
IRQn_Type I2C_RI[NUM_OF_I2C]  = {INTIICRI0_IRQn, INTIICRI1_IRQn, INTIICRI2_IRQn};
IRQn_Type I2C_TI[NUM_OF_I2C]  = {INTIICTI0_IRQn, INTIICTI1_IRQn, INTIICTI2_IRQn};
IRQn_Type I2C_SPI[NUM_OF_I2C] = {INTIICSPI0_IRQn, INTIICSPI1_IRQn, INTIICSPI2_IRQn};
IRQn_Type I2C_STI[NUM_OF_I2C] = {INTIICSTI0_IRQn, INTIICSTI1_IRQn, INTIICSTI2_IRQn};
IRQn_Type I2C_NAKI[NUM_OF_I2C]= {INTIICNAKI0_IRQn, INTIICNAKI1_IRQn, INTIICNAKI2_IRQn};
IRQn_Type I2C_ALI[NUM_OF_I2C] = {INTIICALI0_IRQn, INTIICALI1_IRQn, INTIICALI2_IRQn};
IRQn_Type I2C_TMOI[NUM_OF_I2C]= {INTIICTMOI0_IRQn, INTIICTMOI1_IRQn, INTIICTMOI2_IRQn};

IRQHandler I2C_TEI_ISR[NUM_OF_I2C]  = { INT_Excep_I2C_TEI0, INT_Excep_I2C_TEI1, INT_Excep_I2C_TEI2};
IRQHandler I2C_RI_ISR[NUM_OF_I2C]   = { INT_Excep_I2C_RI0, INT_Excep_I2C_RI1, INT_Excep_I2C_RI2};
IRQHandler I2C_TI_ISR[NUM_OF_I2C]   = { INT_Excep_I2C_TI0, INT_Excep_I2C_TI1, INT_Excep_I2C_TI2};
IRQHandler I2C_SPI_ISR[NUM_OF_I2C]  = { INT_Excep_I2C_SPI0, INT_Excep_I2C_SPI1, INT_Excep_I2C_SPI2};
IRQHandler I2C_STI_ISR[NUM_OF_I2C]  = { INT_Excep_I2C_STI0, INT_Excep_I2C_STI1, INT_Excep_I2C_STI2};
IRQHandler I2C_NAKI_ISR[NUM_OF_I2C] = { INT_Excep_I2C_NAKI0, INT_Excep_I2C_NAKI1, INT_Excep_I2C_NAKI2};
IRQHandler I2C_ALI_ISR[NUM_OF_I2C]  = { INT_Excep_I2C_ALI0, INT_Excep_I2C_ALI1, INT_Excep_I2C_ALI2};
IRQHandler I2C_TMOI_ISR[NUM_OF_I2C] = { INT_Excep_I2C_TMOI0, INT_Excep_I2C_TMOI1, INT_Excep_I2C_TMOI2};

PinName SDA_PIN[NUM_OF_I2C] = {P1_1, P1_3, P1_7};
PinName SCL_PIN[NUM_OF_I2C] = {P1_0, P1_2, P1_6};
i2c_t i2c_obj[NUM_OF_I2C];
typedef volatile struct st_riic i2c_reg;

MBED_I2C_Driver g_MBED_I2C_Driver;

static BOOL rz_i2c_wait_BUS_BUSY(i2c_reg *reg)
{
    UINT32 timeout = I2C_TIMEOUT_BUS_BUSY;
    if (reg->RIICnCR2.UINT32 & CR2_BBSY) {
        if (timeout-- == 0) {
#if defined(DEBUG_I2C_TIMEOUT)
            dbg_printf("I2C BBSY TO\r\n");
#endif
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL rz_i2c_wait_STOP(i2c_reg *reg)
{
    UINT32 timeout = I2C_TIMEOUT_STOP_CONDITION;
    while ((reg->RIICnSR2.UINT32 & SR2_STOP) == 0) {
        if (timeout-- == 0) {
#if defined(DEBUG_I2C_TIMEOUT)
            dbg_printf("I2C STOP TO\r\n");
#endif
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL rz_i2c_wait_TDRE(i2c_reg *reg)
{
    UINT32 timeout = I2C_TIMEOUT_BUS_BUSY;
    while ((reg->RIICnSR2.UINT32 & SR2_TDRE) == 0) {
        if ((reg->RIICnSR2.UINT32 & SR2_NACKF) || (timeout-- == 0)) {
#if defined(DEBUG_I2C_TIMEOUT)
            dbg_printf("I2C TDRE TO\r\n");
#endif
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL rz_i2c_wait_RDRF(i2c_reg *reg)
{
    UINT32 timeout = I2C_TIMEOUT_BUS_BUSY;
    while ((reg->RIICnSR2.UINT32 & SR2_RDRF) == 0) {
        if ((reg->RIICnSR2.UINT32 & SR2_NACKF) || (timeout-- == 0)) {
#if defined(DEBUG_I2C_TIMEOUT)
            dbg_printf("I2C RDRF TO\r\n");
#endif
            return FALSE;
        }
    }
    return TRUE;
}

static void rz_i2c_reset(UINT32 ch)
{
    i2c_t *obj = (i2c_t *)&i2c_obj[ch];
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);

    reg->RIICnCR1.UINT32 &= ~CR1_ICE;   // CR1.ICE off
    reg->RIICnCR1.UINT32 |=  CR1_RST;   // CR1.IICRST on
    reg->RIICnCR1.UINT32 |=  CR1_ICE;   // CR1.ICE on

    reg->RIICnMR1.UINT8[0] &= ~0x08;    // P_phi /x  9bit (including Ack)
    reg->RIICnSER.UINT8[0] = 0x00;      // no slave addr enabled
    reg->RIICnMR1.UINT8[0] |=  obj->pclk_bit;
    reg->RIICnBRL.UINT8[0] =  obj->width_low;
    reg->RIICnBRH.UINT8[0] =  obj->width_hi;
    reg->RIICnMR2.UINT8[0] =  0x07;
    reg->RIICnMR3.UINT8[0] =  0x00;

    reg->RIICnFER.UINT8[0] =  0x72;
    reg->RIICnIER.UINT8[0] =  0x00;
    reg->RIICnCR1.UINT32 &= ~CR1_RST;   // CR1.IICRST negate reset
}

static void rz_i2c_frequency(UINT32 ch, int hz) {
    float pclk_val;
    float wait_utime;
    volatile float bps;
    volatile float L_time;         /* H Width period */
    volatile float H_time;         /* L Width period */
    uint32_t tmp_L_width;
    uint32_t tmp_H_width;
    uint32_t remainder;
    uint32_t wk_cks = 0;
    i2c_t *obj = (i2c_t *)&i2c_obj[ch];
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);

    /* set PCLK */
    if (false == RZ_A1_IsClockMode0()) {
        pclk_val = (float)CM1_RENESAS_RZ_A1_P0_CLK;
    } else {
        pclk_val = (float)CM0_RENESAS_RZ_A1_P0_CLK;
    }

    /* Min 10kHz, Max 400kHz */
    if (hz < 10000) {
        bps = 10000;
    } else if (hz > 400000) {
        bps = 400000;
    } else {
        bps = (float)hz;
    }

    /* Calculation L width time */
    L_time = (1 / (2 * bps));   /* Harf period of frequency */
    H_time = L_time;

    /* Check I2C mode of Speed */
    if (bps > 100000) {
        /* Fast-mode */
        L_time -= 102E-9;    /* Falling time of SCL clock. */
        H_time -= 138E-9;    /* Rising time of SCL clock. */
        /* Check L wideth */
        if (L_time < 1.3E-6) {
            /* Wnen L width less than 1.3us */
            /* Subtract Rise up and down time for SCL from H/L width */
            L_time = 1.3E-6;
            H_time = (1 / bps) - L_time - 138E-9 - 102E-9;
        }
    }

    tmp_L_width   = (uint32_t)(L_time * pclk_val * 10);
    tmp_L_width >>= 1;
    wk_cks++;
    while (tmp_L_width >= 341) {
        tmp_L_width >>= 1;
        wk_cks++;
    }
    remainder   = tmp_L_width % 10;
    tmp_L_width = ((tmp_L_width + 9) / 10) - 3;       /* carry */

    tmp_H_width   = (uint32_t)(H_time * pclk_val * 10);
    tmp_H_width >>= wk_cks;
    if (remainder == 0) {
        tmp_H_width   = ((tmp_H_width + 9) / 10) - 3; /* carry */
    } else {
        remainder    += tmp_H_width % 10;
        tmp_H_width   = (tmp_H_width / 10) - 3;
        if (remainder > 10) {
            tmp_H_width += 1;                         /* fine adjustment */
        }
    }
    /* timeout of BBSY bit is minimum low width by frequency */
    /* so timeout calculates "(low width) * 2" by frequency */
    wait_utime = (L_time * 2) * 1000000;
    /* 1 wait of BBSY bit is about 0.3us. if it's below 0.3us, wait count is set as 1. */
    if (wait_utime <= 0.3) {
        obj->bbsy_wait_cnt = 1;
    } else {
        obj->bbsy_wait_cnt = (int)(wait_utime / 0.3);
    }

    /* I2C Rate */
    obj->pclk_bit  = (uint8_t)(0x10 * wk_cks);        /* P_phi / xx */
    obj->width_low = (uint8_t)(tmp_L_width | 0x000000E0);
    obj->width_hi  = (uint8_t)(tmp_H_width | 0x000000E0);
}

#define NUM_OF_TOGGLE   200

static void rz_i2c_clear_line(UINT32 ch)
{
    UINT32 timeout;
    UINT32 count = NUM_OF_TOGGLE;
    UINT32 hang = 1;
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
#if defined(DEBUG_I2C)
    dbg_printf("I2C CLR\r\n");
#endif
    reg->RIICnFER.UINT32 &= ~FER_MALE;
    reg->RIICnCR1.UINT32 &= ~CR1_SOWP;
    while (count-- > 0) {
        reg->RIICnCR1.UINT32 |= CR1_CLO;
        if (reg->RIICnCR1.UINT32 & CR1_SDAI) {
            hang = 0;
            break;
        }
        timeout = I2C_TIMEOUT_NACK;
        while (reg->RIICnCR1.UINT32 & CR1_CLO) {
            if (timeout-- == 0)
                break;
        }
    }
    reg->RIICnCR1.UINT32 |= CR1_SOWP;
    if (hang) {
#if defined(DEBUG_I2C_ERR)
        dbg_printf("I2C SDA NG\r\n");
#endif
    } else {
#if defined(DEBUG_I2C_ERR)
        dbg_printf("I2C SDA OK\r\n");
#endif
    }
}

static void i2c_IPR(UINT32 ch, UINT32 val)
{
    UINT32 i, intno;
    intno = (UINT32)I2C_TEI[ch];
    for (i = 0; i < 8; i++) {
        GIC_SetPriority((IRQn_Type)(intno + i), val);
    }
}

static void i2c_IEN(UINT32 ch, UINT32 val)
{
    UINT32 i, intno;
    intno = (UINT32)I2C_TEI[ch];
    if (val) {
#if 0
        for (i = 0; i < 8; i++) {
            GIC_EnableIRQ(intno + i);
        }
#else
        GIC_EnableIRQ(I2C_TEI[ch]);
        GIC_EnableIRQ(I2C_RI[ch]);
        GIC_EnableIRQ(I2C_TI[ch]);
        //GIC_EnableIRQ(I2C_SPI[ch]);
        //GIC_EnableIRQ(I2C_STI[ch]);
#if defined(ENABLE_NACKF)
        GIC_EnableIRQ(I2C_NAKI[ch]);
#endif
        //GIC_EnableIRQ(I2C_ALI[ch]);
        //GIC_EnableIRQ(I2C_TMOI[ch]);
#endif
    } else {
        for (i = 0; i < 8; i++) {
            GIC_DisableIRQ((IRQn_Type)(intno + i));
        }
    }
}

static void i2c_Power(UINT32 ch) {
    volatile uint8_t dummy;
    switch (ch) {
        case I2C_0: CPGSTBCR9 &= ~(0x80); break;
        case I2C_1: CPGSTBCR9 &= ~(0x40); break;
        case I2C_2: CPGSTBCR9 &= ~(0x20); break;
        case I2C_3: CPGSTBCR9 &= ~(0x10); break;
    }
    dummy = CPGSTBCR9;
}

BOOL MBED_I2C_Driver::Initialize(UINT32 ch)
{
    i2c_t *obj = (i2c_t *)&i2c_obj[ch];
    I2CName i2c_sda = (I2CName)pinmap_peripheral(SDA_PIN[ch], PinMap_I2C_SDA);
    I2CName i2c_scl = (I2CName)pinmap_peripheral(SCL_PIN[ch], PinMap_I2C_SCL);
    obj->i2c = pinmap_merge(i2c_sda, i2c_scl);
    MBED_ASSERT((int)obj->i2c != NC);
    i2c_Power(ch);
    /* set default frequency at 100k */
    rz_i2c_frequency(ch, 100000);
    rz_i2c_reset(ch);
    pinmap_pinout(SDA_PIN[ch], PinMap_I2C_SDA);
    pinmap_pinout(SCL_PIN[ch], PinMap_I2C_SCL);
    g_MBED_I2C_Driver.m_currentXAction = NULL;
    g_MBED_I2C_Driver.m_currentXActionUnit = NULL;
    g_MBED_I2C_Driver.m_initialized  = TRUE;
    InterruptHandlerRegister(I2C_TEI[ch], (IRQHandler)I2C_TEI_ISR[ch]);
    InterruptHandlerRegister(I2C_RI[ch],  (IRQHandler)I2C_RI_ISR[ch]);
    InterruptHandlerRegister(I2C_TI[ch],  (IRQHandler)I2C_TI_ISR[ch]);
    InterruptHandlerRegister(I2C_SPI[ch], (IRQHandler)I2C_SPI_ISR[ch]);
    InterruptHandlerRegister(I2C_STI[ch], (IRQHandler)I2C_STI_ISR[ch]);
    InterruptHandlerRegister(I2C_NAKI[ch],(IRQHandler)I2C_NAKI_ISR[ch]);
    InterruptHandlerRegister(I2C_ALI[ch], (IRQHandler)I2C_ALI_ISR[ch]);
    InterruptHandlerRegister(I2C_TMOI[ch],(IRQHandler)I2C_TMOI_ISR[ch]);
    i2c_IPR(ch, 6);
#if defined(DEBUG_I2C)
    dbg_printf("I2C Init\r\n");
#endif
    return TRUE;
}

BOOL MBED_I2C_Driver::Uninitialize(UINT32 ch)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
#if defined(DEBUG_I2C)
    dbg_printf("I2C Uninit\r\n");
#endif
    return TRUE;
}

void MBED_I2C_Driver::ReadLastByte(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    I2C_HAL_XACTION *xAction = g_MBED_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_MBED_I2C_Driver.m_currentXActionUnit;

    if(xAction->ProcessingLastUnit()) {
        reg->RIICnSR2.UINT32 &= ~SR2_STOP;
        reg->RIICnCR2.UINT32 |= CR2_SP;
        ReadFromSubordinate(ch, unit);
        reg->RIICnMR3.UINT32 &= ~MR3_WAIT;
        (void)rz_i2c_wait_STOP(reg);
        reg->RIICnSR2.UINT32 &= ~SR2_NACKF;
        reg->RIICnSR2.UINT32 &= ~SR2_STOP;
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed);//Complete
    } else {
        ReadFromSubordinate(ch, unit);
        MasterXAction_Start(ch, xAction, true);
    }
}

void MBED_I2C_Driver::StopCondition(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    reg->RIICnSR2.UINT32 &= ~SR2_STOP;
    reg->RIICnCR2.UINT32 |= CR2_SP;
    UINT8 dummy = reg->RIICnDRR.UINT8[0];
    (void)rz_i2c_wait_STOP(reg);
    reg->RIICnSR2.UINT32 &= ~SR2_NACKF;
    reg->RIICnSR2.UINT32 &= ~SR2_STOP;
}

void MBED_I2C_Driver::Abort(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    I2C_HAL_XACTION *xAction = g_MBED_I2C_Driver.m_currentXAction;
    StopCondition(ch);
#if defined(I2C_RESET_ERROR)
    rz_i2c_reset(ch);
#else
    reg->RIICnIER.UINT32 = 0;
    reg->RIICnCR1.UINT32 &= ~CR1_ICE;
#endif
#if defined(DEBUG_I2C)
    dbg_printf("I2C Abt\r\n");
#endif
    xAction->Signal(I2C_HAL_XACTION::c_Status_Aborted);
}

void MBED_I2C_Driver::MasterXAction_Start(UINT32 ch, I2C_HAL_XACTION* xAction, bool repeatedStart)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    i2c_t *obj = (i2c_t *)&i2c_obj[ch];
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    UINT8 address;
    volatile UINT32 timeout;

#if defined(DEBUG_I2C_DUMP_REGS)
    dbg_printf("XActionS-0 CR1:%02X CR2:%02X SR1:%02X SR2:%02X IER:%02X\r\n",
            reg->RIICnCR1.UINT8[0], reg->RIICnCR2.UINT8[0], reg->RIICnSR1.UINT8[0], reg->RIICnSR2.UINT8[0], reg->RIICnIER.UINT8[0]);
    dbg_printf("XActionS-1 MR1:%02X MR2:%02X MR3:%02X\r\n",
            reg->RIICnMR1.UINT8[0], reg->RIICnMR2.UINT8[0], reg->RIICnMR3.UINT8[0]);
    dbg_printf("XActionS-2 FER:%02X BRL:%02X BRH:%02X\r\n",
            reg->RIICnFER.UINT8[0], reg->RIICnBRL.UINT8[0], reg->RIICnBRH.UINT8[0]);
#endif
    if (g_MBED_I2C_Driver.m_initialized == FALSE)
        return;
    if ((reg->RIICnCR1.UINT32 & CR1_SDAI) == 0) {
        rz_i2c_clear_line(ch);
    }
    GLOBAL_LOCK(irq);
    g_MBED_I2C_Driver.m_currentXAction = xAction;
    g_MBED_I2C_Driver.m_currentXActionUnit = xAction->m_xActionUnits[xAction->m_current++];
    I2C_HAL_XACTION_UNIT *unit = g_MBED_I2C_Driver.m_currentXActionUnit;

    reg->RIICnCR1.UINT32 &= ~CR1_ICE;       // I2C disable
    reg->RIICnMR1.UINT8[0] &= ~0x08;        // P_phi /x  9bit (including Ack)
    reg->RIICnMR1.UINT8[0] |=  obj->pclk_bit;
    reg->RIICnBRL.UINT8[0] =  obj->width_low;
    reg->RIICnBRH.UINT8[0] =  obj->width_hi;
    reg->RIICnCR1.UINT32 |= CR1_ICE;        // I2C enable
    address  = 0xFE & (xAction->m_address << 1);
    address |= unit->IsReadXActionUnit() ? MBED_I2C_Driver::c_DirectionRead : MBED_I2C_Driver::c_DirectionWrite;
    reg->RIICnCR2.UINT32 |= CR2_MST;        // I2C master
    reg->RIICnCR2.UINT32 |= CR2_TRS;        // I2C transmit
#ifdef DEBUG_I2C
    if (repeatedStart) {
        dbg_printf("I2C RS SLA=%02X %02X %02X %02X\r\n",
                address, reg->RIICnCR1.UINT8[0], reg->RIICnCR2.UINT8[0], reg->RIICnSR2.UINT8[0]);
    } else {
        dbg_printf("I2C ST SLA=%02X %02X %02X %02X\r\n",
                address, reg->RIICnCR1.UINT8[0], reg->RIICnCR2.UINT8[0], reg->RIICnSR2.UINT8[0]);
    }
#endif
    if (rz_i2c_wait_BUS_BUSY(reg) == FALSE) {
        Abort(ch);
        return;
    }
    if (!repeatedStart)
        reg->RIICnCR2.UINT32 |= CR2_ST;    // I2C start condition
    else
        reg->RIICnCR2.UINT32 |= CR2_RS;     // I2C repeatedstart condition
    if (rz_i2c_wait_TDRE(reg) == FALSE) {
        Abort(ch);
        return;
    }
    // TODO: handling 10 bit address
    if (!unit->IsReadXActionUnit()) {
        reg->RIICnIER.UINT32 |= (IER_TIE);
        reg->RIICnDRT.UINT32 = address;    // I2C send slave address
        i2c_IEN(ch, 1);
    } else {
        HAL_Time_Sleep_MicroSeconds_InterruptEnabled(50);
        reg->RIICnDRT.UINT32 = address;    // I2C send slave address
        if (rz_i2c_wait_RDRF(reg) == FALSE) {
            Abort(ch);
            return;
        }
        if (unit->m_bytesToTransfer == 1) {
            reg->RIICnMR3.UINT32 |= MR3_WAIT;
            reg->RIICnMR3.UINT32 |= MR3_ACKWP;
            reg->RIICnMR3.UINT32 |= MR3_ACKBT;
            reg->RIICnMR3.UINT32 &= ~MR3_ACKWP;
        } else if (unit->m_bytesToTransfer == 2) {
            reg->RIICnMR3.UINT32 |= MR3_WAIT;
        }
        reg->RIICnIER.UINT32 |= (IER_RIE);
        UINT8 dummy = reg->RIICnDRR.UINT8[0];
        i2c_IEN(ch, 1);
    }
#ifdef DEBUG_I2C
    dbg_printf("I2C ST\r\n");
#endif
}

void MBED_I2C_Driver::MasterXAction_Stop(UINT32 ch)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    i2c_IEN(ch, 0);
    reg->RIICnIER.UINT32 = 0;
    reg->RIICnCR1.UINT32 &= ~CR1_ICE;
    g_MBED_I2C_Driver.m_currentXAction = NULL;
    g_MBED_I2C_Driver.m_currentXActionUnit = NULL;
#ifdef DEBUG_I2C
    dbg_printf("I2C Stop %02x\r\n", reg->RIICnSR2.UINT8[0]);
#endif
}

void MBED_I2C_Driver::GetClockRate(UINT32 ch, UINT32 rateKhz, UINT8& clockRate, UINT8& clockRate2)
{
    i2c_t *obj = (i2c_t *)&i2c_obj[ch];
    rz_i2c_frequency(ch, (int)rateKhz*1000);
    clockRate2 = (UINT8)obj->width_low;
    clockRate = (UINT8)obj->width_hi;
#ifdef DEBUG_I2C_CLK
    dbg_printf("I2C CLK=%d %d\r\n", clockRate, clockRate2);
#endif
}

void MBED_I2C_Driver::WriteToSubordinate(UINT32 ch, I2C_HAL_XACTION_UNIT *unit)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    UINT8 *queueData;
    queueData = unit->m_dataQueue.Pop();
    //ASSERT(queueData);
    if (queueData == NULL) {
#if defined(DEBUG_I2C)
        dbg_printf("I2C wque err\r\n");
#endif
        return;
    }
#ifdef DEBUG_I2C_TX_DATA
    dbg_printf("w%02x", *queueData);
    //dbg_printf("w");
#endif
    while ((reg->RIICnSR2.UINT32 & SR2_TDRE) == 0);
    reg->RIICnDRT.UINT8[0] = (UINT8)*queueData;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void MBED_I2C_Driver::ReadFromSubordinate(UINT32 ch, I2C_HAL_XACTION_UNIT *unit)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    UINT8* queueData;
    queueData = unit->m_dataQueue.Push();
    //ASSERT(queueData);
    if (queueData == NULL) {
#if defined(DEBUG_I2C)
        dbg_printf("I2C rque err\r\n");
#endif
        return;
    }
    UINT8 data = (UINT8)reg->RIICnDRR.UINT8[0];
#ifdef DEBUG_I2C_RX_DATA
    dbg_printf("r%02x", data);
    //dbg_printf("r");
#endif
    *queueData = data;
    ++unit->m_bytesTransferred;
    --unit->m_bytesToTransfer;
}

void MBED_I2C_OTHER_ISR(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    GLOBAL_LOCK(irq);
    // Check Timeout
#if defined(ENABLE_TMOIE)
    if ((reg->RIICnSR2.UINT32 & SR2_TMOF) && (reg->RIICnIER.UINT32 & IER_TMOIE)) {
#if defined(DEBUG_I2C_INT)
        dbg_printf("I2C TO\r\n");
#endif
        //IIC_EEI_IntTimeOut();
        reg->RIICnSR2.UINT32 &= ~SR2_TMOF;
    }
#endif
#if defined(ENABLE_AL)
    // Check Arbitration Lost
    if ((reg->RIICnSR2.UINT32 & SR2_AL) && (reg->RIICnIER.UINT32 & IER_ALIE)) {
#if defined(DEBUG_I2C_INT)
        dbg_printf("I2C AL\r\n");
#endif
        //IIC_EEI_IntAL();
        reg->RIICnSR2.UINT32 &= ~SR2_AL;
    }
#endif
#if defined(ENABLE_STOP)
    // Check stop condition detection
    if ((reg->RIICnSR2.UINT32 & SR2_STOP) && (reg->RIICnIER.UINT32 & IER_SPIE)) {
        // IIC_EEI_IntSP();
#if defined(DEBUG_I2C_INT)
        dbg_printf("I2C SC\r\n");
#endif
    }
#endif
#if defined(ENABLE_NACKF)
    // Check NACK reception
    if ((reg->RIICnSR2.UINT32 & SR2_NACKF) && (reg->RIICnIER.UINT32 & IER_NAKIE)) {
#if defined(DEBUG_I2C_INT)
        dbg_printf("I2C NK\r\n");
#endif
        //IIC_EEI_IntNack();
        reg->RIICnSR2.UINT32 &= ~SR2_NACKF;
        MBED_I2C_Driver::Abort(ch);
    }
#endif
#if defined(ENABLE_START)
    // Check start condition detection
    if ((reg->RIICnSR2.UINT32 & SR2_START) && (reg->RIICnIER.UINT32 & IER_STIE)) {
#if defined(DEBUG_I2C_INT)
        dbg_printf("I2C SC\r\n");
#endif
        //IIC_EEI_IntST();
        reg->RIICnSR2.UINT32 &= ~SR2_START;
    }
#endif
}

void MBED_I2C_RI_ISR(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    I2C_HAL_XACTION_UNIT *unit = g_MBED_I2C_Driver.m_currentXActionUnit;
    I2C_HAL_XACTION *xAction = g_MBED_I2C_Driver.m_currentXAction;
    GLOBAL_LOCK(irq);
#ifdef DEBUG_I2C_INT
    dbg_printf("I2C RXI\r\n");
#endif
    if (rz_i2c_wait_RDRF(reg) == FALSE) {
        MBED_I2C_Driver::Abort(ch);
        return;
    }
    if (unit->m_bytesToTransfer > 2) {
        if (unit->m_bytesToTransfer == 3)
            reg->RIICnMR3.UINT32 |= MR3_WAIT;
        g_MBED_I2C_Driver.ReadFromSubordinate(ch, unit);
    } else if (unit->m_bytesToTransfer == 2) {
        reg->RIICnMR3.UINT32 |= MR3_ACKWP;
        reg->RIICnMR3.UINT32 |= MR3_ACKBT;
        reg->RIICnMR3.UINT32 &= ~MR3_ACKWP;
        g_MBED_I2C_Driver.ReadFromSubordinate(ch, unit);
    } else {
        g_MBED_I2C_Driver.ReadLastByte(ch);
        reg->RIICnIER.UINT32 &= ~(IER_RIE);
    }
}

void MBED_I2C_TI_ISR(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    I2C_HAL_XACTION_UNIT *unit = g_MBED_I2C_Driver.m_currentXActionUnit;
    GLOBAL_LOCK(irq);
#ifdef DEBUG_I2C_INT
    dbg_printf("I2C TI\r\n");
#endif
    (void)rz_i2c_wait_TDRE(reg);
    if (unit->m_bytesToTransfer) {
        g_MBED_I2C_Driver.WriteToSubordinate(ch, unit);
    } else {
        reg->RIICnIER.UINT32 &= ~IER_TIE;
        reg->RIICnIER.UINT32 |= IER_TEIE;
    }
}

void MBED_I2C_TEI_ISR(UINT32 ch)
{
    i2c_reg *reg = (i2c_reg *)(RIIC[ch]);
    I2C_HAL_XACTION *xAction = g_MBED_I2C_Driver.m_currentXAction;
    I2C_HAL_XACTION_UNIT *unit = g_MBED_I2C_Driver.m_currentXActionUnit;
#ifdef DEBUG_I2C_INT
    dbg_printf("I2C TEI\r\n");
#endif
    reg->RIICnIER.UINT32 &= ~IER_TEIE;
    if (xAction->ProcessingLastUnit()) {
        while ((reg->RIICnSR2.UINT32 & SR2_TEND) == 0);
        g_MBED_I2C_Driver.StopCondition(ch);
        xAction->Signal(I2C_HAL_XACTION::c_Status_Completed );
    } else {
#if (IIC_READ_WRITE_DELAY == 1)
    HAL_Time_Sleep_MicroSeconds_InterruptEnabled(500);
#endif
        g_MBED_I2C_Driver.MasterXAction_Start(ch, xAction, true);
    }
}

void  MBED_I2C_Driver::GetPins(UINT32 ch, GPIO_PIN& scl, GPIO_PIN& sda)
{
    NATIVE_PROFILE_HAL_PROCESSOR_I2C();
    scl = SCL_PIN[ch];
    sda = SDA_PIN[ch];
}

void INT_Excep_I2C_TEI0(void) { MBED_I2C_TEI_ISR(0); }
void INT_Excep_I2C_RI0(void)  { MBED_I2C_RI_ISR(0); }
void INT_Excep_I2C_TI0(void)  { MBED_I2C_TI_ISR(0); }
void INT_Excep_I2C_SPI0(void) { MBED_I2C_OTHER_ISR(0); }
void INT_Excep_I2C_STI0(void) { MBED_I2C_OTHER_ISR(0); }
void INT_Excep_I2C_NAKI0(void){ MBED_I2C_OTHER_ISR(0); }
void INT_Excep_I2C_ALI0(void) { MBED_I2C_OTHER_ISR(0); }
void INT_Excep_I2C_TMOI0(void){ MBED_I2C_OTHER_ISR(0); }
void INT_Excep_I2C_TEI1(void) { MBED_I2C_TEI_ISR(1); }
void INT_Excep_I2C_RI1(void)  { MBED_I2C_RI_ISR(1); }
void INT_Excep_I2C_TI1(void)  { MBED_I2C_TI_ISR(1); }
void INT_Excep_I2C_SPI1(void) { MBED_I2C_OTHER_ISR(1); }
void INT_Excep_I2C_STI1(void) { MBED_I2C_OTHER_ISR(1); }
void INT_Excep_I2C_NAKI1(void){ MBED_I2C_OTHER_ISR(1); }
void INT_Excep_I2C_ALI1(void) { MBED_I2C_OTHER_ISR(1); }
void INT_Excep_I2C_TMOI1(void){ MBED_I2C_OTHER_ISR(1); }
void INT_Excep_I2C_TEI2(void) { MBED_I2C_TEI_ISR(2); }
void INT_Excep_I2C_RI2(void)  { MBED_I2C_RI_ISR(2); }
void INT_Excep_I2C_TI2(void)  { MBED_I2C_TI_ISR(2); }
void INT_Excep_I2C_SPI2(void) { MBED_I2C_OTHER_ISR(2); }
void INT_Excep_I2C_STI2(void) { MBED_I2C_OTHER_ISR(2); }
void INT_Excep_I2C_NAKI2(void){ MBED_I2C_OTHER_ISR(2); }
void INT_Excep_I2C_ALI2(void) { MBED_I2C_OTHER_ISR(2); }
void INT_Excep_I2C_TMOI2(void){ MBED_I2C_OTHER_ISR(2); }
