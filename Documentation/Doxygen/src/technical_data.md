# Technical Data and Limitations {#page_technical_data}

This lists the technical data of CMSIS-FreeRTOS.

## Limitations

The following list briefly describes the limitations and unsupported features of the CMSIS-RTOS2 wrapper for FreeRTOS:

- Static memory allocation will only work if *all* memory (from attributes structure) is provided statically. In order to allocate object memory statically, you need to:
  - provide the memory for control blocks and stack in the `osThreadAttr_t` structure for threads.
  - provide the memory for control blocks and message data in the `osMessageQueueAttr_t` structure for memory queues.
  - provide the memory for control blocks for other objects in the object's attributes structure.
- Each timer object requires additional 8 bytes of memory:
  - to allocate all memory statically, provide the memory for control block of size (sizeof(StaticTimer_t) + 8 bytes)
  - otherwise, additional 8 bytes of dynamic memory will be used
- `osKernelSuspend` and `osKernelResume` are not supported.
- `osThreadDetach`, `osThreadJoin()` and attribute `osThreadJoinable` are not supported (`osThreadNew` returns NULL when `osThreadJoinable` attribute is specified).
- `osThreadGetStackSize` is not implemented.
- Event flags are limited to 24 bits.
- `osEventFlagsGetName` is not implemented.
- `osEventFlagsWait` cannot be called from an ISR.
- Priority inherit protocol is used as default mutex behavior (`osMutexNew` creates priority inherit mutex object by default and ignores `osMutexPrioInherit` attribute when specified).
- Robust mutex objects are not supported (`osMutexNew` returns NULL when `osMutexRobust` attribute is specified).
- `osMutexGetName` is not implemented and always returns NULL.
- `osSemaphoreGetName` is not implemented and always returns NULL.
- `osMessageQueueGetName` is not implemented and always returns NULL.
- `osMessageQueuePut` and `osMessageQueueGet` always ignore message priority.
- *Process Isolation (Functional Safety)* functions are not implemented.

## Validation Suite Results

CMSIS provides a [CMSIS-RTOS2 validation suite](https://arm-software.github.io/CMSIS_6/latest/RTOS2/rtosValidation.html) that can be used to test a real-time operating system for compliance to the standard. The test suite has been executed successfully on the CMSIS-FreeRTOS implementation ([see results](cmsis_rtos2_validation.txt)).

The following table explains the exceptions:

| Test Case                  | Result         | Reason               | Summary                                                                 |
|----------------------------|----------------|----------------------|-------------------------------------------------------------------------|
| TC_osKernelGetState_2      | not executed   | unsupported feature  | Test attempts to call `osKernelGetState` after a `osKernelSuspend` call. `osKernelSuspend` is not implemented. |
| TC_osKernelLock_2          | not executed   | unsupported feature  | Test attempts to call `osKernelLock` after a `osKernelSuspend` call. `osKernelSuspend` is not implemented. |
| TC_osKernelUnlock_2        | not executed   | unsupported feature  | Test attempts to call `osKernelUnlock` after a `osKernelSuspend` call. `osKernelSuspend` is not implemented. |
| TC_osKernelSuspend_1       | not executed   | unsupported feature  | Test validates `osKernelSuspend` which is not implemented. |
| TC_osKernelResume_1        | not executed   | unsupported feature  | Test validates `osKernelResume` which is not implemented. |
| TC_osThreadNew_3           | not executed   | unsupported feature  | Test attempts to create joinable thread using `osThreadJoinable` attribute. FreeRTOS does not support joinable threads. |
| TC_osThreadGetName_1       | failed         | deviation            | Test attempt to retrieve a name on an unnamed thread. An empty string is returned instead of NULL pointer. |
| TC_osThreadGetState_3      | not executed   | unsupported feature  | Test attempts to retrieve a state of a terminated joinable thread. FreeRTOS does not support joinable threads. |
| TC_osThreadDetach_1        | not executed   | unsupported feature  | Test validates `osThreadDetach` which is not implemented. |
| TC_osThreadDetach_2        | not executed   | unsupported feature  | Test validates `osThreadDetach` which is not implemented. |
| TC_osThreadJoin_1          | not executed   | unsupported feature  | Test validates `osThreadJoin` which is not implemented. |
| TC_osThreadJoin_2          | not executed   | unsupported feature  | Test validates `osThreadJoin` which is not implemented. |
| TC_osThreadJoin_3          | not executed   | unsupported feature  | Test validates `osThreadJoin` which is not implemented. |
| TC_osThreadGetStackSize_1  | not executed   | unsupported feature  | Test validates `osThreadGetStackSize` which is not implemented. |
| TC_ThreadReturn            | not executed   | unsupported feature  | Test attempts to terminate a thread by just returning from a thread. FreeRTOS threads may not return. |
| TC_osEventFlagsSet_1       | failed         | deviation            | Test attempts to set event flags by calling `osEventFlagsSet` multiple times without leaving ISR handler. To process ISR requests, FreeRTOS uses timer deamon which wakes-up after ISR execution. |
| TC_osEventFlagsClear_1     | failed         | deviation            | Test attempts to clear event flags by calling `osEventFlagsClear` multiple times without leaving ISR handler. To process ISR requests, FreeRTOS uses timer deamon which wakes-up after ISR execution. |
| TC_osEventFlagsWait_1      | failed         | unsupported feature  | Test attempts to wait for flags from ISR with zero timeout (try-semantic). FreeRTOS does not support such operation. |
| TC_osEventFlagsGetName_1   | not executed   | unsupported feature  | Test validates `osEventFlagsGetName` which is not implemented. |
| TC_osMutexNew_4            | not executed   | unsupported feature  | Test attempts to create a robust mutex. FreeRTOS implementation does not support robust mutexes. |
| TC_osMutexGetName_1        | not executed   | unsupported feature  | Test validates `osMutexGetName` which is not implemented. |
| TC_MutexRobust             | not executed   | unsupported feature  | Test attempts to validate robust mutex behavior. FreeRTOS implementation does not support robust mutexes. |
| TC_MutexOwnership          | not executed   | unsupported feature  | Test attempts to release a mutex from a thread which is not the mutex owner. FreeRTOS implementation does not verify ownership on mutex release. |
| TC_osSemaphoreGetName_1    | not executed   | unsupported feature  | Test validates `osSemaphoreGetName` which is not implemented. |
| TC_osMessageQueueGetName_1 | not executed   | unsupported feature  | Test validates `osMessageQueueGetName` which is not implemented. |

## Troubleshooting

Users looking for help shall check [subsection of the full FreeRTOS FAQ](https://www.freertos.org/Why-FreeRTOS/FAQs). It contains many useful information that also apply when using FreeRTOS in context of CMSIS-FreeRTOS.

Additionally, please take a look at the following:

1. **Interrupts are disabled when main is called or before the kernel is started**

    Before the FreeRTOS kernel is started, threads (i.e. tasks) may be created together with other objects like mutexes, semaphores, message queues etc. When functions like `xTaskCreate`, `xSemaphoreCreateMutex`, `xQueueCreate` and others get called, they prevent interrupt handlers from calling FreeRTOS API functions in order to keep FreeRTOS kernel variables and state machine consistent (see also here [Interrupts are not executing](https://www.freertos.org/Why-FreeRTOS/FAQs)). In cases when interrupts may be executed after object creation and before the FreeRTOS kernel is started they can be re-enabled:
    ```c
        portENABLE_INTERRUPTS();
    ```
    Make sure that the interrupts you execute in such case do not call FreeRTOS API.
