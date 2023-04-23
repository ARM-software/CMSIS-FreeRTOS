/*
 * freertos_os2_control_blocks.h
 *
 * Copyright 2022 Span.IO Inc.
 *
 *  Created on: Oct 25, 2022
 *      Author: sgc
 */

#pragma once

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "freertos_mpool.h"
#include "semphr.h"

#define osTickRateHz (configTICK_RATE_HZ)
#define osMaxDelayTicks (portMAX_DELAY)

typedef MemPool_t MemoryPoolControlBlock;
typedef StaticQueue_t MessageQueueControlBlock;

struct TimerControlBlock {
  StaticTimer_t m_static;

  // These are TimerCallback_t, defined inside cmsis_os2.c
  osTimerFunc_t m_timer_callback;
  void* m_arg;
};

typedef StaticSemaphore_t MutexControlBlock;
typedef StaticSemaphore_t SemaphoreControlBlock;
