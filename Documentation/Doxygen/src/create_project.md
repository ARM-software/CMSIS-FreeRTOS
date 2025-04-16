# Create a FreeRTOS Project {#page_create_project}

You can basically choose between two options when creating a FreeRTOS project:

1. [Using the FreeRTOS API and kernel](#create_native_project).
2. [Using the CMSIS-RTOS2 API with an underlying FreeRTOS kernel](#create_cmsis_project).

Before starting, please review the brief summary of the [Pack Components](#pack_components) below to familiarize yourself with them.

## Pack Components {#pack_components}

The CMSIS-FreeRTOS pack includes the following components and their variants:

| Component            | Variant                 | Description                                                                         |
|----------------------|-------------------------|-------------------------------------------------------------------------------------|
| CMSIS:RTOS2:FreeRTOS | Cortex-M                | CMSIS-RTOS2 API implementation based on the FreeRTOS kernel for Cortex-M.           |
|                      | Cortex-A                | Adds support for external IRQ controller on Cortex-A processors.                    |
| RTOS:CORE            | Cortex-M                | Core API for Cortex-M processors, providing task scheduling and communication.      |
|                      | Cortex-M MPU            | Adds support for the Memory Protection Unit (MPU) on Cortex-M processors.           |
|                      | Cortex-M Non-Secure     | Core API for Cortex-M, supporting TrustZone non-secure domain operation. **[Warning: Deprecated]** |
|                      | Cortex-M Non-Secure MPU | Combines TrustZone non-secure domain support with MPU functionality. **[Warning: Deprecated]** |
|                      | Cortex-A                | Core API for Cortex-A processors.                                                   |
| RTOS:Config          | CMSIS RTOS2             | Configuration file tailored for the CMSIS-RTOS2 API.                                |
|                      | FreeRTOS                | Configuration file for native FreeRTOS API usage.                                   |
| RTOS:Coroutines      | -                       | Adds FreeRTOS Co-routine API.                                                       |
| RTOS:Event Groups    | -                       | Provides event group functionality for task synchronization and communication.      |
| RTOS:Heap            | Heap_1                  | A simple memory allocation scheme that does not permit memory to be freed.          |
|                      | Heap_2                  | Allows memory to be freed but does not coalesce adjacent free memory blocks.        |
|                      | Heap_3                  | Wraps the standard `malloc()` and `free()` functions for thread safety.             |
|                      | Heap_4                  | Coalesces adjacent free memory blocks to avoid fragmentation.                       |
|                      | Heap_5                  | Extends Heap_4 with the ability to span the heap across non-adjacent memory areas.  |
| RTOS:Message Buffer  | -                       | Enables tasks and interrupts to send and receive variable-length messages.          |
| RTOS:Stream Buffer   | -                       | Allows tasks and interrupts to send and receive continuous streams of data.         |
| RTOS:Timers          | -                       | Adds support for software timers, enabling delayed and periodic function execution. |
| RTOS:TrustZone       | -                       | Provides context management for execution in TrustZone secure domain.               |

> **NOTE**
>
> CMSIS-FreeRTOS 11.2.0 adds support for TrustZone **domain detection from project** settings. Regardless of the build contexts
> (no TrustZone, TrustZone secure or non-secure domain) the below variants shall be used:
>
> - RTOS:CORE&Cortex-M
> - RTOS:CORE&Cortex-M MPU
>
> This makes **variants Cortex-M Non-Secure and Cortex-M Non-Secure MPU obsolete** and will be removed in future releases.

## Create a Native FreeRTOS Project {#create_native_project}

The steps to create a microcontroller application using FreeRTOS are:

- Create a new project and select a microcontroller device.
- In the Manage Run-Time Environment window, select **::Device:Startup**, **::RTOS:CORE** and
  **::RTOS:Config** in the **FreeRTOS** variant and an applicable **::RTOS:Heap** scheme
  (for more information on the heap schemes, visit the FreeRTOS documentation):

  ![Manage RTE FreeRTOS Native](manage_rte_freertos_native.png)

  \n
- If the **Validation Output** requires other components to be present, try to use the **Resolve** button.
- Click **OK**. In the **Project** window, you will see the files that have been automatically added
  to your project, such as **FreeRTOSConfig.h**, the source code files, as well as the system and
  startup files:

  ![Project Window FreeRTOS Native](project_window_freertos_native.png)

### Configure FreeRTOS {#configure_native_freertos}

When you have created the native FreeRTOS project, you can configure the real-time operating system
using the **FreeRTOSConfig.h** file. Please refer to the
[FreeRTOS documentation](https://www.freertos.org/a00110.html) for more information on the specific settings.

![FreeRTOS Config H Native](freertos_config_h_native.png)

### Interrupt Priority Configuration {#interrupt_priority_config}

FreeRTOS implements critical sections using the
[BASEPRI](https://developer.arm.com/documentation/dui0552/a/the-cortex-m3-processor/programmers-model/core-registers?lang=en)
register (available in Armv7-M and Armv8-M architecture-based devices) which masks only a subset of interrupts.
This is configured via the `configMAX_SYSCALL_INTERRUPT_PRIORITY` setting. Therefore, it is needed to properly
configure this setting. It is also needed to set appropriate interrupt priorities for interrupt service routines (ISR)
that use RTOS functions. This can especially impact drivers which typically use peripheral interrupts. Normally,
these use the RTOS directly or indirectly through registered callbacks.

Arm Cortex-M cores store interrupt priority values in the most significant bits of the interrupt priority registers
which can have a maximum of eight bits. Many implementations offer only three priority bits. These three bits are
shifted up to be bits five, six and seven respectively. `configMAX_SYSCALL_INTERRUPT_PRIORITY` must not be 0 and can
be xxx00000. This results in the following table:

| configMAX_SYSCALL_INTERRUPT_PRIORITY    | Upper three bits | Priority    |
|:---------------------------------------:|:----------------:|:-----------:|
| 32                                      | 001              | 1 (Highest) |
| 64                                      | 010              | 2           |
| 96                                      | 011              | 3           |
| 128                                     | 100              | 4           |
| 160                                     | 101              | 5           |
| 196                                     | 110              | 6           |
| 224                                     | 111              | 7 (Lowest)  |

#### Example

If you set `configMAX_SYSCALL_INTERRUPT_PRIORITY` to 32, then the priority of an interrupt service routine that uses
RTOS functions must then be higher or equal to 1. This ensures that this interrupt will be masked during a critical section.

A WiFi driver using the SPI interface registers a callback to SPI which is executed in an interrupt context.
The callback function in the WiFi driver uses RTOS functions. Therefore, the SPI interrupt priority must be
set to a value equal or higher to the FreeRTOS preempt priority, for example 1.

> **Note:**
>
> - For a detailed description of how FreeRTOS is using Cortex-M code registers, refer to
>   [Running the RTOS on an ARM Cortex-M Core](https://www.freertos.org/RTOS-Cortex-M3-M4.html).

### Add Event Recorder Visibility {#native_freertos_add_evr}

- To use the Event Recorder together with FreeRTOS, add the software component **::Compiler:Event Recorder** to your project.
- Open [FreeRTOSConfig.h](#configure_native_freertos) and
  - verify the header file **freertos_evr.h** is included
  - add Event Recorder configuration definitions (see [Event Recorder Configuration](#configure_evr))
- Call **EvrFreeRTOSSetup()** in your application code (ideally in `main()`).
- If you are using simulation mode, add an initialization file with the following content:
  ```plaintext
  MAP 0xE0001000, 0xE0001007 READ WRITE
  signal void DWT_CYCCNT (void) {
  while (1) {
    rwatch(0xE0001004);
    _WWORD(0xE0001004, states);
    }
  }
  DWT_CYCCNT()
  ```
- Build the application code and download it to the debug hardware or run it in simulation.

Once the target application generates event information, it can be viewed in the µVision debugger using the **Event Recorder**.

## Create a CMSIS-FreeRTOS Project {#create_cmsis_project}

The steps to create a microcontroller application using CMSIS-FreeRTOS are:

- Create a new project and select a microcontroller device.
- In the Manage Run-Time Environment window, select **::Device:Startup**, **::CMSIS::RTOS2 (API)::FreeRTOS**,
  **::RTOS:CORE** in the **FreeRTOS** variant, **::RTOS:Config** in the **CMSIS RTOS2** variant, **::RTOS:Timers**,
  **::RTOS:Event Groups**, and an applicable **::RTOS:Heap** scheme (for more information on the heap schemes,
  visit the FreeRTOS documentation):

  ![Manage RTE FreeRTOS RTOS2](manage_rte_freertos_rtos2.png)

  \n
- If the **Validation Output** requires other components to be present, try to use the **Resolve** button.
- Click **OK**. In the **Project** window, you will see the files that have been automatically added to your project,
  such as **FreeRTOSConfig.h**, the source code files, as well as the system and startup files:

  ![Project Window FreeRTOS RTOS2](project_window_freertos_rtos2.png)

### Configure CMSIS-FreeRTOS {#configure_cmsis_freertos}

When you have created the CMSIS-FreeRTOS project, you can configure the real-time operating system using the **FreeRTOSConfig.h** file. It can be opened using the Configuration Wizard view:

![FreeRTOS Config H CMSIS RTOS](freertos_config_h_cmsis_rtos.png)

The following main settings are available:

| Name                           | Configuration define                 | Description                                                           |
|--------------------------------|--------------------------------------|-----------------------------------------------------------------------|
| Minimal stack size [words]     | configMINIMAL_STACK_SIZE             | Stack for idle task and default task stack in words.                  |
| Total heap size [bytes]        | configTOTAL_HEAP_SIZE                | Heap memory size in bytes.                                            |
| Kernel tick frequency [Hz]     | configTICK_RATE_HZ                   | Kernel tick rate in Hz.                                               |
| Timer task stack depth [words] | configTIMER_TASK_STACK_DEPTH         | Stack for timer task in words.                                        |
| Timer task priority            | configTIMER_TASK_PRIORITY            | Timer task priority.                                                  |
| Timer queue length             | configTIMER_QUEUE_LENGTH             | Timer command queue length.                                           |
| Preemption interrupt priority  | configMAX_SYSCALL_INTERRUPT_PRIORITY | Maximum priority of interrupts that are safe to call FreeRTOS API.    |
| Use time slicing               | configUSE_TIME_SLICING               | Enable setting to use time slicing.                                   |
| Use tickless idle              | configUSE_TICKLESS_IDLE              | Enable low power tickless mode that stops tick interrupt when idle.   |
| Idle should yield              | configIDLE_SHOULD_YIELD              | Control Yield behavior of the idle task.                              |
| Check for stack overflow       | configCHECK_FOR_STACK_OVERFLOW       | Enable or disable stack overflow checking.                            |
| Use idle hook                  | configUSE_IDLE_HOOK                  | Enable callback function call on each idle task iteration.            |
| Use tick hook                  | configUSE_TICK_HOOK                  | Enable callback function call during each tick interrupt.             |
| Use daemon task startup hook   | configUSE_DAEMON_TASK_STARTUP_HOOK   | Enable callback function call when timer service starts.              |
| Use malloc failed hook         | configUSE_MALLOC_FAILED_HOOK         | Enable callback function call when out of dynamic memory.             |
| Queue registry size            | configQUEUE_REGISTRY_SIZE            | Define maximum number of queue objects registered for debug purposes. |

> **Note**
>
> - Refer to [Interrupt Priority Configuration](#interrupt_priority_config) for more information on the usage of `configMAX_SYSCALL_INTERRUPT_PRIORITY`.

#### Event Recorder Configuration

The following settings are available (see [Event Recorder Configuration](#configure_evr) for details):

| Name                           | Configuration define                 | Description                                                                             |
|--------------------------------|--------------------------------------|-----------------------------------------------------------------------------------------|
| Initialize Event Recorder      | configEVR_INITIALIZE                 | Initialize Event Recorder before FreeRTOS kernel start.                                 |
| Setup recording level filter   | configEVR_SETUP_LEVEL                | Enable configuration of FreeRTOS events recording level.                                |
| Task functions                 | configEVR_LEVEL_TASKS                | Define event recording level bitmask for events generated from Tasks functions.         |
| Queue functions                | configEVR_LEVEL_QUEUE                | Define event recording level bitmask for events generated from Queue functions.         |
| Timer functions                | configEVR_LEVEL_TIMERS               | Define event recording level bitmask for events generated from Timer functions.         |
| Event Groups functions         | configEVR_LEVEL_EVENTGROUPS          | Define event recording level bitmask for events generated from Event Groups functions.  |
| Heap functions                 | configEVR_LEVEL_HEAP                 | Define event recording level bitmask for events generated from Heap functions.          |
| Stream Buffer functions        | configEVR_LEVEL_STREAMBUFFER         | Define event recording level bitmask for events generated from Stream Buffer functions. |

#### Memory Allocation Configuration

The following settings are available:

| Name                                   | Configuration define                 | Description                                                                        |
|----------------------------------------|--------------------------------------|------------------------------------------------------------------------------------|
| Support static memory allocation       | configSUPPORT_STATIC_ALLOCATION      | Enable or disable support for static memory allocation.                            |
| Support dynamic memory allocation      | configSUPPORT_DYNAMIC_ALLOCATION     | Enable or disable support for dynamic memory allocation.                           |
| Use kernel provided static memory      | configKERNEL_PROVIDED_STATIC_MEMORY  | When enabled, FreeRTOS kernel provides static memory for Idle and Timer tasks.     |
| Use application allocated heap         | configAPPLICATION_ALLOCATED_HEAP     | When enabled, application must provide heap buffer externally.                     |
| Use separate heap for stack allocation | configSTACK_ALLOCATION_FROM_SEPARATE_HEAP | Enable or disable stack allocation for any task from a separate heap.         |
| Use heap protector                     | configENABLE_HEAP_PROTECTOR          | Enable or disable bounds checking and obfuscation to heap block pointers.          |

#### Port Specific Configuration

The following settings are available:

| Name                              | Configuration define           | Description                                                                        |
|-----------------------------------|--------------------------------|------------------------------------------------------------------------------------|
| Use Floating Point Unit           | configENABLE_FPU                | Enable or disable support for FPU when switching execution context.                |
| Use M-Profile Vector Extension    | configENABLE_MVE                | Enable or disable support for MVE when switching execution context.                |
| Use Memory Protection Unit        | configENABLE_MPU                | Enable or disable support for MPU on ARMv8-M MPU enabled ports.                    |
| Use TrustZone Secure Side Only    | configRUN_FREERTOS_SECURE_ONLY  | Enable this setting when FreeRTOS runs on the Secure side only.                    |
| Use TrustZone Security Extension  | configENABLE_TRUSTZONE          | Enable TrustZone when FreeRTOS runs on the Non-Secure side and calls functions from the Secure side.|
| Minimal secure stack size [words] | configMINIMAL_SECURE_STACK_SIZE | Stack for idle task Secure side context in words.                                  |

#### Interrupt Controller Configuration

**Note:** Settings related to interrupt controller are relevant only on ARMv7-A ports where it is necessary to configure Arm Generic Interrupt Controller (GIC).

The following settings are available:

| Name                                      | Configuration define                           | Description                                                   |
|-------------------------------------------|------------------------------------------------|---------------------------------------------------------------|
| Interrupt controller base address         | configINTERRUPT_CONTROLLER_BASE_ADDRESS         | Sets the base address of the interrupt controller peripheral. |
| Interrupt controller CPU interface offset | configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET | Sets the offset from interrupt controller base address at which the CPU interface starts.          |
| Interrupt controller unique priorities    | configUNIQUE_INTERRUPT_PRIORITIES               | Sets the number of unique priorities that can be specified in the interrupt controller peripheral. |

#### Symmetric Multiprocessing Configuration

**Note:** Symmetric Multiprocessing Configuration (SMP) settings are only relevant if FreeRTOS port implementation supports SMP.

The following settings are available:

| Name                        | Configuration define        | Description                                                      |
|-----------------------------|-----------------------------|------------------------------------------------------------------|
| Number of processor cores   | configNUMBER_OF_CORES       | Sets the number of available processor cores.                    |
| Use processor core affinity | configUSE_CORE_AFFINITY     | Enables the control for task to run on specific processor cores. |
| Use passive idle hook       | configUSE_PASSIVE_IDLE_HOOK | Enable callback function call on each idle task iteration.       |

### Add Event Recorder Visibility {#cmsis_freertos_add_evr}

- To use the Event Recorder together with FreeRTOS, add the software component **::Compiler:Event Recorder** to your project.
- Open [FreeRTOSConfig.h](#configure_cmsis_freertos) and
  - verify the header file **freertos_evr.h** is included
  - modify Event Recorder configuration definitions (see [Event Recorder Configuration](#configure_evr)) to change default configuration
- Call **osKernelInitialize()** in your application code (ideally in `main()`) to setup Event Recorder according to configuration settings.
- If you are using simulation mode, add an initialization file with the following content:
  ```plaintext
  MAP 0xE0001000, 0xE0001007 READ WRITE
  signal void DWT_CYCCNT (void) {
  while (1) {
    rwatch(0xE0001004);
    _WWORD(0xE0001004, states);
    }
  }
  DWT_CYCCNT()
  ```
- Build the application code and download it to the debug hardware or run it in simulation.

Once the target application generates event information, it can be viewed in the µVision debugger
using the **Event Recorder**.

## Create a Mixed-Interface Project {#create_mixed_project}

Using CMSIS-RTOS2 API and native FreeRTOS API simultaneously is possible and some projects do require
using the native FreeRTOS API and the CMSIS-RTOS2 API at the same time. Such project should be
[created as CMSIS-FreeRTOS project](#create_cmsis_project).

Depending on the application requirements, FreeRTOS kernel can be started either by using FreeRTOS
native API or by using CMSIS-RTOS2 API.

### Start the Kernel Using CMSIS-RTOS2 API

```c
/*
  Application thread: Initialize and start the Application
*/
void app_main (void *argument) {

  while(1) {
    // Application code
    // ...
  }
}

/*
  Main function: Initialize and start the kernel
*/
int main (void) {
  SystemCoreClockUpdate();

  // Initialize CMSIS-RTOS2
  osKernelInitialize();

  // Create application main thread
  osThreadNew(app_main, NULL, NULL);

  // Start the kernel and execute the first thread
  osKernelStart();

  while(1);
}
```

#### Restrictions

After the kernel is started using CMSIS-RTOS2 API, FreeRTOS native API can be used with the following restrictions:

- `vTaskStartScheduler` must not be called

### Start the Kernel Using Native API

```c
/*
  Application main thread: Initialize and start the application
*/
void app_main (void *argument) {

  while(1) {
    // Application code
    // ...
  }
}

/*
  Main function: Initialize and start the kernel
*/
int main (void) {
  SystemCoreClockUpdate();

  // Setup the Event Recorder (optionally)
  EvrFreeRTOSSetup(0);

  // Create application main thread
  xTaskCreate (app_main, "app_main", 64, NULL, tskIDLE_PRIORITY+1, NULL);

  // Start the kernel and execute the first thread
  vTaskStartScheduler();

  while(1);
}
```

#### Restrictions

After the kernel is started using FreeRTOS native API, CMSIS-RTOS2 API can be used without restrictions.

## Configure Event Recorder {#configure_evr}

This section describes the configuration settings for the [Event Recorder](https://arm-software.github.io/CMSIS-View/latest/evr.html). For more information refer to section [Add Event Recorder Visibility to native FreeRTOS project](#native_freertos_add_evr) or [Add Event Recorder Visibility to CMSIS-FreeRTOS project](#cmsis_freertos_add_evr).

Use below definitions to configure Event Recorder initialization and recording level filter setup.

```plaintext
#define configEVR_INITIALIZE
```

| Value  | Description                           |
|--------|---------------------------------------|
| 0      | Disable Event Recorder initialization |
| 1      | Enable Event Recorder initialization  |

Definition `configEVR_INITIALIZE` enables Event Recorder initialization during execution of function `EvrFreeRTOSSetup`. Default value is `1`.

```plaintext
#define configEVR_SETUP_LEVEL
```

| Value  | Description                           |
|--------|---------------------------------------|
| 0      | Disable recording level filter setup  |
| 1      | Enable recording level filter setup   |

Definition `configEVR_SETUP_LEVEL` enables setup of recording level filter for events generated by FreeRTOS. Recording level is configured during execution of function `EvrFreeRTOSSetup`. Default value is `1`.

```plaintext
#define configEVR_LEVEL_TASKS
#define configEVR_LEVEL_QUEUE
#define configEVR_LEVEL_TIMERS
#define configEVR_LEVEL_EVENTGROUPS
#define configEVR_LEVEL_HEAP
#define configEVR_LEVEL_STREAMBUFFER
```

| Value  | Description                                            |
|--------|--------------------------------------------------------|
| 0x00   | Disable event generation                               |
| 0x01   | Enable generation of error events                      |
| 0x05   | Enable generation of error and operational events      |
| 0x07   | Enable generation of error, API and operational events |
| 0x0F   | Enable generation of all events                        |

Definitions `configEVR_LEVEL_x` set the recording level bitmask for events generated by each function group. They are taken into account only when recording level filter setup is enabled. Default value is `0x05`.

## Debug a CMSIS-FreeRTOS Project

**Note:** The following only applies when used with [Arm Keil MDK](https://developer.arm.com/Tools%20and%20Software/Keil%20MDK). If you are using a different toolchain, please consult its user's manual.

Apart from the debug capabilities that [Event Recorder](#configure_evr) offers, Keil MDK also supports thread-aware breakpoints, just like for the standard CMSIS-RTOS.

#### Code Example

```plaintext
BS FuncN1, 1, "break = (CURR_TID == tid_phaseA) ? 1 : 0"
BS FuncN1, 1, "break = (CURR_TID == tid_phaseA || CURR_TID == tid_phaseD) ? 1 : 0"
BS \\Blinky\Blinky.c\FuncN1\179, 1, "break = (CURR_TID == tid_phaseA || CURR_TID == tid_phaseD) ? 1 : 0"
```

> **Note:**
>
> - For more information on conditional breakpoints in Keil MDK, consult the [user's manual](https://developer.arm.com/documentation/101407/latest/Debugging/Debug-Windows-and-Dialogs/Breakpoints-Window).
> - Enabling [Periodic Window Update](https://developer.arm.com/documentation/101407/latest/User-Interface/View-Menu)
>   is required to capture register values for active running threads while executing. When turned off, only the current
>   FreeRTOS thread can be unwound after execution has been stopped.

#### Caveats

- You cannot specify individual breakpoints on the same address. The following is not possible:
  ```plaintext
  BS ThCallee, 1, "break = (CURR_TID==tid_phaseA) ? 1 : 0"
  BS ThCallee, 1, "break = (CURR_TID==tid_phaseD) ? 1 : 0"
  ```
  Instead, use this:
  ```plaintext
  BS ThCallee, 1, "break= (CURR_TID==tid_phaseA || CURR_TID==tid_phaseD) ? 1 : 0"
  ```

- If you don't want to use [Periodic Window Update](https://developer.arm.com/documentation/101407/latest/User-Interface/View-Menu), obtain the thread and unwind information by adding a function that gets called from each thread of interest:
  ```c
  _attribute_((noinline)) int FuncN1 (int n1) {
    ...
  }
  ```
  Then, specify a thread-aware breakpoint using an "invalid" thread ID:
  ```plaintext
  BS FuncN1, 1, "break = (CURR_TID == tid_phaseA + 1) ? 1 : 0"
  ```
  `'tid_phaseA'` would be valid, `'tid_phaseA + 1'` is not but will still capture the most recent registers and store them to the actual thread context each time a thread-aware breakpoint is checked.

- Function inlining typically causes thread-aware breakpoints to fail. To avoid this, prepend the `'noinline'` attribute to the function that is used to stop when the current FreeRTOS thread id matches:
  ```c
  _attribute_((noinline)) int FuncN1 (int n1) {
    ...
  }
  ```
  This helps to make thread-aware breakpoints far less dependent on the compiler optimization level.

- Thread-aware breakpoints should be setup using a [debug script](https://developer.arm.com/documentation/101407/latest/Debugging/Debug-Scripting). Reason being that thread-aware breakpoints are of a 'hybrid' type, that is a combined address and condition expression that works best when run from a debug script.
