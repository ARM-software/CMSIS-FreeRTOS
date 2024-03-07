//-------------------------------------------------------------------------------------------------
// Author: Ivan Zaitsev, ivan.zaitsev@gmail.com
//
// This file follows the FreeRTOS distribution license.
//
// FreeRTOS is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License (version 2) as published by the
// Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.
//
// ***************************************************************************
// >>!   NOTE: The modification to the GPL is included to allow you to     !<<
// >>!   distribute a combined work that includes FreeRTOS without being   !<<
// >>!   obliged to provide the source code for proprietary components     !<<
// >>!   outside of the FreeRTOS kernel.                                   !<<
// ***************************************************************************
//
// FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  Full license text is available on the following
// link: http://www.freertos.org/a00114.html
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Scheduler includes.
//-------------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"

//-------------------------------------------------------------------------------------------------
// Implementation of functions defined in portable.h for the C28x port.
//-------------------------------------------------------------------------------------------------

// Constants required for hardware setup.
#define portINITIAL_CRITICAL_NESTING  ( ( uint16_t ) 10 )
#define portFLAGS_INT_ENABLED         ( ( StackType_t ) 0x08 )
#if defined(__TMS320C28XX_FPU64__)
# define AUX_REGISTERS_TO_SAVE        27 // XAR + FPU registers
# define XAR4_REGISTER_POSITION       6  // XAR4 position in AUX registers array
# define STF_REGISTER_POSITION        10 // STF position in AUX registers array
#elif defined(__TMS320C28XX_FPU32__)
# define AUX_REGISTERS_TO_SAVE        19 // XAR + FPU registers
# define XAR4_REGISTER_POSITION       6  // XAR4 position in AUX registers array
# define STF_REGISTER_POSITION        10 // STF position in AUX registers array
#else
# define AUX_REGISTERS_TO_SAVE        9  // XAR registers only
# define XAR4_REGISTER_POSITION       5  // XAR4 position in AUX registers array
#endif

extern uint32_t getSTF( void );
extern void vApplicationSetupTimerInterrupt( void );

// Each task maintains a count of the critical section nesting depth.  Each
// time a critical section is entered the count is incremented.  Each time a
// critical section is exited the count is decremented - with interrupts only
// being re-enabled if the count is zero.
//
// ulCriticalNesting will get set to zero when the scheduler starts, but must
// not be initialised to zero as this will cause problems during the startup
// sequence.
// ulCriticalNesting should be 32 bit value to keep stack alignment unchanged.
volatile uint32_t ulCriticalNesting = portINITIAL_CRITICAL_NESTING;
volatile uint16_t bYield = 0;
volatile uint16_t bPreemptive = 0;

//-------------------------------------------------------------------------------------------------
// Initialise the stack of a task to look exactly as if
// timer interrupt was executed.
//-------------------------------------------------------------------------------------------------
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
  uint16_t i;
  uint16_t base = 0;

  pxTopOfStack[base++]  = 0x0080;  // ST0. PSM = 0(No shift)
  pxTopOfStack[base++]  = 0x0000;  // T
  pxTopOfStack[base++]  = 0x0000;  // AL
  pxTopOfStack[base++]  = 0x0000;  // AH
  pxTopOfStack[base++]  = 0xFFFF;  // PL
  pxTopOfStack[base++]  = 0xFFFF;  // PH
  pxTopOfStack[base++]  = 0xFFFF;  // AR0
  pxTopOfStack[base++]  = 0xFFFF;  // AR1
  pxTopOfStack[base++]  = 0x8A08;  // ST1
  pxTopOfStack[base++]  = 0x0000;  // DP
  pxTopOfStack[base++] = 0x0000;  // IER
  pxTopOfStack[base++] = 0x0000;  // DBGSTAT
  pxTopOfStack[base++] = ((uint32_t)pxCode) & 0xFFFFU;       // PCL
  pxTopOfStack[base++] = ((uint32_t)pxCode >> 16) & 0x00FFU; // PCH
  pxTopOfStack[base++] = 0xAAAA;  // Alignment
  pxTopOfStack[base++] = 0xBBBB;  // Alignment

  // Fill the rest of the registers with dummy values.
  for(i = 0; i < (2 * AUX_REGISTERS_TO_SAVE); i++)
  {
    uint16_t low  = 0x0000;
    uint16_t high = 0x0000;

    if(i == (2 * XAR4_REGISTER_POSITION))
    {
      low  = ((uint32_t)pvParameters) & 0xFFFFU;
      high = ((uint32_t)pvParameters >> 16) & 0xFFFFU;
    }

#if defined(__TMS320C28XX_FPU32__)
    if(i == (2 * STF_REGISTER_POSITION))
    {
      uint32_t stf = getSTF();

      low  = stf & 0xFFFFU;
      high = (stf >> 16) & 0xFFFFU;
    }
#endif

    pxTopOfStack[base + i] = low;
    i++;
    pxTopOfStack[base + i] = high;
  }

  base += i;

  // Reserve place for ST1 which will be used in context switch
  // to set correct SPA bit ASAP.
  pxTopOfStack[base++] = 0x8A18;  // ST1 with SPA bit set
  pxTopOfStack[base++] = 0x0000;  // DP
  pxTopOfStack[base++] = 0x0000;  // placeholder for 32 bit ulCriticalNesting
  pxTopOfStack[base++] = 0x0000;

  // Return a pointer to the top of the stack we have generated so this can
  // be stored in the task control block for the task.
  return pxTopOfStack + base;
}

//-------------------------------------------------------------------------------------------------
void vPortEndScheduler( void )
{
  // It is unlikely that the TMS320 port will get stopped.
  // If required simply disable the tick interrupt here.
}

//-------------------------------------------------------------------------------------------------
// See header file for description.
//-------------------------------------------------------------------------------------------------
BaseType_t xPortStartScheduler(void)
{
  vApplicationSetupTimerInterrupt();

  ulCriticalNesting = 0;

#if(configUSE_PREEMPTION == 1)
  bPreemptive = 1;
#else
  bPreemptive = 0;
#endif

  portENABLE_INTERRUPTS();
  portRESTORE_FIRST_CONTEXT();

  // Should not get here!
  return pdFAIL;
}

//-------------------------------------------------------------------------------------------------
void vPortEnterCritical( void )
{
  portDISABLE_INTERRUPTS();
  ulCriticalNesting++;
}

//-------------------------------------------------------------------------------------------------
void vPortExitCritical( void )
{
  ulCriticalNesting--;
  if( ulCriticalNesting == 0 )
  {
    portENABLE_INTERRUPTS();
  }
}
