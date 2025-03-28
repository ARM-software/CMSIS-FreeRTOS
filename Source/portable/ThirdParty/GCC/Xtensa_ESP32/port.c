/*
 * SPDX-FileCopyrightText: 2020 Amazon.com, Inc. or its affiliates
 * SPDX-FileCopyrightText: 2015-2019 Cadence Design Systems, Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */

/*
 * FreeRTOS Kernel V11.2.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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
 * 1 tab == 4 spaces!
 */

/*
 * Copyright (c) 2015-2019 Cadence Design Systems, Inc.
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

#include <stdlib.h>
#include <string.h>
#include <xtensa/config/core.h>

#include "xtensa_rtos.h"
#include "esp_idf_version.h"

#if ( ESP_IDF_VERSION < ESP_IDF_VERSION_VAL( 4, 2, 0 ) )
    #include "rom/ets_sys.h"
    #include "esp_panic.h"
    #include "esp_crosscore_int.h"
#else
    #if CONFIG_IDF_TARGET_ESP32S3
        #include "esp32s3/rom/ets_sys.h"
    #elif CONFIG_IDF_TARGET_ESP32S2
        #include "esp32s2/rom/ets_sys.h"
    #elif CONFIG_IDF_TARGET_ESP32
        #include "esp32/rom/ets_sys.h"
    #endif
    #include "esp_private/panic_reason.h"
    #include "esp_debug_helpers.h"
    #include "esp_private/crosscore_int.h"
    #include "esp_log.h"
#endif /* ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0) */
#include "soc/cpu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "esp_heap_caps.h"

#include "esp_intr_alloc.h"

#include "port_systick.h"

/* Defined in xtensa_context.S */
extern void _xt_coproc_init( void );

_Static_assert( tskNO_AFFINITY == CONFIG_FREERTOS_NO_AFFINITY, "incorrect tskNO_AFFINITY value" );

/*-----------------------------------------------------------*/

extern volatile int port_xSchedulerRunning[ portNUM_PROCESSORS ];
unsigned port_interruptNesting[ portNUM_PROCESSORS ] = { 0 }; /* Interrupt nesting level. Increased/decreased in portasm.c, _frxt_int_enter/_frxt_int_exit */

/*-----------------------------------------------------------*/

/* User exception dispatcher when exiting */
void _xt_user_exit( void );

#if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
/* Wrapper to allow task functions to return (increases stack overhead by 16 bytes) */
    static void vPortTaskWrapper( TaskFunction_t pxCode,
                                  void * pvParameters )
    {
        pxCode( pvParameters );
        /*FreeRTOS tasks should not return. Log the task name and abort. */
        char * pcTaskName = pcTaskGetTaskName( NULL );
        ESP_LOGE( "FreeRTOS", "FreeRTOS Task \"%s\" should not return, Aborting now!", pcTaskName );
        abort();
    }
#endif /* if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER */

/*
 * Stack initialization
 */
/* *INDENT-OFF* */
#if portUSING_MPU_WRAPPERS
    StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                         TaskFunction_t pxCode,
                                         void * pvParameters,
                                         BaseType_t xRunPrivileged )
#else
    StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                         TaskFunction_t pxCode,
                                         void * pvParameters )
#endif
/* *INDENT-ON* */
{
    StackType_t * sp;
    StackType_t * tp;
    XtExcFrame * frame;

    #if XCHAL_CP_NUM > 0
        uint32_t * p;
    #endif

    uint32_t * threadptr;
    void * task_thread_local_start;
    extern int _thread_local_start, _thread_local_end, _flash_rodata_start, _flash_rodata_align;

    /* TODO: check that TLS area fits the stack */
    uint32_t thread_local_sz = ( uint8_t * ) &_thread_local_end - ( uint8_t * ) &_thread_local_start;

    thread_local_sz = ALIGNUP( 0x10, thread_local_sz );

    /* Initialize task's stack so that we have the following structure at the top:
     *
     *  ----LOW ADDRESSES ----------------------------------------HIGH ADDRESSES----------
     *   task stack | interrupt stack frame | thread local vars | co-processor save area |
     *  ----------------------------------------------------------------------------------
     |                                                                    |
     |              SP                                                             pxTopOfStack
     |
     |  All parts are aligned to 16 byte boundary.
     */

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = ( StackType_t * ) ( ( ( UBaseType_t ) pxTopOfStack - XT_CP_SIZE - thread_local_sz - XT_STK_FRMSZ ) & ~0xf );

    /* Clear the entire frame (do not use memset() because we don't depend on C library) */
    for( tp = sp; tp <= pxTopOfStack; ++tp )
    {
        *tp = 0;
    }

    frame = ( XtExcFrame * ) sp;

    /* Explicitly initialize certain saved registers */
    #if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
        frame->pc = ( UBaseType_t ) vPortTaskWrapper; /* task wrapper                       */
    #else
        frame->pc = ( UBaseType_t ) pxCode;           /* task entrypoint                    */
    #endif
    frame->a0 = 0;                                    /* to terminate GDB backtrace     */
    frame->a1 = ( UBaseType_t ) sp + XT_STK_FRMSZ;    /* physical top of stack frame        */
    frame->exit = ( UBaseType_t ) _xt_user_exit;      /* user exception exit dispatcher */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    /* Also set entry point argument parameter. */
    #ifdef __XTENSA_CALL0_ABI__
        #if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
            frame->a2 = ( UBaseType_t ) pxCode;
            frame->a3 = ( UBaseType_t ) pvParameters;
        #else
            frame->a2 = ( UBaseType_t ) pvParameters;
        #endif
        frame->ps = PS_UM | PS_EXCM;
    #else
        /* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
        #if CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER
            frame->a6 = ( UBaseType_t ) pxCode;
            frame->a7 = ( UBaseType_t ) pvParameters;
        #else
            frame->a6 = ( UBaseType_t ) pvParameters;
        #endif
        frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC( 1 );
    #endif /* ifdef __XTENSA_CALL0_ABI__ */

    #ifdef XT_USE_SWPRI
        /* Set the initial virtual priority mask value to all 1's. */
        frame->vpri = 0xFFFFFFFF;
    #endif

    /* Init threadptr register and set up TLS run-time area. */
    task_thread_local_start = ( void * ) ( ( ( uint32_t ) pxTopOfStack - XT_CP_SIZE - thread_local_sz ) & ~0xf );
    memcpy( task_thread_local_start, &_thread_local_start, thread_local_sz );
    threadptr = ( uint32_t * ) ( sp + XT_STK_EXTRA );

    /* Calculate THREADPTR value.
     * The generated code will add THREADPTR value to a constant value determined at link time,
     * to get the address of the TLS variable.
     * The constant value is calculated by the linker as follows
     * (search for 'tpoff' in elf32-xtensa.c in BFD):
     *    offset = address - tls_section_vma + align_up(TCB_SIZE, tls_section_alignment)
     * where TCB_SIZE is hardcoded to 8.
     */
    const uint32_t tls_section_alignment = ( uint32_t ) &_flash_rodata_align; /* ALIGN value of .flash.rodata section */
    const uint32_t tcb_size = 8;                                              /* Unrelated to FreeRTOS, this is the constant from BFD */
    const uint32_t base = ( tcb_size + tls_section_alignment - 1 ) & ( ~( tls_section_alignment - 1 ) );
    *threadptr = ( uint32_t ) task_thread_local_start - ( ( uint32_t ) &_thread_local_start - ( uint32_t ) &_flash_rodata_start ) - base;

    #if XCHAL_CP_NUM > 0
        /* Init the coprocessor save area (see xtensa_context.h) */

        /* No access to TCB here, so derive indirectly. Stack growth is top to bottom.
         * //p = (uint32_t *) xMPUSettings->coproc_area;
         */
        p = ( uint32_t * ) ( ( ( uint32_t ) pxTopOfStack - XT_CP_SIZE ) & ~0xf );
        configASSERT( ( uint32_t ) p >= frame->a1 );
        p[ 0 ] = 0;
        p[ 1 ] = 0;
        p[ 2 ] = ( ( ( uint32_t ) p ) + 12 + XCHAL_TOTAL_SA_ALIGN - 1 ) & -XCHAL_TOTAL_SA_ALIGN;
    #endif

    return sp;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* It is unlikely that the Xtensa port will get stopped.  If required simply
     * disable the tick interrupt here. */
    abort();
}

/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
    portDISABLE_INTERRUPTS();
    /* Interrupts are disabled at this point and stack contains PS with enabled interrupts when task context is restored */

    #if XCHAL_CP_NUM > 0
        /* Initialize co-processor management for tasks. Leave CPENABLE alone. */
        _xt_coproc_init();
    #endif

    /* Setup the hardware to generate the tick */
    vPortSetupTimer();

    /* NOTE: For ESP32-S3, vPortSetupTimer allocates an interrupt for the
     * systimer which is used as the source for FreeRTOS systick.
     *
     * The behaviour of portEXIT_CRITICAL is different in FreeRTOS and ESP-IDF -
     * the former enables the interrupts no matter what the state was at the beginning
     * of the call while the latter restores the interrupt state to what was at the
     * beginning of the call.
     *
     * This resulted in the interrupts being enabled before the _frxt_dispatch call,
     * who was unable to switch context to the queued tasks.
     */
    portDISABLE_INTERRUPTS();

    port_xSchedulerRunning[ xPortGetCoreID() ] = 1;

    /* Cannot be directly called from C; never returns */
    __asm__ volatile ( "call0    _frxt_dispatch\n" );

    /* Should not get here. */
    return pdTRUE;
}

/*-----------------------------------------------------------*/

void vPortYieldOtherCore( BaseType_t coreid )
{
    esp_crosscore_int_send_yield( coreid );
}

/*-----------------------------------------------------------*/

/*
 * Used to set coprocessor area in stack. Current hack is to reuse MPU pointer for coprocessor area.
 */
#if portUSING_MPU_WRAPPERS
    void vPortStoreTaskMPUSettings( xMPU_SETTINGS * xMPUSettings,
                                    const struct xMEMORY_REGION * const xRegions,
                                    StackType_t * pxBottomOfStack,
                                    configSTACK_DEPTH_TYPE uxStackDepth )
    {
        #if XCHAL_CP_NUM > 0
            xMPUSettings->coproc_area = ( StackType_t * ) ( ( uint32_t ) ( pxBottomOfStack + uxStackDepth - 1 ) );
            xMPUSettings->coproc_area = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) xMPUSettings->coproc_area ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
            xMPUSettings->coproc_area = ( StackType_t * ) ( ( ( uint32_t ) xMPUSettings->coproc_area - XT_CP_SIZE ) & ~0xf );

            /* NOTE: we cannot initialize the coprocessor save area here because FreeRTOS is going to
             * clear the stack area after we return. This is done in pxPortInitialiseStack().
             */
        #endif
    }

    void vPortReleaseTaskMPUSettings( xMPU_SETTINGS * xMPUSettings )
    {
        /* If task has live floating point registers somewhere, release them */
        _xt_coproc_release( xMPUSettings->coproc_area );
    }

#endif /* if portUSING_MPU_WRAPPERS */

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
BaseType_t xPortInIsrContext()
{
    unsigned int irqStatus;
    BaseType_t ret;

    irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    ret = ( port_interruptNesting[ xPortGetCoreID() ] != 0 );
    portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
    return ret;
}

/*
 * This function will be called in High prio ISRs. Returns true if the current core was in ISR context
 * before calling into high prio ISR context.
 */
BaseType_t IRAM_ATTR xPortInterruptedFromISRContext()
{
    return( port_interruptNesting[ xPortGetCoreID() ] != 0 );
}

void IRAM_ATTR vPortEvaluateYieldFromISR( int argc,
                                          ... )
{
    BaseType_t xYield;
    va_list ap;

    va_start( ap, argc );

    if( argc )
    {
        xYield = ( BaseType_t ) va_arg( ap, int );
        va_end( ap );
    }
    else
    {
        /*it is a empty parameter vPortYieldFromISR macro call: */
        va_end( ap );
        traceISR_EXIT_TO_SCHEDULER();
        _frxt_setup_switch();
        return;
    }

    /*Yield exists, so need evaluate it first then switch: */
    if( xYield == pdTRUE )
    {
        traceISR_EXIT_TO_SCHEDULER();
        _frxt_setup_switch();
    }
}

void vPortAssertIfInISR()
{
    if( xPortInIsrContext() )
    {
        esp_rom_printf( "core=%d port_interruptNesting=%d\n\n", xPortGetCoreID(), port_interruptNesting[ xPortGetCoreID() ] );
    }

    configASSERT( !xPortInIsrContext() );
}

/*
 * For kernel use: Initialize a per-CPU mux. Mux will be initialized unlocked.
 */
void vPortCPUInitializeMutex( portMUX_TYPE * mux )
{
    #ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
        esp_rom_printf( "Initializing mux %p\n", mux );
        mux->lastLockedFn = "(never locked)";
        mux->lastLockedLine = -1;
    #endif
    mux->owner = portMUX_FREE_VAL;
    mux->count = 0;
}

#include "portmux_impl.h"

/*
 * For kernel use: Acquire a per-CPU mux. Spinlocks, so don't hold on to these muxes for too long.
 */
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
    void vPortCPUAcquireMutex( portMUX_TYPE * mux,
                               const char * fnName,
                               int line )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

        vPortCPUAcquireMutexIntsDisabled( mux, portMUX_NO_TIMEOUT, fnName, line );
        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
    }

    bool vPortCPUAcquireMutexTimeout( portMUX_TYPE * mux,
                                      int timeout_cycles,
                                      const char * fnName,
                                      int line )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        bool result = vPortCPUAcquireMutexIntsDisabled( mux, timeout_cycles, fnName, line );

        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
        return result;
    }

#else /* ifdef CONFIG_FREERTOS_PORTMUX_DEBUG */
    void vPortCPUAcquireMutex( portMUX_TYPE * mux )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

        vPortCPUAcquireMutexIntsDisabled( mux, portMUX_NO_TIMEOUT );
        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
    }

    bool vPortCPUAcquireMutexTimeout( portMUX_TYPE * mux,
                                      int timeout_cycles )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();
        bool result = vPortCPUAcquireMutexIntsDisabled( mux, timeout_cycles );

        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
        return result;
    }
#endif /* ifdef CONFIG_FREERTOS_PORTMUX_DEBUG */


/*
 * For kernel use: Release a per-CPU mux
 *
 * Mux must be already locked by this core
 */
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
    void vPortCPUReleaseMutex( portMUX_TYPE * mux,
                               const char * fnName,
                               int line )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

        vPortCPUReleaseMutexIntsDisabled( mux, fnName, line );
        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
    }
#else
    void vPortCPUReleaseMutex( portMUX_TYPE * mux )
    {
        unsigned int irqStatus = portSET_INTERRUPT_MASK_FROM_ISR();

        vPortCPUReleaseMutexIntsDisabled( mux );
        portCLEAR_INTERRUPT_MASK_FROM_ISR( irqStatus );
    }
#endif /* ifdef CONFIG_FREERTOS_PORTMUX_DEBUG */

#define STACK_WATCH_AREA_SIZE       ( 32 )
#define STACK_WATCH_POINT_NUMBER    ( SOC_CPU_WATCHPOINTS_NUM - 1 )

void vPortSetStackWatchpoint( void * pxStackStart )
{
    /*Set watchpoint 1 to watch the last 32 bytes of the stack. */
    /*Unfortunately, the Xtensa watchpoints can't set a watchpoint on a random [base - base+n] region because */
    /*the size works by masking off the lowest address bits. For that reason, we futz a bit and watch the lowest 32 */
    /*bytes of the stack we can actually watch. In general, this can cause the watchpoint to be triggered at most */
    /*28 bytes early. The value 32 is chosen because it's larger than the stack canary, which in FreeRTOS is 20 bytes. */
    /*This way, we make sure we trigger before/when the stack canary is corrupted, not after. */
    int addr = ( int ) pxStackStart;

    addr = ( addr + 31 ) & ( ~31 );
    esp_cpu_set_watchpoint( STACK_WATCH_POINT_NUMBER, ( char * ) addr, 32, ESP_WATCHPOINT_STORE );
}

#if ( ESP_IDF_VERSION < ESP_IDF_VERSION_VAL( 4, 2, 0 ) )

    #if defined( CONFIG_SPIRAM_SUPPORT )

/*
 * Compare & set (S32C1) does not work in external RAM. Instead, this routine uses a mux (in internal memory) to fake it.
 */
        static portMUX_TYPE extram_mux = portMUX_INITIALIZER_UNLOCKED;

        void uxPortCompareSetExtram( volatile uint32_t * addr,
                                     uint32_t compare,
                                     uint32_t * set )
        {
            uint32_t prev;

            uint32_t oldlevel = portSET_INTERRUPT_MASK_FROM_ISR();

            #ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
                vPortCPUAcquireMutexIntsDisabled( &extram_mux, portMUX_NO_TIMEOUT, __FUNCTION__, __LINE__ );
            #else
                vPortCPUAcquireMutexIntsDisabled( &extram_mux, portMUX_NO_TIMEOUT );
            #endif
            prev = *addr;

            if( prev == compare )
            {
                *addr = *set;
            }

            *set = prev;
            #ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
                vPortCPUReleaseMutexIntsDisabled( &extram_mux, __FUNCTION__, __LINE__ );
            #else
                vPortCPUReleaseMutexIntsDisabled( &extram_mux );
            #endif

            portCLEAR_INTERRUPT_MASK_FROM_ISR( oldlevel );
        }
    #endif //defined(CONFIG_SPIRAM_SUPPORT)

#endif /* ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0) */


uint32_t xPortGetTickRateHz( void )
{
    return ( uint32_t ) configTICK_RATE_HZ;
}

/* For now, running FreeRTOS on one core and a bare metal on the other (or other OSes) */
/* is not supported. For now CONFIG_FREERTOS_UNICORE and CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE */
/* should mirror each other's values. */
/* */
/* And since this should be true, we can just check for CONFIG_FREERTOS_UNICORE. */
#if CONFIG_FREERTOS_UNICORE != CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    #error "FreeRTOS and system configuration mismatch regarding the use of multiple cores."
#endif

extern void esp_startup_start_app_common( void );

void esp_startup_start_app( void )
{
    #if !CONFIG_ESP_INT_WDT
        #if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
            assert( !soc_has_cache_lock_bug() && "ESP32 Rev 3 + Dual Core + PSRAM requires INT WDT enabled in project config!" );
        #endif
    #endif

    esp_startup_start_app_common();

    ESP_LOGI( "cpu_start", "Starting scheduler on PRO CPU." );
    vTaskStartScheduler();
}
