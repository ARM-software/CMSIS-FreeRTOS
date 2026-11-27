# Overview

This directory contains the FreeRTOS port for the Texas Instruments F29H85x series of MCUs equipped with C2000 C29x core(s), developed using the c29clang compiler. Guidelines for using this port can be found below.

## Software & Tool Dependencies

- [FreeRTOS Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/tag/V11.2.0) **v11.2.0**
- [C29-CGT](https://www.ti.com/tool/download/C29-CGT/1.0.0.LTS) **v1.0.0LTS**
- [Code Composer Studio IDE](https://www.ti.com/tool/download/CCSTUDIO) **v20x**
- [F29H85X-SDK](https://www.ti.com/tool/download/F29H85X-SDK/1.01.00.00) **v1.01.00.00** (or above)

## Test Project

The standard test project for validating port functionality can be found in the [Partner-Supported Demos](https://github.com/FreeRTOS/FreeRTOS-Partner-Supported-Demos/tree/main/C2000_C29x_F29H85x_CCS/) repository. Additional example projects can be found in the F29H85X-SDK.

## Notes

- `configUSE_MINI_LIST_ITEM` must be set to 0 for C29x port. Refer to [configuration docs](https://www.freertos.org/a00110.html#configUSE_MINI_LIST_ITEM) for more information
- Currently, the c29clang compiler utilizes some FPU registers even when FPU operations are not done. Therefore FPU context are *always* saved & restored
- Default resources utilized:
    - **CPUTIMER2** & **INT_TIMER2** : Timer and PIPE interrupt used for kernel tick
    - **INT_SW1** : PIPE Interrupt used for yield (task switch). Note that this interrupt MUST ALWAYS be configured to the lowest possible priority

  These resources are configured in *`portdefines.h`*. It is recommended to use the default configurations. However, if these are updated during development, make sure to Clean & Rebuild your CCS project.

## Support

- Additional examples, tools and documentation are available in the F29H85X-SDK. Refer to the [SDK documentation](https://software-dl.ti.com/C2000/docs/f29h85x-sdk/latest/docs/html/index.html) for further details  
- For further support/queries, please raise a query on the Texas Instruments [E2E Support Forum](https://e2e.ti.com/)  
