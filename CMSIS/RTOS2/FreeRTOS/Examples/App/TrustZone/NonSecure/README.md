# TrustZone NonSecure Application
The TrustZone Non-Secure example project shows the setup of the CMSIS-RTOS2 FreeRTOS
application for Armv8-M TrustZone. It demonstrates how the Non-Secure application uses
the Arm CMSE interface to communicate with the Secure application via function calls.

> **Note:**
> - The TrustZone Secure example project must be used together with Non-Secure project
>   to successfully execute the application on target.

### Functionality
The application executes CMSIS-RTOS2 thread which calls a secure side function from
the non-secure side. The secure function then calls back to a non-secure side. The
return value of the secure function is the value of a counter variable stored on
the secure side.

### Output
Counter values used in this application are output via printf and retargeted to debug
console.
