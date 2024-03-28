This Arm Cortex-M33 secure/non-secure example project shows the setup
of the CMSIS-RTOS2 FreeRTOS application for Armv8-M TrustZone.

The application uses CMSIS and can be executed on Arm Virtual Hardware Target.
It demonstrates booting from the secure domain and passing execution to
non-secure domain running FreeRTOS kernel. Arm CMSE interface is used to
demonstrate function calls from non-secure to secure domain.

Use Batch Build to build the application or set TZ_Secure project as active,
build it first, then switch to TZ_NonSecure project, built it and start the
Debug Session (Ctrl+F5).

Secure application:
 - Setup code and start non-secure application.

Non-secure application:
 - Calls a secure function from non-secure state.
   Secure function calls back to a non-secure function.

Output:
Variables used in this application can be viewed in the Debugger Watch window.