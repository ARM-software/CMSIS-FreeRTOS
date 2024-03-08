/*
 * FreeRTOS Kernel V10.5.1+
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given Z80 (Z180, Z80N) hardware and SCCZ80 or SDCC compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    long
#define portSHORT                   int

typedef uint16_t                    StackType_t;
typedef int8_t                      BaseType_t;
typedef uint8_t                     UBaseType_t;

#if configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS
    typedef uint16_t                TickType_t;
    #define portMAX_DELAY           ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS  == TICK_TYPE_WIDTH_32_BITS )
    typedef uint32_t                TickType_t;
    #define portMAX_DELAY           ( TickType_t ) 0xffffffffUL
#else
    #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif

/*-----------------------------------------------------------*/

/* General purpose stringify macros. */

#define string(a) __string(a)
#define __string(a) #a

/*-----------------------------------------------------------*/

/* Architecture specifics. */

#define portSTACK_GROWTH            ( -1 )
#define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT          1

/*-----------------------------------------------------------*/

/* Critical section management. */

#define portENTER_CRITICAL()        \
    do{                             \
        __asm__(                    \
            "ld a,i             \n" \
            "di                 \n" \
            "push af            \n" \
            );                      \
    }while(0)

#define portEXIT_CRITICAL()         \
    do{                             \
        __asm__(                    \
            "pop af             \n" \
            "; di    ; unneeded \n" \
            "jp PO,ASMPC+4      \n" \
            "ei                 \n" \
            );                      \
    }while(0)

#define portDISABLE_INTERRUPTS()    \
    do{                             \
        __asm__(                    \
            "di                 \n" \
            );                      \
    }while(0)

#define portENABLE_INTERRUPTS()     \
    do{                             \
        __asm__(                    \
            "ei                 \n" \
            );                      \
    }while(0)

#define portNOP()                   \
    do{                             \
        __asm__(                    \
            "nop                \n" \
            );                      \
    }while(0)

/*
 * Macros to save all the registers, and save the stack pointer into the TCB.
 *
 * The first thing we do is save the flags then disable interrupts. This is to
 * guard our stack against having a context switch interrupt after we have already
 * pushed the registers onto the stack.
 *
 * The interrupts will have been disabled during the call to portSAVE_CONTEXT()
 * so we need not worry about reading/writing to the stack pointer.
 */

#define portSAVE_CONTEXT()          \
    do{                             \
        __asm__(                    \
            "push af            \n" \
            "ld a,i             \n" \
            "di                 \n" \
            "push af ; iff1:iff2\n" \
            "push bc            \n" \
            "push de            \n" \
            "push hl            \n" \
            "exx                \n" \
            "ex af,af           \n" \
            "push af            \n" \
            "push bc            \n" \
            "push de            \n" \
            "push hl            \n" \
            "push ix            \n" \
            "push iy            \n" \
            "ld hl,0            \n" \
            "add hl,sp          \n" \
            "ld de,(_pxCurrentTCB)  \n"\
            "ex de,hl           \n" \
            "ld (hl),e          \n" \
            "inc hl             \n" \
            "ld (hl),d          \n" \
            );                      \
    }while(0)

#define portRESTORE_CONTEXT()       \
    do{                             \
        __asm__(                    \
            "ld hl,(_pxCurrentTCB)  \n" \
            "ld e,(hl)          \n" \
            "inc hl             \n" \
            "ld d,(hl)          \n" \
            "ex de,hl           \n" \
            "ld sp,hl           \n" \
            "pop iy             \n" \
            "pop ix             \n" \
            "pop hl             \n" \
            "pop de             \n" \
            "pop bc             \n" \
            "pop af             \n" \
            "ex af,af           \n" \
            "exx                \n" \
            "pop hl             \n" \
            "pop de             \n" \
            "pop bc             \n" \
            "pop af  ; iff1:iff2\n" \
            "; di    ; unneeded \n" \
            "jp PO,ASMPC+4      \n" \
            "ei                 \n" \
            "pop af             \n" \
            "ret                \n" \
            );                      \
    }while(0)

#define portSAVE_CONTEXT_IN_ISR()   \
    do{                             \
        __asm__(                    \
            "PHASE "string(configISR_ORG)"  \n" \
            "._timer_isr_start  \n" \
            "push af            \n" \
            "ld a,0x7F          \n" \
            "inc a   ; set PE   \n" \
            "push af ; iff1:iff2\n" \
            "push bc            \n" \
            "push de            \n" \
            "push hl            \n" \
            "exx                \n" \
            "ex af,af           \n" \
            "push af            \n" \
            "push bc            \n" \
            "push de            \n" \
            "push hl            \n" \
            "push ix            \n" \
            "push iy            \n" \
            "ld hl,0            \n" \
            "add hl,sp          \n" \
            "ld de,(_pxCurrentTCB)  \n" \
            "ex de,hl           \n" \
            "ld (hl),e          \n" \
            "inc hl             \n" \
            "ld (hl),d          \n" \
            );                      \
    }while(0)

#define portRESTORE_CONTEXT_IN_ISR()\
    do{                             \
        __asm__(                    \
            "ld hl,(_pxCurrentTCB)  \n" \
            "ld e,(hl)          \n" \
            "inc hl             \n" \
            "ld d,(hl)          \n" \
            "ex de,hl           \n" \
            "ld sp,hl           \n" \
            "pop iy             \n" \
            "pop ix             \n" \
            "pop hl             \n" \
            "pop de             \n" \
            "pop bc             \n" \
            "pop af             \n" \
            "ex af,af           \n" \
            "exx                \n" \
            "pop hl             \n" \
            "pop de             \n" \
            "pop bc             \n" \
            "pop af  ; iff1:iff2\n" \
            "; di    ; unneeded \n" \
            "jp PO,ASMPC+4      \n" \
            "ei                 \n" \
            "pop af             \n" \
            "reti               \n" \
            "._timer_isr_end    \n" \
            "DEPHASE            \n" \
            );                      \
    }while(0)

/*-----------------------------------------------------------*/

/* Kernel utilities. */
extern void vPortYield( void );
#define portYIELD()                 vPortYield()

extern void vPortYieldFromISR( void );
#define portYIELD_FROM_ISR()        vPortYieldFromISR()
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
