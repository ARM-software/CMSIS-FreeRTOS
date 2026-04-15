# Overview

This CMSIS-Pack provides a [CMSIS-RTOS2 API](https://arm-software.github.io/CMSIS_6/latest/RTOS2/index.html)
implementation based on [FreeRTOS](https://www.freertos.org/).

## CMSIS-RTOS2 Implementation using FreeRTOS

The [CMSIS-RTOS v2 API](https://arm-software.github.io/CMSIS_6/latest/RTOS2/index.html) is implemented
as a wrapper layer on top of the FreeRTOS kernel. It maps the standardized CMSIS-RTOS2 functions to native
FreeRTOS kernel calls, enabling portable application code that can be used across different RTOS kernels.

Key characteristics of this implementation:

- Supports threads, mutexes, semaphores, event flags, message queues, timers, and memory pools.
- Supports both dynamic and static memory allocation for RTOS objects.
- Supports [Event Recorder](https://arm-software.github.io/CMSIS-View/latest/evr.html) for FreeRTOS event tracing and debugging.
- Validated against the [CMSIS-RTOS2 validation suite](https://arm-software.github.io/CMSIS_6/latest/RTOS2/rtosValidation.html).

> See [Technical Data and Limitations](https://arm-software.github.io/CMSIS-FreeRTOS/latest/index.html)
> for a full list of supported features and known deviations from the CMSIS-RTOS2 standard.

## Examples

The pack includes [CMSIS Solution](https://open-cmsis-pack.github.io/cmsis-toolbox/build-overview/) based examples that can be
built for a wide range of Cortex-M targets using [Arm Fixed Virtual Platforms (FVP)](https://developer.arm.com/Tools%20and%20Software/Fixed%20Virtual%20Platforms) simulation models — no hardware required.

| Example       | Description |
|:--------------|:------------|
| **Hello**     | Simple Hello World application using CMSIS-RTOS2. Use as a starting point for new applications. |
| **TrustZone** | TrustZone application for Armv8-M. Demonstrates booting from the secure side and using non-secure callable function from the non-secure side. |

Examples support Cortex-M0 through Cortex-M85 targets. They can be built using
[Arm Keil Studio extensions for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack)
or via the [CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/) command-line tools.

## Reference Applications

The pack includes a hardware-agnostic [CMSIS Solution](https://open-cmsis-pack.github.io/cmsis-toolbox/build-overview/) based
reference application that can be deployed to any development board that provides the required
[board layer](https://open-cmsis-pack.github.io/cmsis-toolbox/build-overview/#software-layers).

| Reference Application | Description |
|:----------------------|:------------|
| **Blinky**            | Simple CMSIS-RTOS2 application that blinks an LED and responds to a button. Use as a starting point to verify tool setup on real hardware. |

Reference applications can be deployed to a development board using
[Arm Keil Studio extensions for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack).
See [CMSIS-Toolbox - Reference Applications](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/)
to learn more about the reference application concept.

## CMSIS-Pack contents

```
  📦
  ┣ 📂 CMSIS/Config             CMSIS-FreeRTOS configuration templates
  ┣ 📂 CMSIS/Examples           Pre-configured examples using Arm FVP simulation models
  ┣ 📂 CMSIS/RefApp             Reference application using FreeRTOS
  ┣ 📂 CMSIS/Source             CMSIS-RTOS2 implementation using FreeRTOS
  ┣ 📂 Documentation            Pre-built documentation
  ┣ 📂 Overview                 Contains this README
  ┣ 📂 Source                   FreeRTOS kernel source code
  ┣ 📄 ARM.CMSIS-FreeRTOS.pdsc  Pack description file
  ┣ 📄 ARM.CMSIS-FreeRTOS.sha1  Checksums of all the files
  ┗ 📄 LICENSE                  License text
```

## Links

- [Documentation](https://arm-software.github.io/CMSIS-FreeRTOS/latest/index.html)
- [Issues](https://github.com/ARM-software/CMSIS-FreeRTOS/issues)
