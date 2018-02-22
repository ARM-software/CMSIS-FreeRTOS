/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef LED_IO_H
#define LED_IO_H

/* Include the register definition file that is correct for the hardware being
used.  The C and assembler pre-processor must have one of the following board
definitions defined to have the correct register definition header file
included.  Alternatively, just manually include the correct files here. */

	/* Prevent the files being included from the FreeRTOS port layer assembly
	source files. */
	#ifndef __ASSEMBLER__

		#ifdef YRDKRL78G14
			#include "iodefine_RL78G14.h"
			#include "iodefine_RL78G14_ext.h"
			#define LED_BIT			( P4_bit.no1 )
			#define LED_INIT() 		LED_BIT = 0
		#endif /* YRDKRL78G14 */

		#ifdef RSKRL78G1C
			#include "iodefine_RL78G1C.h"
			#include "iodefine_RL78G1C_ext.h"
			#define LED_BIT			( P0_bit.no1 )
			#define LED_INIT()		P0 &= 0xFD; PM0 &= 0xFD
		#endif /* RSKRL78G1C */

		#ifdef RSKRL78L1C
			#include "iodefine_RL78L1C.h"
			#include "iodefine_RL78L1C_ext.h"
			#define LED_BIT			( P4_bit.no1 )
			#define LED_INIT()		P4 &= 0xFD; PM4 &= 0xFD
		#endif /* RSKRL78L1C */

		#ifdef RSKRL78L13
			#include "iodefine_RL78L13.h"
			#include "iodefine_RL78L13_ext.h"
			#define LED_BIT			( P4_bit.no1 )
			#define LED_INIT() 		P4 &= 0xFD; PM4 &= 0xFD
		#endif /* RSKRL78L13 */

		#ifdef RL78_G1A_TB
			#include "iodefine_RL78G1A.h"
			#include "iodefine_RL78G1A_ext.h"
			#define LED_BIT			( P6_bit.no2 )
			#define LED_INIT() 		P6 &= 0xFB; PM6 &= 0xFB
		#endif /* RL78_G1A_TB */

		#ifndef LED_BIT
			#error The hardware platform is not defined
		#endif

	#endif /* INCLUDED_FROM_FREERTOS_ASM_FILE */

#endif /* LED_IO_H */

