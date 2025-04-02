# Function Overview {#page_function_overview}

## CMSIS-RTOS2 API

Overview of all CMSIS-RTOS C API v2 functions that are implemented in CMSIS-FreeRTOS.

### Kernel Information and Control

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__KernelCtrl.html) for details about Kernel Information and Control functions.

- **osKernelInitialize:** supported
- **osKernelGetInfo:** supported
- **osKernelGetState:** supported
- **osKernelStart:** supported
- **osKernelLock:** supported
- **osKernelUnlock:** supported
- **osKernelRestoreLock:** supported
- **osKernelSuspend:** *not implemented*
- **osKernelResume:** *not implemented*
- **osKernelProtect:** *not implemented*
- **osKernelDestroyClass:** *not implemented*
- **osKernelGetTickCount:** supported
- **osKernelGetTickFreq:** supported
- **osKernelGetSysTimerCount:** supported
- **osKernelGetSysTimerFreq:** supported

### Thread Management

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__ThreadMgmt.html) for details about Thread Management functions.

- **osThreadNew:** supported
- **osThreadGetName:** supported
- **osThreadGetClass:** *not implemented*
- **osThreadGetZone:** *not implemented*
- **osThreadGetId:** supported
- **osThreadGetState:** supported
- **osThreadGetStackSize:** *not implemented*
- **osThreadGetStackSpace:** supported
- **osThreadSetPriority:** supported
- **osThreadGetPriority:** supported
- **osThreadYield:** supported
- **osThreadSuspend:** supported
- **osThreadResume:** supported
- **osThreadDetach:** *not implemented*
- **osThreadJoin:** *not implemented*
- **osThreadExit:** supported
- **osThreadTerminate:** supported
- **osThreadFeedWatchdog:** *not implemented*
- **osThreadProtectPrivileged:** *not implemented*
- **osThreadSuspendClass:** *not implemented*
- **osThreadResumeClass:** *not implemented*
- **osThreadTerminateZone:** *not implemented*
- **osThreadGetCount:** supported
- **osThreadEnumerate:** supported

### Thread Flags

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__ThreadFlagsMgmt.html) for details about Thread Flags functions.

- **osThreadFlagsSet:** supported
- **osThreadFlagsClear:** supported
- **osThreadFlagsGet:** supported
- **osThreadFlagsWait:** supported

### Generic Wait Functions

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__Wait.html) for details about Generic Wait functions.

- **osDelay:** supported
- **osDelayUntil:** supported

### Timer Management

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__TimerMgmt.html) for details about Timer Management functions.

- **osTimerNew:** supported
- **osTimerGetName:** supported
- **osTimerStart:** supported
- **osTimerStop:** supported
- **osTimerIsRunning:** supported
- **osTimerDelete:** supported

### Event Flags

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__EventFlags.html) for details about Event Flags functions.
All event flags are limited to 24 bits.

- **osEventFlagsNew:** supported
- **osEventFlagsGetName:** *not implemented*
- **osEventFlagsSet:** supported
- **osEventFlagsClear:** supported
- **osEventFlagsGet:** supported
- **osEventFlagsWait:** cannot be called from an ISR.
- **osEventFlagsDelete:** supported

### Mutex Management

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__MutexMgmt.html) for details about Mutex Management functions.
Priority inherit protocol is used as default mutex behavior (`osMutexNew` creates priority inherit mutex object by default and ignores `osMutexPrioInherit` attribute when specified).
Robust mutex objects are not supported (`osMutexNew` returns NULL when `osMutexRobust` attribute is specified).

- **osMutexNew:** supported
- **osMutexGetName:** *not implemented*
- **osMutexAcquire:** supported
- **osMutexRelease:** supported
- **osMutexGetOwner:** supported
- **osMutexDelete:** supported

### Semaphores

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__SemaphoreMgmt.html) for details about Semaphore functions.

- **osSemaphoreNew:** supported
- **osSemaphoreGetName:** *not implemented*
- **osSemaphoreAcquire:** supported
- **osSemaphoreRelease:** supported
- **osSemaphoreGetCount:** supported
- **osSemaphoreDelete:** supported

### Memory Pool

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__PoolMgmt.html) for details about Memory Pool functions.

- **osMemoryPoolNew:** supported
- **osMemoryPoolGetName:** supported
- **osMemoryPoolAlloc:** supported
- **osMemoryPoolFree:** supported
- **osMemoryPoolGetCapacity:** supported
- **osMemoryPoolGetBlockSize:** supported
- **osMemoryPoolGetCount:** supported
- **osMemoryPoolGetSpace:** supported
- **osMemoryPoolDelete:** supported

### Message Queue

See [API reference](https://arm-software.github.io/CMSIS_6/latest/RTOS2/group__CMSIS__RTOS__Message.html) for details about Message Queue functions.

- **osMessageQueueNew:** supported
- **osMessageQueueGetName:** *not implemented*
- **osMessageQueuePut:** ignores message priority.
- **osMessageQueueGet:** ignores message priority.
- **osMessageQueueGetCapacity:** supported
- **osMessageQueueGetMsgSize:** supported
- **osMessageQueueGetCount:** supported
- **osMessageQueueGetSpace:** supported
- **osMessageQueueReset:** supported
- **osMessageQueueDelete:** supported
