/******************************************************************************
* Copyright (c) 2024 - 2028 MindCurise, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT

* Ver   Who  		 Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  yafeng.yun   5/31/24     Initial version
******************************************************************************/

#include "cpu.h"

#define CPU_OK          0
#define CPU_FAIL        1

#define CPU_A53_0	    (uint32_t)(0x0u)
#define CPU_A53_1	    (uint32_t)(0x1u)
#define CPU_A53_2	    (uint32_t)(0x2u)
#define CPU_A53_3	    (uint32_t)(0x3u)



static uint32_t PowerUpIsland(uint32_t PwrIslandMask)
{
    uint32_t RegVal;
	uint32_t Status = CPU_OK;

    /* Power up request enable */
    Xil_Out32(PMU_GLOBAL_REQ_PWRUP_INT_EN, PwrIslandMask);

    /* Trigger power up request */
    Xil_Out32(PMU_GLOBAL_REQ_PWRUP_TRIG, PwrIslandMask);

    /* Poll for Power up complete */
    do {
        RegVal = Xil_In32(PMU_GLOBAL_REQ_PWRUP_STATUS) & PwrIslandMask;
    } while (RegVal != 0x0U);

	return Status;
}

static uint32_t ReleaseReset_A53 (uint32_t CpuID)
{
	uint32_t RegValue;
	uint32_t Status;
	uint32_t PwrStateMask;
    switch(CpuID)
    {

    case CPU_A53_0:
        PwrStateMask = PMU_GLOBAL_PWR_STATE_ACPU0_MASK | PMU_GLOBAL_PWR_STATE_FP_MASK | PMU_GLOBAL_PWR_STATE_L2_BANK0_MASK;
        Status = PowerUpIsland(PwrStateMask);
        if (Status != CPU_OK) {
            goto END;
        }

        /**
         *  Enable the clock
         */
        RegValue = Xil_In32(CRF_APB_ACPU_CTRL);
        RegValue |= (CRF_APB_ACPU_CTRL_CLKACT_FULL_MASK | CRF_APB_ACPU_CTRL_CLKACT_HALF_MASK);
        Xil_Out32(CRF_APB_ACPU_CTRL, RegValue);

        /**
         * Release reset
         */
        RegValue = Xil_In32(CRF_APB_RST_FPD_APU);
        RegValue &= ~(CRF_APB_RST_FPD_APU_ACPU0_RESET_MASK | CRF_APB_RST_FPD_APU_APU_L2_RESET_MASK | CRF_APB_RST_FPD_APU_ACPU0_PWRON_RESET_MASK);
        Xil_Out32(CRF_APB_RST_FPD_APU, RegValue);
        break;

    case CPU_A53_1:
        PwrStateMask = PMU_GLOBAL_PWR_STATE_ACPU1_MASK |  PMU_GLOBAL_PWR_STATE_FP_MASK | PMU_GLOBAL_PWR_STATE_L2_BANK0_MASK;
        Status = PowerUpIsland(PwrStateMask);
        if (Status != CPU_OK) {
            goto END;
        }

        /**
         *  Enable the clock
         */
        RegValue = Xil_In32(CRF_APB_ACPU_CTRL);
        RegValue |= (CRF_APB_ACPU_CTRL_CLKACT_FULL_MASK |  CRF_APB_ACPU_CTRL_CLKACT_HALF_MASK);
        Xil_Out32(CRF_APB_ACPU_CTRL, RegValue);

        /**
         * Release reset
         */
        RegValue = Xil_In32(CRF_APB_RST_FPD_APU);
        RegValue &= ~(CRF_APB_RST_FPD_APU_ACPU1_RESET_MASK |  CRF_APB_RST_FPD_APU_APU_L2_RESET_MASK | CRF_APB_RST_FPD_APU_ACPU1_PWRON_RESET_MASK);
        Xil_Out32(CRF_APB_RST_FPD_APU, RegValue);
        break;

    case CPU_A53_2:
        PwrStateMask = PMU_GLOBAL_PWR_STATE_ACPU2_MASK | PMU_GLOBAL_PWR_STATE_FP_MASK | PMU_GLOBAL_PWR_STATE_L2_BANK0_MASK;
        Status = PowerUpIsland(PwrStateMask);
        if (Status != CPU_OK) {
            goto END;
        }

        /**
         *  Enable the clock
         */
        RegValue = Xil_In32(CRF_APB_ACPU_CTRL);
        RegValue |= (CRF_APB_ACPU_CTRL_CLKACT_FULL_MASK | CRF_APB_ACPU_CTRL_CLKACT_HALF_MASK);
        Xil_Out32(CRF_APB_ACPU_CTRL, RegValue);

        /**
         * Release reset
         */
        RegValue = Xil_In32(CRF_APB_RST_FPD_APU);
        RegValue &= ~(CRF_APB_RST_FPD_APU_ACPU2_RESET_MASK | CRF_APB_RST_FPD_APU_APU_L2_RESET_MASK | CRF_APB_RST_FPD_APU_ACPU2_PWRON_RESET_MASK);
        Xil_Out32(CRF_APB_RST_FPD_APU, RegValue);
        break;

    case CPU_A53_3:
        PwrStateMask = PMU_GLOBAL_PWR_STATE_ACPU3_MASK | PMU_GLOBAL_PWR_STATE_FP_MASK | PMU_GLOBAL_PWR_STATE_L2_BANK0_MASK;
        Status = PowerUpIsland(PwrStateMask);
        if (Status != CPU_OK) {
            goto END;
        }

        /**
         *  Enable the clock
         */
        RegValue = Xil_In32(CRF_APB_ACPU_CTRL);
        RegValue |= (CRF_APB_ACPU_CTRL_CLKACT_FULL_MASK | CRF_APB_ACPU_CTRL_CLKACT_HALF_MASK);
        Xil_Out32(CRF_APB_ACPU_CTRL, RegValue);

        /**
         * Release reset
         */
        RegValue = Xil_In32(CRF_APB_RST_FPD_APU);
        RegValue &= ~(CRF_APB_RST_FPD_APU_ACPU3_RESET_MASK | CRF_APB_RST_FPD_APU_APU_L2_RESET_MASK | CRF_APB_RST_FPD_APU_ACPU3_PWRON_RESET_MASK);
        Xil_Out32(CRF_APB_RST_FPD_APU, RegValue);
        break;

    default:
        Status = CPU_FAIL;
        break;
    }

END:
	return Status;
}

static void SetCoreResetVector_A53 (uint32_t CpuID, uint64_t VectorAddr)
{
	uint32_t VectorAddrLow;
	uint32_t VectorAddrHigh;
	uint32_t LowAddressReg;
	uint32_t HighAddressReg;

    /**
     * for A53 cpu, write 64bit handoff address
     * to the RVBARADDR in APU
     */
    VectorAddrLow = (uint32_t )(VectorAddr & 0xFFFFFFFFU);
    VectorAddrHigh = (uint32_t )((VectorAddr>>32) & 0xFFFFFFFFU);
    switch (CpuID)
    {
    case CPU_A53_0:
        LowAddressReg = APU_RVBARADDR0L;
        HighAddressReg = APU_RVBARADDR0H;
        break;
    case CPU_A53_1:
        LowAddressReg = APU_RVBARADDR1L;
        HighAddressReg = APU_RVBARADDR1H;
        break;
    case CPU_A53_2:
        LowAddressReg = APU_RVBARADDR2L;
        HighAddressReg = APU_RVBARADDR2H;
        break;
    case CPU_A53_3:
        LowAddressReg = APU_RVBARADDR3L;
        HighAddressReg = APU_RVBARADDR3H;
        break;

    default:
        /**
         * error can be triggered here
         */
        LowAddressReg = 0U;
        HighAddressReg = 0U;
        break;
    }
    Xil_Out32(LowAddressReg, VectorAddrLow);
    Xil_Out32(HighAddressReg, VectorAddrHigh);

	return;
}


uint32_t CpuUp_A53(uint32_t CpuID, uint64_t ResetAddress)
{
    Xil_DCacheDisable();

    // Step 1: Setup Reset vector of the core
    SetCoreResetVector_A53(CpuID, ResetAddress);

    // Step 2: Take the core out of reset
    ReleaseReset_A53(CpuID);

    Xil_DCacheEnable();

    return 0;
}


void StartAndWaitSecondaryCpuUp(uint64_t CpuID, uint64_t ResetAddr)
{
    static volatile uint32_t CpuUpFlags[4] = {0};
    if (CpuID == 0)
    {
        CpuUpFlags[CpuID] = 1;
        for (CpuID = 1; CpuID < 4; ++CpuID)
        {

            extern void Xil_Assert(const char *File, int32_t Line);
            if (CpuUp_A53((uint32_t)CpuID, ResetAddr) != 0)
            {
                Xil_Assert(__FILE__, __LINE__);
            }
        }

        // Wait for secondary CPUs up
        int CpuIndx = 0;
        while (CpuIndx < 4)
        {
            if (CpuUpFlags[CpuIndx] != 0)
            {
                CpuIndx++;
            }
        }
    }
    else
    {
        CpuUpFlags[CpuID] = 1;
    }
}

