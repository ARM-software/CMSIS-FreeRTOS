# Hello World Example

This example prints "Hello World" and a counter value via the standard output.

## Prerequisites

### Tools:
 - [Microsoft Visual Studio Code](https://code.visualstudio.com/download) with Arm Keil Studio Pack extension (optional, alternatively CLI can be used)

When using Visual Studio Code and Arm Environment Manager all other tools will be installed automatically after environment activation.

Alternatively, manually install and configure the tools:

- [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox) command-line building tools
- Toolchains:
  - [Arm Compiler for Embedded](https://developer.arm.com/Tools%20and%20Software/Arm%20Compiler%20for%20Embedded) (commercial) or
  - [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) (community supported) or
  - [Arm LLVM Embedded Toolchain](https://learn.arm.com/install-guides/llvm-embedded/) (community supported) or
  - [IAR Compiler for Arm](https://www.iar.com/ewarm) (commercial)
- [Fixed Virtual Platforms (FVPs)](https://developer.arm.com/Tools%20and%20Software/Fixed%20Virtual%20Platforms) simulation models


## Build Solution/Project

### Using Visual Studio Code with Arm Keil Studio Pack extensions

Required tools described in file 'vcpkg-configuration.json' should be automatically installed by vcpkg. You can see the status of vcpkg in the status bar.

Required CMSIS packs need to be also installed. In case a required pack is missing, a notification window will pop-up to install the missing pack.

Open the 'CMSIS' view from the side bar, select desired 'Target Type', 'Build Type' and 'Project" you want to build and press the 'Build' button.

### Using Command Line Interface (CLI)

Download required packs (not required when the packs are already available) by executing the following commands:
   ```sh
   csolution list packs -s Examples.csolution.yml -m > packs.txt
   cpackget update-index
   cpackget add -f packs.txt
   ```
Build the project by executing the following command:
```sh
cbuild Examples.csolution.yml --context Hello.Debug+CM3 --update-rte --toolchain AC6
```

## Run the application

### Using Visual Studio Code with extensions

 - Connect the board's DAPLink USB to the PC (provides also power).
 - Open the 'CMSIS' view from the side bar:
   - Press the 'Run' button and wait until the image is programmed and starts running.
   - Press the 'Open Serial' button and select the board's serial port with 115200 baud rate.
 - Observe the terminal output.

 ### Using Drag-and-drop programming or external programmer and terminal

 - Connect the board's DAPLink USB to the PC (provides also power).
 - Program the image (.hex) using Drag-and-drop programming or use external programmer.
 - Open terminal on the PC and connect to the board's serial port with 115200 baud rate.
 - Observe the terminal output.

## Debug the application

### Using Visual Studio Code with extensions

Open the 'CMSIS' view from the side bar and press the 'Debug' button.
