 /*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2019-2026 Cadence Design Systems, Inc.
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

/*
 * Xtensa-specific FreeRTOS interface to the libidma OS functions. This allows
 * FreeRTOS threads to use iDMA channels safely. If you wish to provide your
 * own implementation then this one is easily overridden from application code.
 * However make sure to provide a full replacement for the public API.
 */

#include <xtensa/hal.h>
#include <xtensa/config/core.h>

#if XCHAL_HAVE_IDMA

#include <xtensa/idma.h>

#include "FreeRTOS.h"
#include "xtensa_api.h"

#include "task.h"
#include "semphr.h"

#if (defined INCLUDE_xTaskGetCurrentTaskHandle) && (INCLUDE_xTaskGetCurrentTaskHandle)

//-----------------------------------------------------------------------------
//  Allow one buffer per channel per thread. Normally this is in excess of what
//  is actually needed, so adjust as necessary. Most threads will end up using
//  only one channel at a time, and not all threads in an application even use
//  iDMA.
//
//  Thread-local storage is currently not used here because the assumption is
//  that most threads will not use iDMA, so it is not useful to tie up one or
//  more TLS slots in every thread.
//
//  Since FreeRTOS task suspend/resume APIs are not interrupt-safe, a
//  counting semaphore implements the block/unblock APIs.  Recommend enabling
//  configSUPPORT_STATIC_ALLOCATION to speed up semaphore creation.
//-----------------------------------------------------------------------------

typedef struct {
    idma_buf_t * buf_list[XCHAL_IDMA_NUM_CHANNELS];
    TaskHandle_t thread;
#if ( configNUMBER_OF_CORES > 1 )
    uint32_t core;
#endif
#if ( configSUPPORT_STATIC_ALLOCATION )
    StaticSemaphore_t sem_buf;
#endif
    SemaphoreHandle_t sem_handle;
} xt_idma_buf_info_t;

//-----------------------------------------------------------------------------
//  Statically allocate some number of thread buffer info structs. Adjust this
//  as needed. For large values, the linear searches will not be efficient but
//  we don't expect MAX_THREADS to be large. Besides, channel buffer changes
//  should be relatively infrequent.
//
//  On multicore systems, this structure is also protected by a mutex.
//-----------------------------------------------------------------------------

#define MAX_THREADS     8

static ALIGNDCACHE xt_idma_buf_info_t xt_idma_buf_info[MAX_THREADS];

#if ( configNUMBER_OF_CORES > 1 )
#if ( configSUPPORT_STATIC_ALLOCATION )
static StaticSemaphore_t xt_idma_mtx_buf;
#endif
static SemaphoreHandle_t xt_idma_mtx_handle = NULL;
#endif


//-----------------------------------------------------------------------------
//  idma_register_interrupts
//-----------------------------------------------------------------------------
int32_t
idma_register_interrupts(int32_t ch, os_handler done_handler, os_handler err_handler)
{
#if XCHAL_HAVE_INTERRUPTS
    if (ch < XCHAL_IDMA_NUM_CHANNELS) {
        uint32_t doneint = (uint32_t) XCHAL_IDMA_CH0_DONE_INTERRUPT + (uint32_t) ch;
        uint32_t errint  = (uint32_t) XCHAL_IDMA_CH0_ERR_INTERRUPT + (uint32_t) ch;
        int32_t  ret     = 0;
        xt_handler oldh;

        oldh = xt_set_interrupt_handler(doneint, done_handler, cvt_int32_to_voidp(ch));
        ret += (oldh == NULL) ? 1 : 0;
        xt_interrupt_enable(doneint);

        oldh = xt_set_interrupt_handler(errint, err_handler, cvt_int32_to_voidp(ch));
        ret += (oldh == NULL) ? 1 : 0;
        xt_interrupt_enable(errint);

        return (ret == 0) ? 0 : -1;
    }
#endif
    return -1;
}


//-----------------------------------------------------------------------------
//  idma_disable_interrupts
//-----------------------------------------------------------------------------
uint32_t
idma_disable_interrupts(void)
{
    return xthal_disable_interrupts();
}


//-----------------------------------------------------------------------------
//  idma_enable_interrupts
//-----------------------------------------------------------------------------
void
idma_enable_interrupts(uint32_t level)
{
    xthal_restore_interrupts(level);
}


//-----------------------------------------------------------------------------
//  idma_thread_id
//-----------------------------------------------------------------------------
void *
idma_thread_id(void)
{
    return xTaskGetCurrentTaskHandle();
}


//-----------------------------------------------------------------------------
//  idma_thread_block
//-----------------------------------------------------------------------------
void
idma_thread_block(void * thread)
{
    if (thread != NULL) {
        int32_t i;
        for (i = 0; i < MAX_THREADS; i++) {
            if (xt_idma_buf_info[i].thread == thread) {
                break;
            }
        }
        configASSERT (i < MAX_THREADS);
        xSemaphoreTake(xt_idma_buf_info[i].sem_handle, portMAX_DELAY);
    }
}


//-----------------------------------------------------------------------------
//  idma_thread_unblock
//-----------------------------------------------------------------------------
void
idma_thread_unblock(void * thread)
{
    if (thread != NULL) {
        int32_t i;
        for (i = 0; i < MAX_THREADS; i++) {
            if (xt_idma_buf_info[i].thread == thread) {
                break;
            }
        }
        configASSERT (i < MAX_THREADS);
        xSemaphoreGive(xt_idma_buf_info[i].sem_handle);
    }
}


//-----------------------------------------------------------------------------
//  idma_chan_buf_set
//-----------------------------------------------------------------------------
void
idma_chan_buf_set(int32_t ch, idma_buf_t * buf)
{
    if (ch < XCHAL_IDMA_NUM_CHANNELS) {
        TaskHandle_t thread = xTaskGetCurrentTaskHandle();
        int32_t     j      = -1;
        int32_t     i;
        uint32_t    ps;

#if ( configNUMBER_OF_CORES > 1 )
        if (!xt_idma_mtx_handle) {
#if ( configSUPPORT_STATIC_ALLOCATION )
            xt_idma_mtx_handle = xSemaphoreCreateMutexStatic(&xt_idma_mtx_buf);
#else
            xt_idma_mtx_handle = xSemaphoreCreateMutex();
#endif
        }
        xSemaphoreTake(xt_idma_mtx_handle, portMAX_DELAY);
#endif
        // Find a free slot and allocate it. This requires interrupts
        // disabled because we're manipulating global shared data.
        ps = xthal_disable_interrupts();

        for (i = 0; i < MAX_THREADS; i++) {
            if (xt_idma_buf_info[i].thread == thread) {
                break;
            }
            if ((xt_idma_buf_info[i].thread == NULL) && (j < 0)) {
                j = i;
            }
        }
        if (i < MAX_THREADS) {
            xt_idma_buf_info[i].buf_list[ch] = buf;
        }
        else if (j >= 0) {
            xt_idma_buf_info[j].thread = thread;
            xt_idma_buf_info[j].buf_list[ch] = buf;
#if ( configNUMBER_OF_CORES > 1 )
            xt_idma_buf_info[j].core = portGET_CORE_ID();
#endif
#if ( configSUPPORT_STATIC_ALLOCATION )
            xt_idma_buf_info[j].sem_handle = 
                    xSemaphoreCreateCountingStatic(1, 0, &xt_idma_buf_info[j].sem_buf);
#else
            xt_idma_buf_info[j].sem_handle = xSemaphoreCreateCounting(1, 0);
#endif
        }
        else {
            configASSERT(0);
        }

        xthal_restore_interrupts(ps);
#if ( configNUMBER_OF_CORES > 1 )
        xSemaphoreGive(xt_idma_mtx_handle);
#endif
    }
}


//-----------------------------------------------------------------------------
//  idma_chan_buf_get
//-----------------------------------------------------------------------------
idma_buf_t *
idma_chan_buf_get(int32_t ch)
{
    if (ch < XCHAL_IDMA_NUM_CHANNELS) {
        TaskHandle_t thread = xTaskGetCurrentTaskHandle();
        int32_t i;

        for (i = 0; i < MAX_THREADS; i++) {
            if (xt_idma_buf_info[i].thread == thread) {
#if ( configNUMBER_OF_CORES > 1 )
                // Confirm thread has not changed cores
                configASSERT(xt_idma_buf_info[i].core == portGET_CORE_ID());
#endif
                return xt_idma_buf_info[i].buf_list[ch];
            }
        }
    }

    return NULL;
}


//-----------------------------------------------------------------------------
//  idma_chan_buf_clear
//-----------------------------------------------------------------------------
void
idma_chan_buf_clear(int32_t ch)
{
    if (ch < XCHAL_IDMA_NUM_CHANNELS) {
        TaskHandle_t thread = xTaskGetCurrentTaskHandle();
        int32_t     i;
        int32_t     j;
        uint32_t    ps;
#if !( configSUPPORT_STATIC_ALLOCATION )
        SemaphoreHandle_t sem_to_delete = NULL;
#endif

#if ( configNUMBER_OF_CORES > 1 )
        xSemaphoreTake(xt_idma_mtx_handle, portMAX_DELAY);
#endif
        // The following requires interrupts disabled because we're
        // manipulating global shared data.
        ps = xthal_disable_interrupts();

        for (i = 0; i < MAX_THREADS; i++) {
            if (xt_idma_buf_info[i].thread == thread) {
                // Clear the slot buffer for the channel.
                xt_idma_buf_info[i].buf_list[ch] = NULL;

                // If this slot is now unused, free it up.
                for (j = 0; j < XCHAL_IDMA_NUM_CHANNELS; j++) {
                    if (xt_idma_buf_info[i].buf_list[j] != NULL) {
                        break;
                    }
                }
                if (j == XCHAL_IDMA_NUM_CHANNELS) {
#if ( configNUMBER_OF_CORES > 1 )
                    // Confirm thread has not changed cores
                    configASSERT(xt_idma_buf_info[i].core == portGET_CORE_ID());
#endif
#if !( configSUPPORT_STATIC_ALLOCATION )
                    // Delete sem after releasing lock and enabling interrupts
                    sem_to_delete = xt_idma_buf_info[i].sem_handle;
#endif
                    xt_idma_buf_info[i].sem_handle = NULL;
                    xt_idma_buf_info[i].thread = NULL;
                }

                break;
            }
        }

        xthal_restore_interrupts(ps);
#if ( configNUMBER_OF_CORES > 1 )
        xSemaphoreGive(xt_idma_mtx_handle);
#endif
#if !( configSUPPORT_STATIC_ALLOCATION )
        if (sem_to_delete != NULL) {
            vSemaphoreDelete(sem_to_delete);
        }
#endif
    }
}

#else

#warn INCLUDE_xTaskGetCurrentTaskHandle required for threaded iDMA support

#endif // (defined INCLUDE_xTaskGetCurrentTaskHandle) && (INCLUDE_xTaskGetCurrentTaskHandle)

#endif // XCHAL_HAVE_IDMA

