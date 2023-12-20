# Hello World Application
This example prints "Hello World" and a counter value via the standard output. It
can be used as a starting point when developing new applications.

### Functionality
The application initializes CMSIS-RTOS2, creates the main application thread and starts
the RTOS scheduler. The application thread increments a counter and outputs it together
with the counter value.

### Output
The "Hello World" string, along with the counter value, is output via printf and
retargeted to the debug console.
