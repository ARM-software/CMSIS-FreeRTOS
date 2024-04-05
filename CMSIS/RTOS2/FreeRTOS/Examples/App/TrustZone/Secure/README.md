# TrustZone Secure Application
The TrustZone Secure example project shows the setup of the CMSIS-RTOS2 FreeRTOS
application for Armv8-M TrustZone. It demonstrates how the Secure application boots
from the secure side and passes the execution to the application running on the
non-secure side. It also shows how to define secure side functions that can be called
from the non-secure side application.

### Functionality
The application sets the stack for the non-secure side and then calls the Reset Handler on
the non-secure side. It also provides the function that is declared as a non-secure
callable function (i.e. it can be called from the non-secure side application).
