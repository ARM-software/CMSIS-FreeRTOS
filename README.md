# CMSIS-FreeRTOS

This repository contains the CMSIS-RTOS adoption of [FreeRTOS-Kernel Version 10.5.1](https://github.com/FreeRTOS/FreeRTOS-Kernel/releases/download/V10.5.1/FreeRTOS-KernelV10.5.1.zip) as it is released as Software Pack on http://www.keil.com/pack. The [documentation](https://arm-software.github.io/CMSIS-FreeRTOS/) is available under https://arm-software.github.io/CMSIS-FreeRTOS/.

Use *Issues* to provide feedback and report problems for CMSIS FreeRTOS implementation.

Use *main* branch for pull-requests.

Note: Branches *develop* and *master* will be deprecated. Please update your fork and switch to branch *main* to track the latest state of development.

## Directory Structure

| Directory                     | Content                             |                
| ----------------------------- | ------------------------------------|
| CMSIS                         | CMSIS-FreeRTOS related files        |
| CMSIS/RTOS2/FreeRTOS/Config   | CMSIS-FreeRTOS configuration file   |
| CMSIS/RTOS2/FreeRTOS/Examples | CMSIS-FreeRTOS example projects     |
| CMSIS/RTOS2/FreeRTOS/Source   | CMSIS-FreeRTOS source code          |
| Config                        | FreeRTOS Kernel configuration file  |
| Documentation                 | Generated documentation placeholder |
| DoxyGen                       | Source of the documentation         |
| License                       | FreeRTOS Kernel license text file   |
| Source                        | FreeRTOS Kernel source code         |

## Generate CMSIS Pack for Release

This GitHub development repository contains all the sources you need to successfully build the pack.

To build the complete pack for installation use the **gen_pack.sh** bash script. This script file also
generates the documentation.

Documentation may be generated separately using the bash script **gen_doc.sh** (located in ./DoxyGen).

**_NOTE_**
> Repository tags have to be updated to successfully generate the pack. To update them, use: </br>
> ```$ git fetch -f --prune --prune-tags origin```</br>
> When working with an existing fork replace 'origin' with the parent repository URL</br>
> ```$ git fetch -f --prune --prune-tags https://github.com/ARM-software/CMSIS-FreeRTOS.git```</br>


## License

The FreeRTOS kernel source files are released under the MIT open source license (read [LICENSE.md](./Source/LICENSE.md)).
The rest of the repository content is covered by Apache 2.0 license (read [LICENSE](./LICENSE)).
