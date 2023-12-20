# TrustZone: secure/non-secure thread context management

TrustZone example shows application setup for secure/non-secure thread context management.
It is divided into two parts:
  - Secure application
  - Non-secure application

## Secure Application

Secure application does initial setup and starts the non-secure application. It contains functions
that can be called from the non-secure application. The interface for these (non-secure callable)
functions is provided as "CMSE lib" object file, which is integrated into the non-secure application.

## Non-secure Application

Non-secure application initializes RTOS and executes thread that calls secure function. In addition
it also contains a callback function which gets called from the secure application.

