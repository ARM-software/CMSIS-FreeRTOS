[![License Apache--2.0](https://img.shields.io/badge/License-Apache--2.0-green?label=License)](https://github.com/Arm-Software/CMSIS-FreeRTOS/blob/main/LICENSE)
[![License MIT](https://img.shields.io/badge/License-MIT-yellow?label=License)](https://github.com/Arm-Software/CMSIS-FreeRTOS/blob/main/LICENSE)
[![Build Examples](https://img.shields.io/github/actions/workflow/status/Arm-Software/CMSIS-FreeRTOS/examples.yaml?logo=arm&logoColor=0091bd&label=Build%20Examples)](https://github.com/Arm-Software/CMSIS-FreeRTOS/tree/main/.github/workflows/examples.yaml)
[![RTOS2 Validation](https://img.shields.io/github/actions/workflow/status/Arm-Software/CMSIS-FreeRTOS/examples.yaml?logo=arm&logoColor=0091bd&label=RTOS%20Validation)](https://github.com/Arm-Software/CMSIS-FreeRTOS/tree/main/.github/workflows/cmsis_rv2.yaml)
[![Build pack](https://img.shields.io/github/actions/workflow/status/Arm-Software/CMSIS-FreeRTOS/build.yaml?logo=arm&logoColor=0091bd&label=Build%20pack)](./.github/workflows/build.yaml)

# CMSIS-FreeRTOS

This repository contains the CMSIS-RTOS adoption of [FreeRTOS-Kernel Version 11.2.0](https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/download/V11.2.0/FreeRTOS-KernelV11.2.0.zip) as it is released as Software Pack on http://www.keil.com/pack. The [documentation](https://arm-software.github.io/CMSIS-FreeRTOS/) is available under https://arm-software.github.io/CMSIS-FreeRTOS/.

Use *Issues* to provide feedback and report problems for CMSIS FreeRTOS implementation.

Use *main* branch for pull-requests.

## Directory Structure

| Directory                     | Content                             |                
| ----------------------------- | ------------------------------------|
| CMSIS                         | CMSIS-FreeRTOS related files        |
| CMSIS/RTOS2/FreeRTOS/Config   | CMSIS-FreeRTOS configuration file   |
| CMSIS/RTOS2/FreeRTOS/Examples | CMSIS-FreeRTOS example projects     |
| CMSIS/RTOS2/FreeRTOS/Source   | CMSIS-FreeRTOS source code          |
| Documentation                 | Source of the documentation         |
| Source                        | FreeRTOS Kernel source code         |

## Generate CMSIS Pack for Release

This GitHub development repository contains all the sources you need to successfully build the pack.

To build the complete pack for installation use the **gen_pack.sh** bash script. This script file also
generates the documentation.

Documentation may be generated separately using the bash script **gen_doc.sh** (located in ./Documentation/Doxygen).

## License

The FreeRTOS kernel source files are released under the MIT open source license (read [LICENSE.md](./Source/LICENSE.md)).
The rest of the repository content is covered by Apache 2.0 license (read [LICENSE](./LICENSE)).
