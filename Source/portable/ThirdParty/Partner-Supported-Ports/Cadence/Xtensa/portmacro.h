/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2025 Cadence Design Systems, Inc.
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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include "xtensa_rtos.h"

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <assert.h>

#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/system.h>   /* required for XSHAL_CLIB */

/* required for SMP support */
#if (XCHAL_SW_VERSION >= 1506000)
#include <xtensa/xtsubsystem.h>
#elif (XCHAL_SW_VERSION >= 1505000)
#include "xtsubsystem_patch.h"
#endif
#include <sys/reent.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */

#define portCHAR		int8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	int

typedef portSTACK_TYPE                 StackType_t;
typedef portBASE_TYPE                  BaseType_t;
typedef unsigned portBASE_TYPE         UBaseType_t;

#if( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#else
	#error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif
/*-----------------------------------------------------------*/

// portbenchmark
#include "portbenchmark.h"

// Critical section management. These cannot be nested. They should be used
// with a lot of care and cannot be called from interrupt context.
static inline void
portDISABLE_INTERRUPTS(void)
{
#if XCHAL_HAVE_INTERRUPTS
#if XCHAL_HAVE_XEA2
	xthal_intlevel_set_min (XT_IRQ_LOCK_LEVEL);
#else
	xthal_disable_interrupts ();
#endif
#endif
	portbenchmarkINTERRUPT_DISABLE ();
}

static inline void
portENABLE_INTERRUPTS(void)
{
	portbenchmarkINTERRUPT_RESTORE (0);
#if XCHAL_HAVE_INTERRUPTS
#if XCHAL_HAVE_XEA2
	xthal_intlevel_set (0);
#else
	xthal_enable_interrupts ();
#endif
#endif
}

extern void vTaskEnterCritical(void);
extern void vTaskExitCritical(void);

#if portUSING_MPU_WRAPPERS

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define portENTER_CRITICAL()        vPortEnterCritical()
#define portEXIT_CRITICAL()         vPortExitCritical()

#define portSTACK_ALIGNMENT         XCHAL_MPU_ALIGN
#define portPRIVILEGE_BIT           0x80000000UL
#define portIS_PRIVILEGED()         (int) \
    ({ \
        register unsigned code __asm("a2") = SYSCALL_is_priv; \
        __asm volatile ("syscall\n" \
                        : "+a"(code) :: "memory"); \
        code; \
    })
#define portRAISE_PRIVILEGE() \
    do { \
        register unsigned code __asm("a2") = SYSCALL_raise_priv; \
        __asm volatile ("syscall\n" \
                        : "+a"(code) :: "memory"); \
    } while (0)
#if XCHAL_HAVE_XEA2
#define portRESET_PRIVILEGE() (XT_WSR_PS(XT_RSR_PS() | PS_RING(1)))
#else
#define portRESET_PRIVILEGE() (XT_WSR_PS(XT_RSR_PS() | PS_RING))
#endif

#else

#define portENTER_CRITICAL()        vTaskEnterCritical()
#define portEXIT_CRITICAL()         vTaskExitCritical()
#define portPRIVILEGE_BIT           0UL

#endif

// These allow nested interrupt disabling and restoring via local registers or stack.
// They can be called from interrupt context.
static inline uint32_t
portENTER_CRITICAL_NESTED(void)
{
	uint32_t state;

#if XCHAL_HAVE_INTERRUPTS
#if XCHAL_HAVE_XEA2
	state = xthal_intlevel_set_min (XT_IRQ_LOCK_LEVEL);
#else
	state = xthal_disable_interrupts ();
#endif
#else
	state = 0;
#endif
	portbenchmarkINTERRUPT_DISABLE ();
	return state;
}

static inline void
portEXIT_CRITICAL_NESTED(uint32_t state)
{
	portbenchmarkINTERRUPT_RESTORE (state);
#if XCHAL_HAVE_INTERRUPTS
#if XCHAL_HAVE_XEA2
	xthal_intlevel_set (state);
#else
	xthal_restore_interrupts (state);
#endif
#endif
}

// These FreeRTOS versions are similar to the nested versions above
#define portSET_INTERRUPT_MASK()                     portENTER_CRITICAL_NESTED()
#define portCLEAR_INTERRUPT_MASK(state)              portEXIT_CRITICAL_NESTED(state)
#define portSET_INTERRUPT_MASK_FROM_ISR()            portENTER_CRITICAL_NESTED()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(state)     portEXIT_CRITICAL_NESTED(state)

BaseType_t xPortRaisePrivilege( void );

/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH                ( -1 )
#define portTICK_PERIOD_MS              ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef configBYTE_ALIGNMENT
#define portBYTE_ALIGNMENT              configBYTE_ALIGNMENT
#elif XCHAL_DATA_WIDTH < 16
#define portBYTE_ALIGNMENT              XCHAL_DATA_WIDTH
#else
#define portBYTE_ALIGNMENT              16
#endif
#define portNOP()                       XT_NOP()
/* XT_MEMW() not required for generic C code memory barriers */
#define portMEMORY_BARRIER()            __asm__ volatile ("" ::: "memory")
/*-----------------------------------------------------------*/

/* Multicore specifics. */
#define portMAX_CORE_COUNT              8

#ifndef configNUMBER_OF_CORES
    #define configNUMBER_OF_CORES       1
#elif ( configNUMBER_OF_CORES < 1 || configNUMBER_OF_CORES > portMAX_CORE_COUNT )
    #error "Invalid number of cores specified in config!"
#endif


/* FreeRTOS core id is always zero based; set to 0 for single-core case */
#if ( configNUMBER_OF_CORES > 1 )

/* Various checks to confirm config is compatible with FreeRTOS SMP */
#if ( !XCHAL_DCACHE_IS_COHERENT || ( XCHAL_SUBSYS_NUM_CORES == 1 ))
    #error "SMP support requires Coherent Multicore Subsystem"
#endif

#if !XCHAL_HAVE_PRID
    #error "SMP support requires PRID"
#endif

#if !XCHAL_HAVE_EXCLUSIVE
    #error "SMP support requires Exclusive Store to implement locks"
#endif

#if portUSING_MPU_WRAPPERS
    #error "SMP support requires FreeRTOS MPU wrappers to be off"
#endif

#if (XCHAL_SW_VERSION < 1505000)
    #error "SMP requires Xtensa toolchain RJ.5 or later"
#endif

#if !XCHAL_SUBSYS_IPI_NUM_SETS
    #error "SMP support requires at least one set of inter-processor interrupts (IPIs)"
#endif

#if (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C0_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C0 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C1_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C1 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 2) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C2_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C2 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 3) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C3_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C3 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 4) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C4_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C4 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 5) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C5_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C5 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 6) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C6_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C6 core interrupt is > XCHAL_EXCM_LEVEL
#endif
#if (configNUMBER_OF_CORES >= 7) && (XCHAL_INT_LEVEL(XCHAL_SUBSYS_IPI_S0C7_INTNUM) > XCHAL_EXCM_LEVEL)
#error IPI S0C7 core interrupt is > XCHAL_EXCM_LEVEL
#endif

#ifndef configTICK_CORE
    #define configTICK_CORE             0
#elif ( configTICK_CORE < 0 || configTICK_CORE >= configNUMBER_OF_CORES )
    #error "Invalid tick core specified in config!"
#endif
#if ( configUSE_CORE_AFFINITY == 1 )
    #define configTIMER_SERVICE_TASK_CORE_AFFINITY  ( 1 << configTICK_CORE )
#endif

    /* The Xtensa SMP port maintains an array of xt_internal_data_t structures,
     * which are padded to a cache line boundary.  This prevents cache thrashing
     * since various members are read or written by their own core.  If new fields
     * are added, XT_PERCORE_DATA_SIZE must be adjusted accordingly.
     */
#if (defined __DYNAMIC_REENT__)
    #define XT_PERCORE_REENT_DATA_SIZE  (8 + sizeof(struct _reent))
#else
    #define XT_PERCORE_REENT_DATA_SIZE  0
#endif
    #define XT_PERCORE_DATA_SIZE        (sizeof(UBaseType_t) + 20 + XT_PERCORE_REENT_DATA_SIZE)

    typedef struct xt_internal_data {
        uint32_t port_interruptNesting;     // First field for asm efficiency
        uint32_t port_switch_flag;
        UBaseType_t uxCriticalNestings;
        uint32_t xt_intenable;
        uint32_t xt_vpri_mask;
        uint32_t xt_core_init_done;
#if (defined __DYNAMIC_REENT__)
        struct _reent *xt_reent_p;          // When xclib defines _reent_ptr()
        void * xt_reent_pad_align;          // Ensure xt_reent is 16-byte aligned
        struct _reent xt_reent;
#endif
#if ( XT_USE_DATARAM == 0 )
        uint8_t  pad[XCHAL_DCACHE_LINESIZE -
                     (XT_PERCORE_DATA_SIZE & (XCHAL_DCACHE_LINESIZE - 1))];
#endif
    } xt_internal_data_t;

    static_assert( offsetof(xt_internal_data_t, port_interruptNesting) == 0,
            "Bad xt_internal_data field order" );
#if ( XT_USE_DATARAM == 0 )
    static_assert( ((sizeof(xt_internal_data_t) & (XCHAL_DCACHE_LINESIZE - 1)) == 0),
            "Incorrect xt_internal_data padding" );
#endif

    #define portGET_CORE_ID()           xthal_get_coreid()
    #define portYIELD_CORE(xCoreID)     xthal_ipi_trigger(xCoreID)
    #define portCRITICAL_NESTING_IN_TCB 0   // Nesting managed by port for SMP

    /* Mutex APIs for SMP locks -- based on XTOS implementation.
     * Requires Xtensa Exclusive Store and PRID options to be present.
     * Must reside in shared memory and declared statically (not on the stack).
     * Align and pad to cache line size for best performance.
     */
    typedef struct xt_mutex {
        uint32_t owner;
        uint32_t count;
        uint8_t  pad[XCHAL_DCACHE_LINESIZE - 2 * sizeof(uint32_t)];
    } xt_mutex;

    typedef xt_mutex *  xt_mutex_p;

    extern xt_mutex _xt_mutex_ISR;
    extern xt_mutex _xt_mutex_task;

    extern void xt_mutex_init(xt_mutex_p pmtx);
    extern int32_t xt_mutex_lock(xt_mutex_p pmtx);
    extern int32_t xt_mutex_unlock(xt_mutex_p pmtx);

    #define portGET_ISR_LOCK( xCoreID )         xt_mutex_lock(&_xt_mutex_ISR)
    #define portRELEASE_ISR_LOCK( xCoreID )     xt_mutex_unlock(&_xt_mutex_ISR)
    #define portGET_TASK_LOCK( xCoreID )        xt_mutex_lock(&_xt_mutex_task)
    #define portRELEASE_TASK_LOCK( xCoreID )    xt_mutex_unlock(&_xt_mutex_task)

    // Per-core data struct can be kept in dataram or indexed in shared sysram
    #if ( XT_USE_DATARAM )
    extern xt_internal_data_t _xt_intdata;
    #define _XT_INTDATA(...)                    (_xt_intdata)
    #else
    extern xt_internal_data_t _xt_intdata[ configNUMBER_OF_CORES ];
    #define _XT_INTDATA(c)                      (_xt_intdata[(c)])
    #endif

    // uxCriticalNestings maintained within per-core data
    #define portGET_CRITICAL_NESTING_COUNT( xCoreID )          ( _XT_INTDATA( xCoreID ).uxCriticalNestings )
    #define portSET_CRITICAL_NESTING_COUNT( xCoreID, x )       ( (_XT_INTDATA( xCoreID ).uxCriticalNestings) = ( x ) )
    #define portINCREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( (_XT_INTDATA( xCoreID ).uxCriticalNestings) ++ )
    #define portDECREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( (_XT_INTDATA( xCoreID ).uxCriticalNestings) -- )

    // port_interruptNesting maintained within per-core data
    #define portINCREMENT_INTERRUPT_NESTING_COUNT()   ( (_XT_INTDATA( portGET_CORE_ID() ).port_interruptNesting) ++ )
    #define portDECREMENT_INTERRUPT_NESTING_COUNT()   ( (_XT_INTDATA( portGET_CORE_ID() ).port_interruptNesting) -- )

    extern UBaseType_t vTaskEnterCriticalFromISR(void);
    extern void vTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus);
    #define portENTER_CRITICAL_FROM_ISR()   vTaskEnterCriticalFromISR()
    #define portEXIT_CRITICAL_FROM_ISR(x)   vTaskExitCriticalFromISR(x)

#else   // configNUMBER_OF_CORES

    /* The single-core Xtensa port maintains a single structure with interrupt-related
     * data structures, which for efficiency are accessed as offsets from a base 
     * structure pointer.
     */
    typedef struct xt_internal_data {
        uint32_t port_interruptNesting;     // First field for asm efficiency
        uint32_t port_switch_flag;
        uint32_t xt_intenable;
        uint32_t xt_vpri_mask;
#if (defined __DYNAMIC_REENT__)
        struct _reent *xt_reent_p;          // When xclib defines _reent_ptr()
#endif
    } xt_internal_data_t;

    static_assert( offsetof(xt_internal_data_t, port_interruptNesting) == 0, "Bad xt_internal_data field order" );

    #define portGET_CORE_ID()                         0
    #define portYIELD_CORE( xCoreID )                 UNUSED( xCoreID )
    #define portCRITICAL_NESTING_IN_TCB               1   // Nesting managed by FreeRTOS fine for 1 core

    #define portGET_ISR_LOCK( xCoreID )
    #define portRELEASE_ISR_LOCK( xCoreID )
    #define portGET_TASK_LOCK( xCoreID )
    #define portRELEASE_TASK_LOCK( xCoreID )

    extern xt_internal_data_t _xt_intdata;
    #define _XT_INTDATA(...)                          ( _xt_intdata )
    #define portINCREMENT_INTERRUPT_NESTING_COUNT()   ( _xt_intdata.port_interruptNesting++ )
    #define portDECREMENT_INTERRUPT_NESTING_COUNT()   ( _xt_intdata.port_interruptNesting-- )

#endif  // configNUMBER_OF_CORES
/*-----------------------------------------------------------*/

/* Fine resolution time */
#define portGET_RUN_TIME_COUNTER_VALUE()  xthal_get_ccount()

/* No need to do anything for the ccount timer. */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() do {} while (0)

/* Kernel utilities. */
BaseType_t xPortIsInsideInterrupt( void );
void vPortYield( void );
void vPortYieldFromInt( void );
void _frxt_setup_switch( void );
#define portYIELD()       vPortYield()
#define portYIELD_FROM_ISR( xHigherPriorityTaskWoken )	\
	if ( ( xHigherPriorityTaskWoken ) != 0 ) {	\
		_frxt_setup_switch();			\
	}

/* Tickless idle */
#if ( configUSE_TICKLESS_IDLE != 0 )
extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) \
        vPortSuppressTicksAndSleep( xExpectedIdleTime )
#endif

#endif //__ASSEMBLER__

/*-----------------------------------------------------------*/

/* Attributes for all the possible regions */

#define  portDFLT_MEM_TYPE              (XTHAL_MEM_BUFFERABLE | XTHAL_MEM_WRITEBACK)

#define  portDFLT_STACK_TYPE            portDFLT_MEM_TYPE
#define  portDFLT_STACK_ACCESS          XTHAL_AR_RWrw

#define  portDFLT_USERDEV_TYPE          XTHAL_MEM_DEVICE
#define  portDFLT_USERDEV_ACCESS        XTHAL_AR_RWXrwx

#define  portDFLT_KERNDEV_TYPE          XTHAL_MEM_DEVICE
#define  portDFLT_KERNDEV_ACCESS        XTHAL_AR_RWX

#define  portDFLT_SHARED_TYPE           portDFLT_MEM_TYPE
#define  portDFLT_SHARED_ACCESS         XTHAL_AR_RWXrwx

#define  portDFLT_KERNCODE_TYPE         portDFLT_MEM_TYPE
#define  portDFLT_KERNCODE_ACCESS       XTHAL_AR_RX

#define  portDFLT_KERNDATA_TYPE         portDFLT_MEM_TYPE
#define  portDFLT_KERNDATA_ACCESS       XTHAL_AR_RW

#define  portDFLT_UNUSED_MEM_TYPE       portDFLT_MEM_TYPE
#define  portDFLT_UNUSED_MEM_ACCESS     XTHAL_AR_RWXrx

#define portNUM_CONFIGURABLE_REGIONS    configNUM_CONFIGURABLE_REGIONS

#define portLEGACY_TASK_STACK_START     (((uint32_t)_bss_start) & -XCHAL_MPU_ALIGN)
#define portLEGACY_TASK_STACK_END       (((uint32_t)_bss_end + XCHAL_MPU_ALIGN - 1) & -XCHAL_MPU_ALIGN)

#ifndef __ASSEMBLER__

/* vPortStoreTaskMPUSettings returns void instead of BaseType_t. Until it's fixed
 * errors from that function can be checked using this global variable
 */
extern volatile int xtMPUError;

/* xtMPUError values
 */
typedef enum {
  MPU_ERR_SETUP_MPU = 1,
  MPU_ERR_REGION_NOT_IN_RANGE,
  MPU_ERR_STACK_NOT_IN_RANGE,
  MPU_ERR_OVERLAP_OTHER_REGIONS,
  MPU_ERR_FIXED_REGION_NOT_ALIGNED,
  MPU_ERR_PRIVATE_NOT_ALIGNED,
  MPU_ERR_INIT_ENTRY,
  MPU_ERR_INIT_ENTRY_NOT_IN_MAP,
  MPU_ERR_INIT_MAP_NOT_VALID
} mpu_err_t;


#endif

/*---------- DO NOT EDIT MPU SETTINGS BELOW ----------------*/

#if portUSING_MPU_WRAPPERS

#ifndef XCHAL_HAVE_MPU
# define XCHAL_HAVE_MPU 0
#endif

#if (XCHAL_HAVE_MPU == 0)
# error "MPU Hardware required!"
#endif

#ifndef portNUM_CONFIGURABLE_REGIONS
# define portNUM_CONFIGURABLE_REGIONS 0
#endif

#if defined(configUSER_DEVICE_START) && defined(configUSER_DEVICE_END)
#  define portUSE_USER_DEVICE_SPACE	1
# else
#  define portUSE_USER_DEVICE_SPACE	0
#endif

#if defined(configSHARED_DATA_START) && defined(configSHARED_DATA_END) && (portUSE_FIXED_MPU_ENTRIES == 1)
#  define portUSE_SHARED_DATA	1
# else
#  define portUSE_SHARED_DATA	0
#endif

#if defined(configPRIVILEGE_DEVICE_START) && defined(configPRIVILEGE_DEVICE_END)
#  define portUSE_PRIVILEGED_DEVICE_SPACE	1
# else
#  define portUSE_PRIVILEGED_DEVICE_SPACE	0
#endif

#if defined(configLEGACY_TASK_STACK_START) && defined(configLEGACY_TASK_STACK_END)
#  define portLEGACY_UNPRIVILEGED_TASKS 1
# else
#  define portLEGACY_UNPRIVILEGED_TASKS 0
#endif

/* Number of MPU entries a restricted thread will use. */
#define portNUM_USED_MPU_ENTRIES     (1  /* entry zero*/                   + \
                                      2* ( portNUM_CONFIGURABLE_REGIONS    + \
                                           portUSE_USER_DEVICE_SPACE       + \
                                           portUSE_PRIVILEGED_DEVICE_SPACE + \
                                           portUSE_SHARED_DATA             + \
                                           portLEGACY_UNPRIVILEGED_TASKS   + \
                                           1 /*stack*/                     + \
                                           1 /*FreeRTOS code */            + \
                                           1 /*FreeRTOS data */              \
                                           ))

#define portNUM_MAX_SWAPPED_MPU_PAIRS (portNUM_CONFIGURABLE_REGIONS + \
                                       portLEGACY_UNPRIVILEGED_TASKS + 1)

#if XCHAL_MPU_ENTRIES < 16
# error "MPU entries < 16 is not supported"
#endif
#if XCHAL_MPU_ENTRIES < portNUM_USED_MPU_ENTRIES
# error "Require MPU with at least portNUM_USED_MPU_ENTRIES foreground entries"
#endif
#if XCHAL_MPU_ALIGN > 16384
# error "MPU region alignment > 16384 is not supported"
#endif

#ifndef __ASSEMBLER__
extern uint32_t privileged_data_start[];
extern uint32_t privileged_data_end[];
extern uint32_t privileged_functions_start[];
extern uint32_t privileged_functions_end[];
extern uint32_t _bss_start[];
extern uint32_t _bss_end[];

#define portPRIVILEGED_CODE_START    ((uint32_t)privileged_functions_start & (uint32_t)-XCHAL_MPU_ALIGN)
#define portPRIVILEGED_CODE_END      (((uint32_t)privileged_functions_end + (XCHAL_MPU_ALIGN - 1)) & (uint32_t)-XCHAL_MPU_ALIGN)
#define portPRIVILEGED_DATA_START    ((uint32_t)privileged_data_start & (uint32_t)-XCHAL_MPU_ALIGN)
#define portPRIVILEGED_DATA_END      (((uint32_t)privileged_data_end + (XCHAL_MPU_ALIGN - 1)) & (uint32_t)-XCHAL_MPU_ALIGN)

typedef struct {
    // Define here mpu_settings, which is port dependent
    xthal_MPU_entry mpumap[portNUM_MAX_SWAPPED_MPU_PAIRS][2];
} xMPU_SETTINGS;

#endif //ASSEMBLER

#endif //portUSING_MPU_WRAPPERS

/*-----------------------------------------------------------*/

#ifndef __ASSEMBLER__

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

// porttrace
#if configUSE_TRACE_FACILITY_2
#include "porttrace.h"
#endif

// configASSERT_2 if requested
#if configASSERT_2
#include <stdio.h>
void exit(int);
#define configASSERT( x )   if (!(x)) { porttracePrint(-1); printf("\nAssertion failed in %s:%d\n", __FILE__, __LINE__); exit(-1); }
#endif

/* Task delete hook for cleanup. May be an issue if tracing is also needed. */
#define traceTASK_DELETE(pxTCB)    portTaskDeleteHook(pxTCB->pxEndOfStack)

static inline void portTaskDeleteHook(void * ptr)
{
#if XCHAL_CP_NUM > 0
    /* Release any owned coprocessors. If the owner save area address
       matches the address in 'ptr' then we are the owner, set to NULL. */
    if (ptr != NULL) {
        extern void * _xt_coproc_owner_sa[XCHAL_CP_MAX];
        unsigned int i;

        for (i = 0; i < XCHAL_CP_MAX; i++) {
            if (_xt_coproc_owner_sa[i] == ptr) {
                _xt_coproc_owner_sa[i] = NULL;
            }
#if (defined XT_DEBUG)
            /* Only for testing / debugging */
            if (_xt_coproc_owner_sa[i] == (void *)0xf1f1f1f1) {
                _xt_coproc_owner_sa[i] = NULL;
            }
#endif
        }
    }
#else
    UNUSED(ptr);
#endif
}

/* C library support -- only XCLIB and NEWLIB are supported. */

/* To enable thread-safe C library support, XT_USE_THREAD_SAFE_CLIB must be
   defined to be > 0 somewhere above or on the command line. */

#if (XT_USE_THREAD_SAFE_CLIB > 0u) && ((XSHAL_CLIB == XTHAL_CLIB_XCLIB) || (XSHAL_CLIB == XTHAL_CLIB_NEWLIB))
extern void vPortClibInit(void);
#endif // XCLIB || NEWLIB support

#endif // __ASSEMBLER__

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
