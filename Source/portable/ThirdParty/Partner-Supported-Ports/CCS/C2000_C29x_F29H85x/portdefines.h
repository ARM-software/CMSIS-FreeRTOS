#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

//-------------------------------------------------------------------------------------------------
// FreeRTOS Port Configuration Macros
//-------------------------------------------------------------------------------------------------

//
// CPU Timer instance and interrupt in PIPE used for the RTOS tick
// Ensure that the INT corresponds to the configured CPUTIMER
//
#define PORT_TICK_TIMER_BASE        CPUTIMER2_BASE
#define PORT_TICK_TIMER_INT         INT_TIMER2
#define PORT_TICK_TIMER_INT_PRI     255U

//
// SW interrupt in PIPE to use as the interrupt for portYIELD()
// It is recommended that this be the last interrupt line in the PIPE
// SW should not use this interrupt line for anything else
//
#define PORT_TASK_SWITCH_INT        INT_SW1

//
// The SW interrupt used for portYIELD() should be the lowest priority
// SW should not use this interrupt priority for any other interrupt
//
#define PORT_TASK_SWITCH_INT_PRI    255U

//-------------------------------------------------------------------------------------------------
// Derived Macros. DO NOT EDIT!!
//-------------------------------------------------------------------------------------------------
#define PORT_TICK_TIMER_O_TCR       ( PORT_TICK_TIMER_BASE + 0x8U )