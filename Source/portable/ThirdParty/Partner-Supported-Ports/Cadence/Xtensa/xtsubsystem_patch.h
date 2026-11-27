/*
 * xtsubsystem.h -- definitions for the Xtensa multicore subsystem
 */

/*
 * Copyright (c) 2024-2025 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef XTENSA_XTSUBSYSTEM_H
#define XTENSA_XTSUBSYSTEM_H

#include <xtensa/hal.h>
#include <xtensa/tie/xt_core.h>
#include <xtensa/core-macros.h>
#include <xtensa/core-macros-compat.h>

/*
 * XTSubsystem Registers
 *
 * The register window is broken down into 2 portions:
 *   1. Inter-processor interrupt (IPI) register window
 *   2. XTSubsystem control register window
 *
 * Register naming suffix description:
 *
 * - _BASE : base address of a block relative to APB
 * - _B    : base address of a block indexed by core/set ID
 * - _I    : increment amount per core/set index
 * - _O    : offset to register block
 */

/* Xtsubsystem block offset relative to APB base */
#define XTSUB_IPI_O             UINT32_C(0xfc000)
#define XTSUB_IPI_BASE          (XTSUB_IPI_O)
#define XTSUB_IPI_SIZE          UINT32_C(0x3000)

/* Xtsubsystem IPI register definitions */
#define XTSUB_IPI_S0C_B         UINT32_C(0x0000)    /* Inter-processor interrupt set 0 base */
#define XTSUB_IPI_S0C_I         UINT32_C(4)         /* IPI per-core increment */
#define XTSUB_IPI_S_I           UINT32_C(0x0100)    /* IPI per-set increment */

/* Xtsubsystem control register (post-IPI) block definitions */
#define XTSUB_CTLREG_O          UINT32_C(0x3000)
#define XTSUB_CTLREG_BASE       (XTSUB_IPI_BASE + XTSUB_CTLREG_O)
#define XTSUB_CTLREG_SIZE       UINT32_C(0x1000)

/* Xtsubsystem full register block definitions */
#define XTSUB_TOTAL_BASE        (XTSUB_IPI_BASE)
#define XTSUB_TOTAL_SIZE        (XTSUB_IPI_SIZE + XTSUB_CTLREG_SIZE)

/* Register offsets must be <= 1020 (decimal) to be used as imm8 values.
 * Must add in XTSUB_CTLREG_BASE to be used as absolute addresses.
 */
#define XTSUB_RUN_ON_RESET      UINT32_C(0x0020)
#define XTSUB_START_VEC_SEL     UINT32_C(0x0024)
#define XTSUB_ALT_RESET_VEC_B   UINT32_C(0x0030)
#define XTSUB_ALT_RESET_VEC_I   UINT32_C(4)
#define XTSUB_CC_CTRL           UINT32_C(0x0100)
#define XTSUB_CC_STAT           UINT32_C(0x0104)
#define XTSUB_PWAIT_STAT        UINT32_C(0x0108)
#define XTSUB_NUM_CORES         UINT32_C(0x010c)
#define XTSUB_CC_PWR_CTRL_B     UINT32_C(0x0200)
#define XTSUB_CC_PWR_CTRL_I     UINT32_C(4)
#define XTSUB_CC_TIME_LO        UINT32_C(0x02f8)
#define XTSUB_CC_TIME_HI        UINT32_C(0x02fc)
#define XTSUB_CC_TIMECMP_LO_B   UINT32_C(0x0300)
#define XTSUB_CC_TIMECMP_HI_B   UINT32_C(0x0304)
#define XTSUB_CC_TIMECMP_I      UINT32_C(8)

/* Definitions for XTSUB_RUN_ON_RESET */
#define XTSUB_RUN_ALL_CORES     ((1 << XCHAL_SUBSYS_NUM_CORES) - UINT32_C(2))

#if !defined(_ASMLANGUAGE) && !defined(_NOCLANGUAGE) && !defined(__ASSEMBLER__)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function puts the Xtsubsystem base address into the location 
 * pointed to by xtsub_addr.
 *
 * Parameters:
 *      xtsub_addr              Pointer to memory to be written on success
 *
 * It returns:
 *      XTHAL_INVALID           If xtsub_addr is NULL,
 *      XTHAL_UNSUPPORTED       APB is not configured, or
 *      XTHAL_SUCCESS           on success
 */
XT_INLINE int32_t xthal_get_xtsub_address(uint32_t *xtsub_addr)
{
    int32_t ret = xthal_get_apb_address(xtsub_addr);
    if (ret == XTHAL_SUCCESS) {
        *xtsub_addr += XTSUB_TOTAL_BASE;
    }
    return ret;
}

/*
 * This function triggers an inter-processor interrupt (IPI) on the specified
 * core using IPI set 0.  The remote core must not also be the current core.
 * The interrupt type is not checked but must be configured as "external edge."
 *
 * By convention, this function triggers processor interrupt <i>, i.e.
 * XCHAL_SUBSYS_IPI_S0C<n>_INTNUM, on "core_id" where <n> corresponds to the 
 * ID of the core calling this function.  Processor interrupt <i> can be
 * mapped to an external BInterrupt pin using XCHAL_EXTINT<i>_NUM.
 *
 * Parameters:
 *      core_id                 Remote core on which to trigger an IPI
 *
 * It returns:
 *      XTHAL_INVALID           If the specified core is invalid,
 *      XTHAL_UNSUPPORTED       APB is not configured, or
 *      XTHAL_SUCCESS           on success
 */
XT_INLINE int32_t xthal_ipi_trigger(uint32_t core_id)
{
    uint32_t curr_core_id = xthal_get_coreid();
    uint32_t ipi_reg_addr;
    int32_t ret;

    if ((core_id == curr_core_id) || (core_id >= XCHAL_SUBSYS_NUM_CORES)) {
        return XTHAL_INVALID;
    }
    ret = xthal_get_xtsub_address(&ipi_reg_addr);
    if (ret != XTHAL_SUCCESS) {
        return ret;
    }

    ipi_reg_addr += XTSUB_IPI_S0C_B + (XTSUB_IPI_S0C_I * curr_core_id);

    // Writing 1 to bit b sends an interrupt pulse to core b.
    // XTSC models this as an MMIO register, so writing a 0 is required
    // to complete the pulse.  RTL does nothing when 0 is written.
    *(volatile uint32_t *)ipi_reg_addr = (1 << core_id);
    *(volatile uint32_t *)ipi_reg_addr = 0;
    return XTHAL_SUCCESS;
}


/*
 *  Multicore Timer APIs.
 *
 *  Always 64 bits wide, unlike other HAL timer APIs.
 *  Minimal error checking is performed in the interest of speed.
 *
 *  NOTE: Some routines are written in inline assembly to preserve
 *  instruction order without the performance impact of memw operations
 *  (which can be added to volatile pointers due to serialization).
 *  Using assembly for these operations prevents bundling of loads and
 *  stores but we still benefit from bundling other ops.
 */

#define STR(x)                  #x
#define XSTR(x)                 STR(x)
#define APB_BASE_ADDR_MASK      UINT32_C(0xfffff000)

/* Internal helper function optimized for cct APIs */
XT_INLINE uint32_t *xthal_get_xtsub_cctbaseaddr(void)
{
#if XCHAL_SUBSYS_HAVE_CCTIMER

    // Optimization: do not call xthal_get_apb_address(), which would
    // involve an extra function call in addition to stack operations
    // for parameter/return value management
#if (XCHAL_HAVE_XEA2 && XCHAL_HAVE_APB) || XCHAL_HAVE_PROGRAMMABLE_APB
    uint32_t apb_addr = XT_RSR_APB0CFG() & APB_BASE_ADDR_MASK;
#elif (XCHAL_HAVE_APB)
    uint32_t apb_addr = XCHAL_APB_BASEADDR;
#endif
    return (uint32_t *)(apb_addr + XTSUB_CTLREG_BASE);

#else

    return NULL;

#endif /* XCHAL_SUBSYS_HAVE_CCTIMER */
}

XT_INLINE uint64_t xthal_get_cct_cycle_count(void)
{
#if XCHAL_SUBSYS_HAVE_CCTIMER

    uint32_t *regbase = xthal_get_xtsub_cctbaseaddr();
    uint32_t lo, hi1, hi2;
    __asm__ volatile ("l32i %0, %3, " XSTR(XTSUB_CC_TIME_HI) "\n\t"
                      "l32i %1, %3, " XSTR(XTSUB_CC_TIME_LO) "\n\t"
                      "l32i %2, %3, " XSTR(XTSUB_CC_TIME_HI) "\n\t"
        : "=&r"(hi1), "=&r"(lo), "=r"(hi2) : "a"(regbase) : "memory" );
    if (hi1 != hi2) {
        // Reread to get consistent cycle count
        lo = XT_L32I((const int *)regbase, XTSUB_CC_TIME_LO);
    }
    return ((uint64_t)hi2 << UINT32_C(32)) | (uint64_t)lo;

#else

    return 0;

#endif /* XCHAL_SUBSYS_HAVE_CCTIMER */
}

XT_INLINE uint64_t xthal_get_cct_cycle_compare(void)
{
#if XCHAL_SUBSYS_HAVE_CCTIMER

    uint32_t id = xthal_get_coreid();
    uint32_t *regbase = xthal_get_xtsub_cctbaseaddr() +
        (XTSUB_CC_TIMECMP_I * id) / sizeof(uint32_t);
    // Reads with intrinsics here is fine since TIMECMP isn't changing
    uint32_t hi = XT_L32I((const int *)regbase, XTSUB_CC_TIMECMP_HI_B);
    uint32_t lo = XT_L32I((const int *)regbase, XTSUB_CC_TIMECMP_LO_B);
    return ((uint64_t)hi << UINT32_C(32)) | (uint64_t)lo;

#else

    return 0;

#endif /* XCHAL_SUBSYS_HAVE_CCTIMER */
}

XT_INLINE void xthal_set_cct_cycle_compare(uint64_t val)
{
#if XCHAL_SUBSYS_HAVE_CCTIMER

    uint32_t id = xthal_get_coreid();
    uint32_t *regbase = xthal_get_xtsub_cctbaseaddr() +
        (XTSUB_CC_TIMECMP_I * id) / sizeof(uint32_t);
    uint32_t lo = (uint32_t)(val & UINT32_MAX);
    uint32_t hi = (uint32_t)(val >> UINT32_C(32));
    // Value is latched upon writing low word; write high word first.
    // For performance reasons, no precautions are taken to avoid an
    // interrupt triggering between the two writes; the caller should
    // protect against that if needed.
    __asm__ volatile ("s32i %0, %2, " XSTR(XTSUB_CC_TIMECMP_HI_B) "\n\t"
                      "s32i %1, %2, " XSTR(XTSUB_CC_TIMECMP_LO_B) "\n\t"
        :: "r"(hi), "r"(lo), "a"(regbase) : "memory" );

#else

    UNUSED(val);

#endif /* XCHAL_SUBSYS_HAVE_CCTIMER */
}

XT_INLINE void xthal_cct_interrupt_clear(void)
{
#if XCHAL_SUBSYS_HAVE_CCTIMER

    uint64_t cctimecmp_disable = (uint64_t)-1;
    uint64_t reread_val;
    xthal_set_cct_cycle_compare(cctimecmp_disable);

    // Reread compare register to ensure that APB writes have completed
    reread_val = xthal_get_cct_cycle_compare();
    UNUSED(reread_val);

#endif /* XCHAL_SUBSYS_HAVE_CCTIMER */
}

#ifdef __cplusplus
}
#endif

#endif /* !__ASSEMBLER__ */

#endif /* XTENSA_XTSUBSYSTEM_H */

