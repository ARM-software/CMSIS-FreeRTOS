# CMSIS-FreeRTOS
The branch *master* of this GitHub repository contains the CMSIS-RTOS adoption of [FreeRTOS Version 10.2.1](https://sourceforge.net/projects/freertos/files/FreeRTOS/V10.2.1/FreeRTOSv10.2.1.zip/download) as it is released as Software Pack on http://www.keil.com/pack. The [documentation](https://arm-software.github.io/CMSIS-FreeRTOS/General/html/index.html) is available under https://arm-software.github.io/CMSIS-FreeRTOS/General/html/index.html.

Use *Issues* to provide feedback and report problems for CMSIS FreeRTOS implementation.

Note: The *branch* develop of this GitHub repository reflects our current state of development and is constantly updated. It gives our users and partners contiguous access to the development. It allows you to review the work and provide feedback or create pull requests for contributions.

## Directory Structure

| Directory                     | Content                            |                
| ----------------------------- | -----------------------------------|
| CMSIS                         | CMSIS-FreeRTOS related files       |
| CMSIS/RTOS2/FreeRTOS/Config   | CMSIS-FreeRTOS configuration file  |
| CMSIS/RTOS2/FreeRTOS/Examples | CMSIS-FreeRTOS example projects    |
| CMSIS/RTOS2/FreeRTOS/Source   | CMSIS-FreeRTOS source code         |
| Config                        | FreeRTOS Kernel configuration file |
| Demo                          | FreeRTOS demo projects             |
| DoxyGen                       | Source of the documentation        |
| License                       | FreeRTOS Kernel license text file  |
| Source                        | FreeRTOS Kernel source code        |
| Utilities                     | Utility programs                   |

## Generate CMSIS Pack for Release

This GitHub development repository contains all the sources you need to successfully build the pack.

To build the complete pack for installation, the following additional tools are required:
 - **doxygen.exe**    Version: 1.8.6 (Documentation Generator)
 - **mscgen.exe**     Version: 0.20  (Message Sequence Chart Converter)
 - **7z.exe (7-Zip)** Version: 16.04 (File Archiver)
  
Using these tools, you can generate on a Windows PC:
 - **CMSIS-FreeRTOS software pack** using the batch file **gen_pack.bat** (located in ./Utilities). This batch file also
   generates the documentation.
  
 - **CMSIS-FreeRTOS documentation** using the batch file **genDoc.bat** (located in ./Doxygen). 

The file ./DoxyGen/How2Doc.txt describes the rules for creating API documentation.

## License

The FreeRTOS kernel is released under the MIT open source license (see License/license.txt).
