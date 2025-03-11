/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2024 Cadence Design Systems, Inc.
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include <stdlib.h>
#include <xtensa/config/core.h>
#include <xtensa/core-macros.h>
#include "asm-offsets.h"
#include "xtensa_rtos.h"
#include "portmacro.h"

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include <string.h>

#if portUSING_MPU_WRAPPERS

#ifdef DEBUG
#include <xtensa/xtutil.h>
# define XPRINT(...) \
    (xt_printf(__VA_ARGS__))
#else
# define XPRINT(...) \
    do { } while (0)
#endif

/* FIXME - this one is needed to spot the error from user threads until
 * FreeRTOS is fixed to return BaseType_t from functions here, not void.
 */
volatile int PRIVILEGED_DATA xtMPUError = 0;

/* Actual number of private regions that will be used
 */
volatile int PRIVILEGED_DATA g_num_used_mpu_entries = 0;

/* pdFAIL commented out until the function is fixed to return BaseType_t not void
 */
#define NAME(a)	#a
#define FAIL_VOID_RETURN(errnum) \
    do { \
        XPRINT("*FreeRTOS* Error: %s\n", NAME(errnum)); \
        xtMPUError = errnum; \
        return; \
    } while (0)

typedef struct privateDetails_struct {
    uint32_t start;
    uint32_t end;
    uint32_t index;
} privateDetails_t;

/* Stores initial private mappings allocated by MPUSetup thread. Must be global
 * as they'll be used later for old-style threads
 */
static volatile privateDetails_t
g_privateDetails[portNUM_MAX_SWAPPED_MPU_PAIRS] PRIVILEGED_DATA;

#define PRIVATE_LEGACY_STACK_IDX 0
#define PRIVATE_STACK_IDX (portLEGACY_UNPRIVILEGED_TASKS)
#define PRIVATE_CONFIGURABLE_IDX (1 + PRIVATE_STACK_IDX)

#define MPU_ALIGN_DOWN(v) ((uint32_t)(v) & -XCHAL_MPU_ALIGN)
#define MPU_ALIGN_UP(v) (((uint32_t)(v) + XCHAL_MPU_ALIGN - 1) & -XCHAL_MPU_ALIGN)
/*-----------------------------------------------------------*/

/*
 * Check if a region is aligned according to MPU requirements
 */
static BaseType_t PRIVILEGED_FUNCTION
region_mpu_aligned(uint32_t start, uint32_t end)
{
    if ((start & (XCHAL_MPU_ALIGN - 1)) || (end & (XCHAL_MPU_ALIGN - 1))) {
        XPRINT("Region %x/%x not aligned\n", start, end);
        return pdFALSE;
    }
    return pdTRUE;
}

static BaseType_t PRIVILEGED_FUNCTION
region_within_region(uint32_t start, uint32_t end, uint32_t rstart, uint32_t rend)
{
    return (start >= rstart) && (end <= rend);
}

static BaseType_t PRIVILEGED_FUNCTION
region_overlaps_region(uint32_t start, uint32_t end, uint32_t rstart, uint32_t rend)
{
    return (start < rend) && (end > rstart);
}

/*
 * Check if a private region/stack is overlapping any other region
 */
static BaseType_t PRIVILEGED_FUNCTION
private_overlap_other_regions(const struct xMEMORY_REGION * const xRegions,
                              StackType_t *pxBottomOfStack, uint32_t stackSizeInBytes)
{
    int i;

    for (i = 0; i < portNUM_CONFIGURABLE_REGIONS + 1; i++) {
        uint32_t start, end;

        if (!xRegions)
            i = portNUM_CONFIGURABLE_REGIONS;

        if (i == portNUM_CONFIGURABLE_REGIONS) {
            start = (uint32_t)pxBottomOfStack;
            end = (uint32_t)pxBottomOfStack + stackSizeInBytes;
        } else {
            start = (uint32_t)xRegions[i].pvBaseAddress;
            end = (uint32_t)(xRegions[i].pvBaseAddress) + xRegions[i].ulLengthInBytes;
        }

        if (start == 0 && end == 0) {
            /* Unused private entries - by design they are placed in MPU so they
             * don't affect the mapping. Check might fail for them so just don't do it.
             */
            continue;
        }

#if (portUSE_SHARED_DATA == 1)
        if (region_overlaps_region(start, end,
                                   (uint32_t)configSHARED_DATA_START,
                                   (uint32_t)configSHARED_DATA_END)) {
            XPRINT("private region %d overlaps shared data!\n", i);
            return pdTRUE;
        }
#endif
#if (portUSE_PRIVILEGED_DEVICE_SPACE == 1)
        if (region_overlaps_region(start, end,
                                   (uint32_t)configPRIVILEGE_DEVICE_START,
                                   (uint32_t)configPRIVILEGE_DEVICE_END)) {
            XPRINT("private region %d overlaps privileged device space!\n", i);
            return pdTRUE;
        }
#endif
#if (portUSE_USER_DEVICE_SPACE == 1)
        if (region_overlaps_region(start, end,
                                   (uint32_t)configUSER_DEVICE_START,
                                   (uint32_t)configUSER_DEVICE_END)) {
            XPRINT("private region %d overlaps user device space!\n", i);
            return pdTRUE;
        }
#endif
#if (portLEGACY_UNPRIVILEGED_TASKS)
        if (region_overlaps_region(start, end,
                                   configLEGACY_TASK_STACK_START,
                                   configLEGACY_TASK_STACK_END)) {
            XPRINT("private region %d [%08x - %08x[ overlaps legacy tack stacks!\n", i, start, end);
            return pdTRUE;
        }
#endif

        if (region_overlaps_region(start, end,
                                   (uint32_t)portPRIVILEGED_CODE_START,
                                   (uint32_t)portPRIVILEGED_CODE_END)) {
            XPRINT("private region %d overlaps FreeRTOS code!\n", i);
            return pdTRUE;
        }

        if (region_overlaps_region(start, end,
                                   (uint32_t)portPRIVILEGED_DATA_START,
                                   (uint32_t)portPRIVILEGED_DATA_END)) {
            XPRINT("private region %d overlaps FreeRTOS code!\n", i);
            return pdTRUE;
        }
    }
    return pdFALSE;
}


/*
 * Check if all fixed regions are aligned according to MPU requirements
 */
static BaseType_t PRIVILEGED_FUNCTION
mpu_fixed_regions_aligned(void)
{
#if (portUSE_SHARED_DATA == 1)
    if (!region_mpu_aligned((uint32_t)configSHARED_DATA_START,
                            (uint32_t)configSHARED_DATA_END)) {
        XPRINT("shared region not aligned\n");
        return pdFALSE;
    }
#endif
#if (portUSE_PRIVILEGED_DEVICE_SPACE == 1)
    if (!region_mpu_aligned((uint32_t)configPRIVILEGE_DEVICE_START,
                            (uint32_t)configPRIVILEGE_DEVICE_END)) {
        XPRINT("priv device region not aligned\n");
        return pdFALSE;
    }
#endif
#if (portUSE_USER_DEVICE_SPACE == 1)
    if (!region_mpu_aligned((uint32_t)configUSER_DEVICE_START,
                            (uint32_t)configUSER_DEVICE_END)) {
        XPRINT("user dev region not aligned\n");
        return pdFALSE;
    }
#endif
#if (portLEGACY_UNPRIVILEGED_TASKS)
    if (!region_mpu_aligned(configLEGACY_TASK_STACK_START,
                            configLEGACY_TASK_STACK_END)) {
        XPRINT("legacy stacks region not aligned\n");
        return pdFALSE;
    }
#endif

    if(!region_mpu_aligned((uint32_t)portPRIVILEGED_CODE_START,
                           (uint32_t)portPRIVILEGED_CODE_END)) {
        XPRINT("priv code region not aligned\n");
        return pdFALSE;
    }
    if(!region_mpu_aligned((uint32_t)portPRIVILEGED_DATA_START,
                           (uint32_t)portPRIVILEGED_DATA_END)) {
        XPRINT("priv data region not aligned\n");
        return pdFALSE;
    }
    return pdTRUE;
}


static void PRIVILEGED_FUNCTION
add_mpu_entry_to_TCB(xthal_MPU_entry mpumap[], uint32_t addr, uint32_t attr,
                     uint32_t type, uint32_t start)
{
    int index = 0;

    //first check if in order
    if (addr & (XCHAL_MPU_ALIGN - 1)) {
        XPRINT ("Entry addr:%x aligned to %x!\n", addr, XCHAL_MPU_ALIGN);
        addr &= (uint32_t)-XCHAL_MPU_ALIGN;
    }

    for (index = XCHAL_MPU_ENTRIES - 1; index >= 0; index--) {

        /* If already have the same entry check if we are start or end of a segment.
         * If start then simply override the matching entry.
         * If end, don't add new entry
         */
        if ((addr >= (mpumap[index].as & ~0x1U) && start) ||
            (addr >  (mpumap[index].as & ~0x1U) && !start)) {
            //move others down
            //FIXME - see not to move everything but only entries added so far
            memmove(mpumap, mpumap + 1, sizeof(xthal_MPU_entry) * (uint32_t)index);
            break;
        }
    }
	if (index >= 0) {
        mpumap[index] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(addr, 1, attr, type);
	}
}

static int  PRIVILEGED_FUNCTION
find_addr_in_map(xthal_MPU_entry mpumap[], uint32_t addr)
{
    int i;

    for (i = XCHAL_MPU_ENTRIES - 1; i >= 0; i--) {
        if (addr >= (mpumap[i].as & ~0x1U))
            return i;
    }
    return -1;
}

/*---------------------------------------------------------------*/

/*  Need to setup MPU so the lower, unused, entries have as=1.
 *  The function is called from either vPortStoreTaskMPUSettings or prvSetupMPU,
 *  whichever gets executed first. Former is called when thread is created which
 *  might happen before the scheduler start (which calls the latter).
 */
static BaseType_t PRIVILEGED_FUNCTION
setupMPU(void)
{
    int i;

    xthal_MPU_entry   mpumap[XCHAL_MPU_ENTRIES];

    /* set all MPU entries to as=0x1,privileged */
    for (i = 0;  i < XCHAL_MPU_ENTRIES; i++) {
        mpumap[i] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(0, 1, portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE);
    }

    XPRINT("PRIV FUNC SPACE defined:%x-%x (section:%x-%x)\n", portPRIVILEGED_CODE_START,  portPRIVILEGED_CODE_END, (uint32_t)privileged_functions_start, (uint32_t)privileged_functions_end);
    XPRINT("PRIV DATA SPACE define:%x-%x (section:%x-%x)\n",  portPRIVILEGED_DATA_START,  portPRIVILEGED_DATA_END, (uint32_t)privileged_data_start, (uint32_t)privileged_data_end);

    add_mpu_entry_to_TCB(mpumap, (uint32_t) portPRIVILEGED_CODE_START,  portDFLT_KERNCODE_ACCESS,     portDFLT_KERNCODE_TYPE,   1);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) portPRIVILEGED_CODE_END,    portDFLT_UNUSED_MEM_ACCESS,   portDFLT_UNUSED_MEM_TYPE, 0);

    add_mpu_entry_to_TCB(mpumap, (uint32_t) portPRIVILEGED_DATA_START,  portDFLT_KERNDATA_ACCESS,     portDFLT_KERNDATA_TYPE,   1);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) portPRIVILEGED_DATA_END,    portDFLT_UNUSED_MEM_ACCESS,   portDFLT_UNUSED_MEM_TYPE, 0);

#if (portUSE_PRIVILEGED_DEVICE_SPACE == 1)
    XPRINT("PRIV Device Space %x - %x\n", configPRIVILEGE_DEVICE_START, configPRIVILEGE_DEVICE_END);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configPRIVILEGE_DEVICE_START, portDFLT_KERNDEV_ACCESS,      portDFLT_KERNDEV_TYPE,    1);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configPRIVILEGE_DEVICE_END,   portDFLT_UNUSED_MEM_ACCESS,   portDFLT_UNUSED_MEM_TYPE, 0);
#endif
#if (portUSE_USER_DEVICE_SPACE == 1)
    XPRINT("USER Device Space %x - %x\n", configUSER_DEVICE_START, configUSER_DEVICE_END);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configUSER_DEVICE_START,      portDFLT_USERDEV_ACCESS,      portDFLT_USERDEV_TYPE,    1);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configUSER_DEVICE_END,        portDFLT_UNUSED_MEM_ACCESS,   portDFLT_UNUSED_MEM_TYPE, 0);
#endif
#if (portUSE_SHARED_DATA == 1)
    XPRINT("Shared Data Section: %x - %x\n", configSHARED_DATA_START, configSHARED_DATA_END);
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configSHARED_DATA_START,      portDFLT_SHARED_ACCESS,       portDFLT_SHARED_TYPE,     1); //FIXME - check priv/type
    add_mpu_entry_to_TCB(mpumap, (uint32_t) configSHARED_DATA_END,        portDFLT_UNUSED_MEM_ACCESS,   portDFLT_UNUSED_MEM_TYPE, 0);
#endif

    for(i = 0; i < g_num_used_mpu_entries; i++) {
        XPRINT("Private Data Section %d: %x - %x \n", i, g_privateDetails[i].start , g_privateDetails[i].end);
        add_mpu_entry_to_TCB(mpumap, (uint32_t)g_privateDetails[i].start,  portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE,  1);
        add_mpu_entry_to_TCB(mpumap, (uint32_t)g_privateDetails[i].end,    portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE,  0);
    }

    for (i = 0;  i < XCHAL_MPU_ENTRIES; i++) {
        mpumap[i].at &= (uint32_t)-XCHAL_MPU_ENTRIES;
        mpumap[i].at |= (uint32_t)i;
    }

    int map_status = xthal_check_map(mpumap, XCHAL_MPU_ENTRIES);
    if (map_status != XTHAL_SUCCESS) {
        xtMPUError = MPU_ERR_INIT_MAP_NOT_VALID;
        XPRINT("MPU Map Check FAILED: error:%d\n", map_status);
        return pdFALSE;
    }

    xthal_write_map(mpumap, XCHAL_MPU_ENTRIES);

#ifdef DEBUG
    xthal_read_map(mpumap);
    for (i = 0; i < XCHAL_MPU_ENTRIES; i++) {
        XPRINT("ORIG mpumap[%d]: as:%x, at:%x\n", i, mpumap[i].as, mpumap[i].at);
    }
#endif

    // Find entries for private memories
    for(i = 0; i < g_num_used_mpu_entries; i++) {
        int index;

        XPRINT("Look for addr %x\n", g_privateDetails[i].start);

        // If MPU Setup decided not to use a region, it set it to 0-0. Then,
        // just swap entries 0,1. In the future, assign a variable telling how
        // much to swap and don't even swap those for zero.
        if (g_privateDetails[i].start == 0) {
            g_privateDetails[i].index = 0;
            continue;
        }
        index = find_addr_in_map(mpumap, g_privateDetails[i].start);
        if (index < 0) {
            XPRINT("Couldn't find index for addr %x\n", g_privateDetails[i].start);
            xtMPUError = MPU_ERR_INIT_ENTRY_NOT_IN_MAP;
            return pdFALSE;
        }
        g_privateDetails[i].index = (uint32_t)index;
    }

    // Update range if not set by MPU thread, to be from the previous to the next entry
#if DEBUG
    for(i = 0; i < g_num_used_mpu_entries; i++) {
        XPRINT("Entry %d index: %d, range: %x-%x\n", i, g_privateDetails[i].index, g_privateDetails[i].start, g_privateDetails[i].end);
    }
#endif
    return pdTRUE;
}

/* Called from xPortStartScheduler. At that time, some threads might have
 * been created already, which required them creating the MPU map, as their
 * private regions must be checked against it. So, create MPU map is called
 * from either parts, but it'll run only once.
 * In this function, we also write the MPU with that map.
 */
BaseType_t PRIVILEGED_FUNCTION
prvSetupMPU(void)
{
    xthal_dcache_all_writeback_inv();
    xthal_icache_all_invalidate();
    return pdTRUE;
}

typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

static void PRIVILEGED_FUNCTION
init_private_mpu_regions(const struct xMEMORY_REGION * const xRegions,
                         StackType_t *pxBottomOfStack, uint32_t stackSizeInBytes)
{
    int i;

    /* First do checks - if there is overlap or fixed regions not aligned */
    if (mpu_fixed_regions_aligned() == pdFALSE) {
        FAIL_VOID_RETURN(MPU_ERR_FIXED_REGION_NOT_ALIGNED);
    }

#ifdef DEBUG
    XPRINT("INITIAL MPU SETTINGS:\n"
           "stack:        %08x - %08x\n",
           (uint32_t)pxBottomOfStack,
           (uint32_t)pxBottomOfStack + stackSizeInBytes);
#if portLEGACY_UNPRIVILEGED_TASKS
    XPRINT("legacy stack: %08x - %08x\n",
           configLEGACY_TASK_STACK_START,
           configLEGACY_TASK_STACK_END);
#endif
    XPRINT("REGIONS:\n");
    for (i = 0; i < portNUM_CONFIGURABLE_REGIONS; i++) {
        XPRINT("              %08x - %08x\n",
               (uint32_t)xRegions[i].pvBaseAddress,
               (uint32_t)(xRegions[i].pvBaseAddress + xRegions[i].ulLengthInBytes));
    }
#endif

#if portLEGACY_UNPRIVILEGED_TASKS
    g_privateDetails[PRIVATE_LEGACY_STACK_IDX].start = configLEGACY_TASK_STACK_START;
    g_privateDetails[PRIVATE_LEGACY_STACK_IDX].end   = configLEGACY_TASK_STACK_END;
#endif

    if (stackSizeInBytes) {
        /* Have addresses ready - update private entries - index comes later */
        g_privateDetails[PRIVATE_STACK_IDX].start = (uint32_t)pxBottomOfStack;
        g_privateDetails[PRIVATE_STACK_IDX].end   = (uint32_t)pxBottomOfStack + stackSizeInBytes;

        for (i = 0; i < portNUM_CONFIGURABLE_REGIONS; i++) {
            if(xRegions[i].pvBaseAddress == 0 && xRegions[i].ulLengthInBytes == 0) {
                break;
            }
            g_privateDetails[i + PRIVATE_CONFIGURABLE_IDX].start  = (uint32_t)(xRegions[i].pvBaseAddress);
            g_privateDetails[i + PRIVATE_CONFIGURABLE_IDX].end    = (uint32_t)(xRegions[i].pvBaseAddress) + xRegions[i].ulLengthInBytes;
        }
        g_num_used_mpu_entries = i + PRIVATE_CONFIGURABLE_IDX;
    } else {
        g_num_used_mpu_entries = portLEGACY_UNPRIVILEGED_TASKS;
    }

    XPRINT("Will use %d regions out of %d allowed\n",
           g_num_used_mpu_entries < PRIVATE_CONFIGURABLE_IDX ?
           0 : g_num_used_mpu_entries - PRIVATE_CONFIGURABLE_IDX,
           portNUM_CONFIGURABLE_REGIONS);

    if (setupMPU() == pdFAIL) {
        FAIL_VOID_RETURN(MPU_ERR_SETUP_MPU);
    }
    return;
}

static void PRIVILEGED_FUNCTION
init_task_mpu(xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions,
              StackType_t *pxBottomOfStack, uint32_t stackSizeInBytes)
{
    int i;
    xthal_MPU_entry (*mpumap)[2] = xMPUSettings->mpumap;

#ifdef DEBUG
    int name_offset = TCB_TASK_NAME_OFF - TCB_MPU_SETTINGS_OFF;
    XPRINT("\n\nTASK NAME: '%s'\n", (char*)xMPUSettings + name_offset);
#endif

#ifdef DEBUG
    XPRINT("PRIVATE: stackBottom:%p, stackSizeBytes:%d, REGIONS (%p):\n",
           pxBottomOfStack, stackSizeInBytes, xRegions);
    if (xRegions) {
        for (i = 0; i < portNUM_CONFIGURABLE_REGIONS; i++)
            XPRINT("      %x-%x\n",
                   (uint32_t)xRegions[i].pvBaseAddress,
                   (uint32_t)(xRegions[i].pvBaseAddress + xRegions[i].ulLengthInBytes));
    }
#endif

    if (xRegions) {

        /* first check if private regions are aligned or overlap fixed regions. */
        if (private_overlap_other_regions(xRegions, pxBottomOfStack, stackSizeInBytes) == pdTRUE) {
            FAIL_VOID_RETURN(MPU_ERR_OVERLAP_OTHER_REGIONS);
        }

        if (region_mpu_aligned((uint32_t)pxBottomOfStack,
                               (uint32_t)pxBottomOfStack + stackSizeInBytes) == pdFALSE)  {
            FAIL_VOID_RETURN(MPU_ERR_PRIVATE_NOT_ALIGNED);
        }

        for (i = 0; i < g_num_used_mpu_entries - PRIVATE_CONFIGURABLE_IDX; i++) {
            if (region_mpu_aligned((uint32_t)(xRegions[i].pvBaseAddress),
                                   (uint32_t)(xRegions[i].pvBaseAddress) + xRegions[i].ulLengthInBytes) == pdFALSE) {
                FAIL_VOID_RETURN(MPU_ERR_PRIVATE_NOT_ALIGNED);
            }
        }

        /* Set stack MPU mapping */

        if (region_within_region((uint32_t)pxBottomOfStack,
                                 (uint32_t)pxBottomOfStack + stackSizeInBytes,
                                 g_privateDetails[PRIVATE_STACK_IDX].start,
                                 g_privateDetails[PRIVATE_STACK_IDX].end) == pdFALSE) {
            FAIL_VOID_RETURN(MPU_ERR_STACK_NOT_IN_RANGE);
        }

        for (i = 0; i < g_num_used_mpu_entries - PRIVATE_CONFIGURABLE_IDX; i++) {
            if (region_within_region((uint32_t)xRegions[i].pvBaseAddress,
                                     (uint32_t)xRegions[i].pvBaseAddress + xRegions[i].ulLengthInBytes,
                                     g_privateDetails[i + PRIVATE_CONFIGURABLE_IDX].start,
                                     g_privateDetails[i + PRIVATE_CONFIGURABLE_IDX].end) == pdFALSE)
                FAIL_VOID_RETURN(MPU_ERR_REGION_NOT_IN_RANGE);
        }

        mpumap[PRIVATE_STACK_IDX][0] = (xthal_MPU_entry)XTHAL_MPU_ENTRY((uint32_t)pxBottomOfStack,                    1, portDFLT_STACK_ACCESS,       portDFLT_STACK_TYPE);
        mpumap[PRIVATE_STACK_IDX][1] = (xthal_MPU_entry)XTHAL_MPU_ENTRY((uint32_t)pxBottomOfStack + stackSizeInBytes, 1, portDFLT_UNUSED_MEM_ACCESS,  portDFLT_UNUSED_MEM_TYPE);
        mpumap[PRIVATE_STACK_IDX][0].at |= g_privateDetails[PRIVATE_STACK_IDX].index;
        mpumap[PRIVATE_STACK_IDX][1].at |= g_privateDetails[PRIVATE_STACK_IDX].index + 1;

#if portLEGACY_UNPRIVILEGED_TASKS
        mpumap[PRIVATE_LEGACY_STACK_IDX][0] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(configLEGACY_TASK_STACK_START, 1, portDFLT_UNUSED_MEM_ACCESS,  portDFLT_UNUSED_MEM_TYPE);
        mpumap[PRIVATE_LEGACY_STACK_IDX][1] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(configLEGACY_TASK_STACK_END  , 1, portDFLT_UNUSED_MEM_ACCESS,  portDFLT_UNUSED_MEM_TYPE);
        mpumap[PRIVATE_LEGACY_STACK_IDX][0].at |= g_privateDetails[PRIVATE_LEGACY_STACK_IDX].index;
        mpumap[PRIVATE_LEGACY_STACK_IDX][1].at |= g_privateDetails[PRIVATE_LEGACY_STACK_IDX].index + 1;
#endif
        /* Set MPU mappings for private memories */

        for (i = PRIVATE_CONFIGURABLE_IDX; i < g_num_used_mpu_entries; i++) {
            int region_i = i - PRIVATE_CONFIGURABLE_IDX;

            uint32_t start = (uint32_t)(xRegions[region_i].pvBaseAddress);
            uint32_t end   = (uint32_t)(xRegions[region_i].pvBaseAddress) + xRegions[region_i].ulLengthInBytes;

            uint32_t privType   = xRegions[region_i].ulParameters & 0xFFFFE000;  // FIXME - use hal functions for these 2
            uint32_t privAccess = xRegions[region_i].ulParameters & 0x00000FFF;

            if (start == 0 && end == 0) {
                start = g_privateDetails[i].start;
                end   = g_privateDetails[i].end;
                privAccess = portDFLT_UNUSED_MEM_ACCESS;
                privType   = portDFLT_UNUSED_MEM_TYPE;
            }
            mpumap[i][0] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(start, 1, privAccess, privType);
            mpumap[i][1] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(end,   1, portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE);
            mpumap[i][0].at |= g_privateDetails[i].index;
            mpumap[i][1].at |= g_privateDetails[i].index + 1;
        }
    } else {
        for (i = 0; i < g_num_used_mpu_entries; i++) {
            mpumap[i][0] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(g_privateDetails[i].start, 1, portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE);
            mpumap[i][0].at |= g_privateDetails[i].index;
            mpumap[i][1] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(g_privateDetails[i].end,   1, portDFLT_UNUSED_MEM_ACCESS, portDFLT_UNUSED_MEM_TYPE);
            mpumap[i][1].at |= g_privateDetails[i].index + 1;
        }
#if portLEGACY_UNPRIVILEGED_TASKS
        mpumap[PRIVATE_LEGACY_STACK_IDX][0] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(MPU_ALIGN_DOWN(pxBottomOfStack),                            1, portDFLT_STACK_ACCESS,       portDFLT_STACK_TYPE);
        mpumap[PRIVATE_LEGACY_STACK_IDX][1] = (xthal_MPU_entry)XTHAL_MPU_ENTRY(MPU_ALIGN_UP((uint32_t)pxBottomOfStack + stackSizeInBytes), 1, portDFLT_UNUSED_MEM_ACCESS,  portDFLT_UNUSED_MEM_TYPE);
        mpumap[PRIVATE_LEGACY_STACK_IDX][0].at |= g_privateDetails[PRIVATE_LEGACY_STACK_IDX].index;
        mpumap[PRIVATE_LEGACY_STACK_IDX][1].at |= g_privateDetails[PRIVATE_LEGACY_STACK_IDX].index + 1;
#endif
    }

    for (i = 0; i < g_num_used_mpu_entries; i++) {
        XPRINT("mpumap[%d]: as:%x, at:%x\n", i * 2 + 0, mpumap[i][0].as, mpumap[i][0].at);
        XPRINT("mpumap[%d]: as:%x, at:%x\n", i * 2 + 1, mpumap[i][1].as, mpumap[i][1].at);
    }
    XPRINT("\n\n");
}

/*
 * Used to set coprocessor area in stack. Current hack is to reuse MPU pointer for coprocessor area.
 */
void PRIVILEGED_FUNCTION
vPortStoreTaskMPUSettings(xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions,
                          StackType_t *pxBottomOfStack, uint32_t ulStackDepth)
{
    static int PRIVILEGED_DATA mpu_init_done = 0;
    uint32_t stackSizeInBytes = ulStackDepth * sizeof(int);

    /* if !xMPUSettings this is initial call to set private region ranges */

    if (!mpu_init_done) {
        mpu_init_done = 1;
        if (!xMPUSettings) {
            init_private_mpu_regions(xRegions, pxBottomOfStack, stackSizeInBytes);
        } else {
            MemoryRegion_t mpuPrivRegions[portNUM_CONFIGURABLE_REGIONS] = {
#if portNUM_CONFIGURABLE_REGIONS > 0
                {0, 0, 0},
#endif
            };
            init_private_mpu_regions(mpuPrivRegions, 0, 0);
        }
    }
    if (xMPUSettings) {
        init_task_mpu(xMPUSettings, xRegions, pxBottomOfStack, stackSizeInBytes);
    }
}
#endif
