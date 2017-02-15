# CMSIS-FreeRTOS
FreeRTOS adaptation for CMSIS-RTOS API Version 2.

The GitHub repository's development branch reflects the current state of development and is constantly updated.

## Directory Structure

| Directory                     | Content                           |                
| ----------------------------- | --------------------------------- |
| CMSIS                         | CMSIS-FreeRTOS related files      |
| CMSIS/RTOS2/FreeRTOS/Config   | CMSIS-FreeRTOS configuration file |
| CMSIS/RTOS2/FreeRTOS/Examples | CMSIS-FreeRTOS example projects   |
| CMSIS/RTOS2/FreeRTOS/Source   | CMSIS-FreeRTOS source code        |
| Config                        | FreeRTOS configuration file       |
| Demo                          | FreeRTOS demo projects            |
| DoxyGen                       | Source of the documentation       |
| License                       | FreeRTOS license text file        |
| Source                        | FreeRTOS source code              |
| Utilities                     | Utility programs                  |

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

CMSIS-FreeRTOS is licensed under the FreeRTOS license.
