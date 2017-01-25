/**
 * \file
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAM4E_AFEC_COMPONENT_
#define _SAM4E_AFEC_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Analog-Front-End Controller */
/* ============================================================================= */
/** \addtogroup SAM4E_AFEC Analog-Front-End Controller */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Afec hardware registers */
typedef struct {
  WoReg AFEC_CR;       /**< \brief (Afec Offset: 0x00) Control Register */
  RwReg AFEC_MR;       /**< \brief (Afec Offset: 0x04) Mode Register */
  RwReg AFEC_EMR;      /**< \brief (Afec Offset: 0x08) Extended Mode Register */
  RwReg AFEC_SEQ1R;    /**< \brief (Afec Offset: 0x0C) Channel Sequence 1 Register */
  RwReg AFEC_SEQ2R;    /**< \brief (Afec Offset: 0x10) Channel Sequence 2 Register */
  WoReg AFEC_CHER;     /**< \brief (Afec Offset: 0x14) Channel Enable Register */
  WoReg AFEC_CHDR;     /**< \brief (Afec Offset: 0x18) Channel Disable Register */
  RoReg AFEC_CHSR;     /**< \brief (Afec Offset: 0x1C) Channel Status Register */
  RoReg AFEC_LCDR;     /**< \brief (Afec Offset: 0x20) Last Converted Data Register */
  WoReg AFEC_IER;      /**< \brief (Afec Offset: 0x24) Interrupt Enable Register */
  WoReg AFEC_IDR;      /**< \brief (Afec Offset: 0x28) Interrupt Disable Register */
  RoReg AFEC_IMR;      /**< \brief (Afec Offset: 0x2C) Interrupt Mask Register */
  RoReg AFEC_ISR;      /**< \brief (Afec Offset: 0x30) Interrupt Status Register */
  RoReg Reserved1[6];
  RoReg AFEC_OVER;     /**< \brief (Afec Offset: 0x4C) Overrun Status Register */
  RwReg AFEC_CWR;      /**< \brief (Afec Offset: 0x50) Compare Window Register */
  RwReg AFEC_CGR;      /**< \brief (Afec Offset: 0x54) Channel Gain Register */
  RoReg Reserved2[1];
  RwReg AFEC_CDOR;     /**< \brief (Afec Offset: 0x5C) Channel Calibration DC Offset Register */
  RwReg AFEC_DIFFR;    /**< \brief (Afec Offset: 0x60) Channel Differential Register */
  RwReg AFEC_CSELR;    /**< \brief (Afec Offset: 0x64) Channel Register Selection */
  RoReg AFEC_CDR;      /**< \brief (Afec Offset: 0x68) Channel Data Register */
  RwReg AFEC_COCR;     /**< \brief (Afec Offset: 0x6C) Channel Offset Compensation Register */
  RwReg AFEC_TEMPMR;   /**< \brief (Afec Offset: 0x70) Temperature Sensor Mode Register */
  RwReg AFEC_TEMPCWR;  /**< \brief (Afec Offset: 0x74) Temperature Compare Window Register */
  RoReg Reserved3[7];
  RwReg AFEC_ACR;      /**< \brief (Afec Offset: 0x94) Analog Control Register */
  RoReg Reserved4[19];
  RwReg AFEC_WPMR;     /**< \brief (Afec Offset: 0xE4) Write Protect Mode Register */
  RoReg AFEC_WPSR;     /**< \brief (Afec Offset: 0xE8) Write Protect Status Register */
  RoReg Reserved5[5];
  RwReg AFEC_RPR;      /**< \brief (Afec Offset: 0x100) Receive Pointer Register */
  RwReg AFEC_RCR;      /**< \brief (Afec Offset: 0x104) Receive Counter Register */
  RoReg Reserved6[2];
  RwReg AFEC_RNPR;     /**< \brief (Afec Offset: 0x110) Receive Next Pointer Register */
  RwReg AFEC_RNCR;     /**< \brief (Afec Offset: 0x114) Receive Next Counter Register */
  RoReg Reserved7[2];
  WoReg AFEC_PTCR;     /**< \brief (Afec Offset: 0x120) Transfer Control Register */
  RoReg AFEC_PTSR;     /**< \brief (Afec Offset: 0x124) Transfer Status Register */
} Afec;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- AFEC_CR : (AFEC Offset: 0x00) Control Register -------- */
#define AFEC_CR_SWRST (0x1u << 0) /**< \brief (AFEC_CR) Software Reset */
#define AFEC_CR_START (0x1u << 1) /**< \brief (AFEC_CR) Start Conversion */
#define AFEC_CR_AUTOCAL (0x1u << 3) /**< \brief (AFEC_CR) Automatic Calibration of AFEC */
/* -------- AFEC_MR : (AFEC Offset: 0x04) Mode Register -------- */
#define AFEC_MR_TRGEN (0x1u << 0) /**< \brief (AFEC_MR) Trigger Enable */
#define   AFEC_MR_TRGEN_DIS (0x0u << 0) /**< \brief (AFEC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. */
#define   AFEC_MR_TRGEN_EN (0x1u << 0) /**< \brief (AFEC_MR) Hardware trigger selected by TRGSEL field is enabled. */
#define AFEC_MR_TRGSEL_Pos 1
#define AFEC_MR_TRGSEL_Msk (0x7u << AFEC_MR_TRGSEL_Pos) /**< \brief (AFEC_MR) Trigger Selection */
#define   AFEC_MR_TRGSEL_AFEC_TRIG0 (0x0u << 1) /**< \brief (AFEC_MR) ADTRG pin */
#define   AFEC_MR_TRGSEL_AFEC_TRIG1 (0x1u << 1) /**< \brief (AFEC_MR) TIO Output of the Timer Counter Channel 0 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG2 (0x2u << 1) /**< \brief (AFEC_MR) TIO Output of the Timer Counter Channel 1 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG3 (0x3u << 1) /**< \brief (AFEC_MR) TIO Output of the Timer Counter Channel 2 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG4 (0x4u << 1) /**< \brief (AFEC_MR) PWM Event Line 0 */
#define   AFEC_MR_TRGSEL_AFEC_TRIG5 (0x5u << 1) /**< \brief (AFEC_MR) PWM Event Line 1 */
#define AFEC_MR_SLEEP (0x1u << 5) /**< \brief (AFEC_MR) Sleep Mode */
#define   AFEC_MR_SLEEP_NORMAL (0x0u << 5) /**< \brief (AFEC_MR) Normal Mode: The AFEC Core and reference voltage circuitry are kept ON between conversions */
#define   AFEC_MR_SLEEP_SLEEP (0x1u << 5) /**< \brief (AFEC_MR) Sleep Mode: The AFEC Core and reference voltage circuitry are OFF between conversions */
#define AFEC_MR_FWUP (0x1u << 6) /**< \brief (AFEC_MR) Fast Wake Up */
#define   AFEC_MR_FWUP_OFF (0x0u << 6) /**< \brief (AFEC_MR) Normal Sleep Mode: The sleep mode is defined by the SLEEP bit */
#define   AFEC_MR_FWUP_ON (0x1u << 6) /**< \brief (AFEC_MR) Fast Wake Up Sleep Mode: The Voltage reference is ON between conversions and AFEC Core is OFF */
#define AFEC_MR_FREERUN (0x1u << 7) /**< \brief (AFEC_MR) Free Run Mode */
#define   AFEC_MR_FREERUN_OFF (0x0u << 7) /**< \brief (AFEC_MR) Normal Mode */
#define   AFEC_MR_FREERUN_ON (0x1u << 7) /**< \brief (AFEC_MR) Free Run Mode: Never wait for any trigger. */
#define AFEC_MR_PRESCAL_Pos 8
#define AFEC_MR_PRESCAL_Msk (0xffu << AFEC_MR_PRESCAL_Pos) /**< \brief (AFEC_MR) Prescaler Rate Selection */
#define AFEC_MR_PRESCAL(value) ((AFEC_MR_PRESCAL_Msk & ((value) << AFEC_MR_PRESCAL_Pos)))
#define AFEC_MR_STARTUP_Pos 16
#define AFEC_MR_STARTUP_Msk (0xfu << AFEC_MR_STARTUP_Pos) /**< \brief (AFEC_MR) Start Up Time */
#define   AFEC_MR_STARTUP_SUT0 (0x0u << 16) /**< \brief (AFEC_MR) 0 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT8 (0x1u << 16) /**< \brief (AFEC_MR) 8 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT16 (0x2u << 16) /**< \brief (AFEC_MR) 16 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT24 (0x3u << 16) /**< \brief (AFEC_MR) 24 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT64 (0x4u << 16) /**< \brief (AFEC_MR) 64 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT80 (0x5u << 16) /**< \brief (AFEC_MR) 80 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT96 (0x6u << 16) /**< \brief (AFEC_MR) 96 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT112 (0x7u << 16) /**< \brief (AFEC_MR) 112 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT512 (0x8u << 16) /**< \brief (AFEC_MR) 512 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT576 (0x9u << 16) /**< \brief (AFEC_MR) 576 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT640 (0xAu << 16) /**< \brief (AFEC_MR) 640 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT704 (0xBu << 16) /**< \brief (AFEC_MR) 704 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT768 (0xCu << 16) /**< \brief (AFEC_MR) 768 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT832 (0xDu << 16) /**< \brief (AFEC_MR) 832 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT896 (0xEu << 16) /**< \brief (AFEC_MR) 896 periods of AFEClock */
#define   AFEC_MR_STARTUP_SUT960 (0xFu << 16) /**< \brief (AFEC_MR) 960 periods of AFEClock */
#define AFEC_MR_SETTLING_Pos 20
#define AFEC_MR_SETTLING_Msk (0x3u << AFEC_MR_SETTLING_Pos) /**< \brief (AFEC_MR) Analog Settling Time */
#define   AFEC_MR_SETTLING_AST3 (0x0u << 20) /**< \brief (AFEC_MR) 3 periods of AFEClock */
#define   AFEC_MR_SETTLING_AST5 (0x1u << 20) /**< \brief (AFEC_MR) 5 periods of AFEClock */
#define   AFEC_MR_SETTLING_AST9 (0x2u << 20) /**< \brief (AFEC_MR) 9 periods of AFEClock */
#define   AFEC_MR_SETTLING_AST17 (0x3u << 20) /**< \brief (AFEC_MR) 17 periods of AFEClock */
#define AFEC_MR_ANACH (0x1u << 23) /**< \brief (AFEC_MR) Analog Change */
#define   AFEC_MR_ANACH_NONE (0x0u << 23) /**< \brief (AFEC_MR) No analog change on channel switching: DIFF0, GAIN0 are used for all channels */
#define   AFEC_MR_ANACH_ALLOWED (0x1u << 23) /**< \brief (AFEC_MR) Allows different analog settings for each channel. See AFEC_CGR Register. */
#define AFEC_MR_TRACKTIM_Pos 24
#define AFEC_MR_TRACKTIM_Msk (0xfu << AFEC_MR_TRACKTIM_Pos) /**< \brief (AFEC_MR) Tracking Time */
#define AFEC_MR_TRACKTIM(value) ((AFEC_MR_TRACKTIM_Msk & ((value) << AFEC_MR_TRACKTIM_Pos)))
#define AFEC_MR_TRANSFER_Pos 28
#define AFEC_MR_TRANSFER_Msk (0x3u << AFEC_MR_TRANSFER_Pos) /**< \brief (AFEC_MR) Transfer Period */
#define AFEC_MR_TRANSFER(value) ((AFEC_MR_TRANSFER_Msk & ((value) << AFEC_MR_TRANSFER_Pos)))
#define AFEC_MR_USEQ (0x1u << 31) /**< \brief (AFEC_MR) Use Sequence Enable */
#define   AFEC_MR_USEQ_NUM_ORDER (0x0u << 31) /**< \brief (AFEC_MR) Normal Mode: The controller converts channels in a simple numeric order. */
#define   AFEC_MR_USEQ_REG_ORDER (0x1u << 31) /**< \brief (AFEC_MR) User Sequence Mode: The sequence respects what is defined in AFEC_SEQR1 and AFEC_SEQR2 registers. */
/* -------- AFEC_EMR : (AFEC Offset: 0x08) Extended Mode Register -------- */
#define AFEC_EMR_CMPMODE_Pos 0
#define AFEC_EMR_CMPMODE_Msk (0x3u << AFEC_EMR_CMPMODE_Pos) /**< \brief (AFEC_EMR) Comparison Mode */
#define   AFEC_EMR_CMPMODE_LOW (0x0u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is lower than the low threshold of the window. */
#define   AFEC_EMR_CMPMODE_HIGH (0x1u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is higher than the high threshold of the window. */
#define   AFEC_EMR_CMPMODE_IN (0x2u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is in the comparison window. */
#define   AFEC_EMR_CMPMODE_OUT (0x3u << 0) /**< \brief (AFEC_EMR) Generates an event when the converted data is out of the comparison window. */
#define AFEC_EMR_CMPSEL_Pos 3
#define AFEC_EMR_CMPSEL_Msk (0x1fu << AFEC_EMR_CMPSEL_Pos) /**< \brief (AFEC_EMR) Comparison Selected Channel */
#define AFEC_EMR_CMPSEL(value) ((AFEC_EMR_CMPSEL_Msk & ((value) << AFEC_EMR_CMPSEL_Pos)))
#define AFEC_EMR_CMPALL (0x1u << 9) /**< \brief (AFEC_EMR) Compare All Channels */
#define AFEC_EMR_CMPFILTER_Pos 12
#define AFEC_EMR_CMPFILTER_Msk (0x3u << AFEC_EMR_CMPFILTER_Pos) /**< \brief (AFEC_EMR) Compare Event Filtering */
#define AFEC_EMR_CMPFILTER(value) ((AFEC_EMR_CMPFILTER_Msk & ((value) << AFEC_EMR_CMPFILTER_Pos)))
#define AFEC_EMR_RES_Pos 16
#define AFEC_EMR_RES_Msk (0x7u << AFEC_EMR_RES_Pos) /**< \brief (AFEC_EMR) Resolution */
#define   AFEC_EMR_RES_NO_AVERAGE (0x0u << 16) /**< \brief (AFEC_EMR) 12-bit resolution, AFEC sample rate is maximum (no averaging). */
#define   AFEC_EMR_RES_LOW_RES (0x1u << 16) /**< \brief (AFEC_EMR) 10-bit resolution, AFEC sample rate is maximum (no averaging). */
#define   AFEC_EMR_RES_OSR4 (0x2u << 16) /**< \brief (AFEC_EMR) 13-bit resolution, AFEC sample rate divided by 4 (averaging). */
#define   AFEC_EMR_RES_OSR16 (0x3u << 16) /**< \brief (AFEC_EMR) 14-bit resolution, AFEC sample rate divided by 16 (averaging). */
#define   AFEC_EMR_RES_OSR64 (0x4u << 16) /**< \brief (AFEC_EMR) 15-bit resolution, AFEC sample rate divided by 64 (averaging). */
#define   AFEC_EMR_RES_OSR256 (0x5u << 16) /**< \brief (AFEC_EMR) 16-bit resolution, AFEC sample rate divided by 256 (averaging). */
#define AFEC_EMR_TAG (0x1u << 24) /**< \brief (AFEC_EMR) TAG of AFEC_LDCR register */
#define AFEC_EMR_STM (0x1u << 25) /**< \brief (AFEC_EMR) Single Trigger Mode */
/* -------- AFEC_SEQ1R : (AFEC Offset: 0x0C) Channel Sequence 1 Register -------- */
#define AFEC_SEQ1R_USCH0_Pos 0
#define AFEC_SEQ1R_USCH0_Msk (0xfu << AFEC_SEQ1R_USCH0_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 0 */
#define AFEC_SEQ1R_USCH0(value) ((AFEC_SEQ1R_USCH0_Msk & ((value) << AFEC_SEQ1R_USCH0_Pos)))
#define AFEC_SEQ1R_USCH1_Pos 4
#define AFEC_SEQ1R_USCH1_Msk (0xfu << AFEC_SEQ1R_USCH1_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 1 */
#define AFEC_SEQ1R_USCH1(value) ((AFEC_SEQ1R_USCH1_Msk & ((value) << AFEC_SEQ1R_USCH1_Pos)))
#define AFEC_SEQ1R_USCH2_Pos 8
#define AFEC_SEQ1R_USCH2_Msk (0xfu << AFEC_SEQ1R_USCH2_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 2 */
#define AFEC_SEQ1R_USCH2(value) ((AFEC_SEQ1R_USCH2_Msk & ((value) << AFEC_SEQ1R_USCH2_Pos)))
#define AFEC_SEQ1R_USCH3_Pos 12
#define AFEC_SEQ1R_USCH3_Msk (0xfu << AFEC_SEQ1R_USCH3_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 3 */
#define AFEC_SEQ1R_USCH3(value) ((AFEC_SEQ1R_USCH3_Msk & ((value) << AFEC_SEQ1R_USCH3_Pos)))
#define AFEC_SEQ1R_USCH4_Pos 16
#define AFEC_SEQ1R_USCH4_Msk (0xfu << AFEC_SEQ1R_USCH4_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 4 */
#define AFEC_SEQ1R_USCH4(value) ((AFEC_SEQ1R_USCH4_Msk & ((value) << AFEC_SEQ1R_USCH4_Pos)))
#define AFEC_SEQ1R_USCH5_Pos 20
#define AFEC_SEQ1R_USCH5_Msk (0xfu << AFEC_SEQ1R_USCH5_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 5 */
#define AFEC_SEQ1R_USCH5(value) ((AFEC_SEQ1R_USCH5_Msk & ((value) << AFEC_SEQ1R_USCH5_Pos)))
#define AFEC_SEQ1R_USCH6_Pos 24
#define AFEC_SEQ1R_USCH6_Msk (0xfu << AFEC_SEQ1R_USCH6_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 6 */
#define AFEC_SEQ1R_USCH6(value) ((AFEC_SEQ1R_USCH6_Msk & ((value) << AFEC_SEQ1R_USCH6_Pos)))
#define AFEC_SEQ1R_USCH7_Pos 28
#define AFEC_SEQ1R_USCH7_Msk (0xfu << AFEC_SEQ1R_USCH7_Pos) /**< \brief (AFEC_SEQ1R) User Sequence Number 7 */
#define AFEC_SEQ1R_USCH7(value) ((AFEC_SEQ1R_USCH7_Msk & ((value) << AFEC_SEQ1R_USCH7_Pos)))
/* -------- AFEC_SEQ2R : (AFEC Offset: 0x10) Channel Sequence 2 Register -------- */
#define AFEC_SEQ2R_USCH8_Pos 0
#define AFEC_SEQ2R_USCH8_Msk (0xfu << AFEC_SEQ2R_USCH8_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 8 */
#define AFEC_SEQ2R_USCH8(value) ((AFEC_SEQ2R_USCH8_Msk & ((value) << AFEC_SEQ2R_USCH8_Pos)))
#define AFEC_SEQ2R_USCH9_Pos 4
#define AFEC_SEQ2R_USCH9_Msk (0xfu << AFEC_SEQ2R_USCH9_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 9 */
#define AFEC_SEQ2R_USCH9(value) ((AFEC_SEQ2R_USCH9_Msk & ((value) << AFEC_SEQ2R_USCH9_Pos)))
#define AFEC_SEQ2R_USCH10_Pos 8
#define AFEC_SEQ2R_USCH10_Msk (0xfu << AFEC_SEQ2R_USCH10_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 10 */
#define AFEC_SEQ2R_USCH10(value) ((AFEC_SEQ2R_USCH10_Msk & ((value) << AFEC_SEQ2R_USCH10_Pos)))
#define AFEC_SEQ2R_USCH11_Pos 12
#define AFEC_SEQ2R_USCH11_Msk (0xfu << AFEC_SEQ2R_USCH11_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 11 */
#define AFEC_SEQ2R_USCH11(value) ((AFEC_SEQ2R_USCH11_Msk & ((value) << AFEC_SEQ2R_USCH11_Pos)))
#define AFEC_SEQ2R_USCH12_Pos 16
#define AFEC_SEQ2R_USCH12_Msk (0xfu << AFEC_SEQ2R_USCH12_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 12 */
#define AFEC_SEQ2R_USCH12(value) ((AFEC_SEQ2R_USCH12_Msk & ((value) << AFEC_SEQ2R_USCH12_Pos)))
#define AFEC_SEQ2R_USCH13_Pos 20
#define AFEC_SEQ2R_USCH13_Msk (0xfu << AFEC_SEQ2R_USCH13_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 13 */
#define AFEC_SEQ2R_USCH13(value) ((AFEC_SEQ2R_USCH13_Msk & ((value) << AFEC_SEQ2R_USCH13_Pos)))
#define AFEC_SEQ2R_USCH14_Pos 24
#define AFEC_SEQ2R_USCH14_Msk (0xfu << AFEC_SEQ2R_USCH14_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 14 */
#define AFEC_SEQ2R_USCH14(value) ((AFEC_SEQ2R_USCH14_Msk & ((value) << AFEC_SEQ2R_USCH14_Pos)))
#define AFEC_SEQ2R_USCH15_Pos 28
#define AFEC_SEQ2R_USCH15_Msk (0xfu << AFEC_SEQ2R_USCH15_Pos) /**< \brief (AFEC_SEQ2R) User Sequence Number 15 */
#define AFEC_SEQ2R_USCH15(value) ((AFEC_SEQ2R_USCH15_Msk & ((value) << AFEC_SEQ2R_USCH15_Pos)))
/* -------- AFEC_CHER : (AFEC Offset: 0x14) Channel Enable Register -------- */
#define AFEC_CHER_CH0 (0x1u << 0) /**< \brief (AFEC_CHER) Channel 0 Enable */
#define AFEC_CHER_CH1 (0x1u << 1) /**< \brief (AFEC_CHER) Channel 1 Enable */
#define AFEC_CHER_CH2 (0x1u << 2) /**< \brief (AFEC_CHER) Channel 2 Enable */
#define AFEC_CHER_CH3 (0x1u << 3) /**< \brief (AFEC_CHER) Channel 3 Enable */
#define AFEC_CHER_CH4 (0x1u << 4) /**< \brief (AFEC_CHER) Channel 4 Enable */
#define AFEC_CHER_CH5 (0x1u << 5) /**< \brief (AFEC_CHER) Channel 5 Enable */
#define AFEC_CHER_CH6 (0x1u << 6) /**< \brief (AFEC_CHER) Channel 6 Enable */
#define AFEC_CHER_CH7 (0x1u << 7) /**< \brief (AFEC_CHER) Channel 7 Enable */
#define AFEC_CHER_CH8 (0x1u << 8) /**< \brief (AFEC_CHER) Channel 8 Enable */
#define AFEC_CHER_CH9 (0x1u << 9) /**< \brief (AFEC_CHER) Channel 9 Enable */
#define AFEC_CHER_CH10 (0x1u << 10) /**< \brief (AFEC_CHER) Channel 10 Enable */
#define AFEC_CHER_CH11 (0x1u << 11) /**< \brief (AFEC_CHER) Channel 11 Enable */
#define AFEC_CHER_CH12 (0x1u << 12) /**< \brief (AFEC_CHER) Channel 12 Enable */
#define AFEC_CHER_CH13 (0x1u << 13) /**< \brief (AFEC_CHER) Channel 13 Enable */
#define AFEC_CHER_CH14 (0x1u << 14) /**< \brief (AFEC_CHER) Channel 14 Enable */
#define AFEC_CHER_CH15 (0x1u << 15) /**< \brief (AFEC_CHER) Channel 15 Enable */
/* -------- AFEC_CHDR : (AFEC Offset: 0x18) Channel Disable Register -------- */
#define AFEC_CHDR_CH0 (0x1u << 0) /**< \brief (AFEC_CHDR) Channel 0 Disable */
#define AFEC_CHDR_CH1 (0x1u << 1) /**< \brief (AFEC_CHDR) Channel 1 Disable */
#define AFEC_CHDR_CH2 (0x1u << 2) /**< \brief (AFEC_CHDR) Channel 2 Disable */
#define AFEC_CHDR_CH3 (0x1u << 3) /**< \brief (AFEC_CHDR) Channel 3 Disable */
#define AFEC_CHDR_CH4 (0x1u << 4) /**< \brief (AFEC_CHDR) Channel 4 Disable */
#define AFEC_CHDR_CH5 (0x1u << 5) /**< \brief (AFEC_CHDR) Channel 5 Disable */
#define AFEC_CHDR_CH6 (0x1u << 6) /**< \brief (AFEC_CHDR) Channel 6 Disable */
#define AFEC_CHDR_CH7 (0x1u << 7) /**< \brief (AFEC_CHDR) Channel 7 Disable */
#define AFEC_CHDR_CH8 (0x1u << 8) /**< \brief (AFEC_CHDR) Channel 8 Disable */
#define AFEC_CHDR_CH9 (0x1u << 9) /**< \brief (AFEC_CHDR) Channel 9 Disable */
#define AFEC_CHDR_CH10 (0x1u << 10) /**< \brief (AFEC_CHDR) Channel 10 Disable */
#define AFEC_CHDR_CH11 (0x1u << 11) /**< \brief (AFEC_CHDR) Channel 11 Disable */
#define AFEC_CHDR_CH12 (0x1u << 12) /**< \brief (AFEC_CHDR) Channel 12 Disable */
#define AFEC_CHDR_CH13 (0x1u << 13) /**< \brief (AFEC_CHDR) Channel 13 Disable */
#define AFEC_CHDR_CH14 (0x1u << 14) /**< \brief (AFEC_CHDR) Channel 14 Disable */
#define AFEC_CHDR_CH15 (0x1u << 15) /**< \brief (AFEC_CHDR) Channel 15 Disable */
/* -------- AFEC_CHSR : (AFEC Offset: 0x1C) Channel Status Register -------- */
#define AFEC_CHSR_CH0 (0x1u << 0) /**< \brief (AFEC_CHSR) Channel 0 Status */
#define AFEC_CHSR_CH1 (0x1u << 1) /**< \brief (AFEC_CHSR) Channel 1 Status */
#define AFEC_CHSR_CH2 (0x1u << 2) /**< \brief (AFEC_CHSR) Channel 2 Status */
#define AFEC_CHSR_CH3 (0x1u << 3) /**< \brief (AFEC_CHSR) Channel 3 Status */
#define AFEC_CHSR_CH4 (0x1u << 4) /**< \brief (AFEC_CHSR) Channel 4 Status */
#define AFEC_CHSR_CH5 (0x1u << 5) /**< \brief (AFEC_CHSR) Channel 5 Status */
#define AFEC_CHSR_CH6 (0x1u << 6) /**< \brief (AFEC_CHSR) Channel 6 Status */
#define AFEC_CHSR_CH7 (0x1u << 7) /**< \brief (AFEC_CHSR) Channel 7 Status */
#define AFEC_CHSR_CH8 (0x1u << 8) /**< \brief (AFEC_CHSR) Channel 8 Status */
#define AFEC_CHSR_CH9 (0x1u << 9) /**< \brief (AFEC_CHSR) Channel 9 Status */
#define AFEC_CHSR_CH10 (0x1u << 10) /**< \brief (AFEC_CHSR) Channel 10 Status */
#define AFEC_CHSR_CH11 (0x1u << 11) /**< \brief (AFEC_CHSR) Channel 11 Status */
#define AFEC_CHSR_CH12 (0x1u << 12) /**< \brief (AFEC_CHSR) Channel 12 Status */
#define AFEC_CHSR_CH13 (0x1u << 13) /**< \brief (AFEC_CHSR) Channel 13 Status */
#define AFEC_CHSR_CH14 (0x1u << 14) /**< \brief (AFEC_CHSR) Channel 14 Status */
#define AFEC_CHSR_CH15 (0x1u << 15) /**< \brief (AFEC_CHSR) Channel 15 Status */
/* -------- AFEC_LCDR : (AFEC Offset: 0x20) Last Converted Data Register -------- */
#define AFEC_LCDR_LDATA_Pos 0
#define AFEC_LCDR_LDATA_Msk (0xffffu << AFEC_LCDR_LDATA_Pos) /**< \brief (AFEC_LCDR) Last Data Converted */
#define AFEC_LCDR_CHNB_Pos 24
#define AFEC_LCDR_CHNB_Msk (0xfu << AFEC_LCDR_CHNB_Pos) /**< \brief (AFEC_LCDR) Channel Number */
/* -------- AFEC_IER : (AFEC Offset: 0x24) Interrupt Enable Register -------- */
#define AFEC_IER_EOC0 (0x1u << 0) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 0 */
#define AFEC_IER_EOC1 (0x1u << 1) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 1 */
#define AFEC_IER_EOC2 (0x1u << 2) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 2 */
#define AFEC_IER_EOC3 (0x1u << 3) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 3 */
#define AFEC_IER_EOC4 (0x1u << 4) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 4 */
#define AFEC_IER_EOC5 (0x1u << 5) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 5 */
#define AFEC_IER_EOC6 (0x1u << 6) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 6 */
#define AFEC_IER_EOC7 (0x1u << 7) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 7 */
#define AFEC_IER_EOC8 (0x1u << 8) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 8 */
#define AFEC_IER_EOC9 (0x1u << 9) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 9 */
#define AFEC_IER_EOC10 (0x1u << 10) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 10 */
#define AFEC_IER_EOC11 (0x1u << 11) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 11 */
#define AFEC_IER_EOC12 (0x1u << 12) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 12 */
#define AFEC_IER_EOC13 (0x1u << 13) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 13 */
#define AFEC_IER_EOC14 (0x1u << 14) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 14 */
#define AFEC_IER_EOC15 (0x1u << 15) /**< \brief (AFEC_IER) End of Conversion Interrupt Enable 15 */
#define AFEC_IER_DRDY (0x1u << 24) /**< \brief (AFEC_IER) Data Ready Interrupt Enable */
#define AFEC_IER_GOVRE (0x1u << 25) /**< \brief (AFEC_IER) General Overrun Error Interrupt Enable */
#define AFEC_IER_COMPE (0x1u << 26) /**< \brief (AFEC_IER) Comparison Event Interrupt Enable+ */
#define AFEC_IER_ENDRX (0x1u << 27) /**< \brief (AFEC_IER) End of Receive Buffer Interrupt Enable */
#define AFEC_IER_RXBUFF (0x1u << 28) /**< \brief (AFEC_IER) Receive Buffer Full Interrupt Enable */
#define AFEC_IER_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IER) Temperature Change Interrupt Enable */
#define AFEC_IER_EOCAL (0x1u << 31) /**< \brief (AFEC_IER) End of Calibration Sequence Interrupt Enable */
/* -------- AFEC_IDR : (AFEC Offset: 0x28) Interrupt Disable Register -------- */
#define AFEC_IDR_EOC0 (0x1u << 0) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 0 */
#define AFEC_IDR_EOC1 (0x1u << 1) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 1 */
#define AFEC_IDR_EOC2 (0x1u << 2) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 2 */
#define AFEC_IDR_EOC3 (0x1u << 3) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 3 */
#define AFEC_IDR_EOC4 (0x1u << 4) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 4 */
#define AFEC_IDR_EOC5 (0x1u << 5) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 5 */
#define AFEC_IDR_EOC6 (0x1u << 6) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 6 */
#define AFEC_IDR_EOC7 (0x1u << 7) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 7 */
#define AFEC_IDR_EOC8 (0x1u << 8) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 8 */
#define AFEC_IDR_EOC9 (0x1u << 9) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 9 */
#define AFEC_IDR_EOC10 (0x1u << 10) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 10 */
#define AFEC_IDR_EOC11 (0x1u << 11) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 11 */
#define AFEC_IDR_EOC12 (0x1u << 12) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 12 */
#define AFEC_IDR_EOC13 (0x1u << 13) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 13 */
#define AFEC_IDR_EOC14 (0x1u << 14) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 14 */
#define AFEC_IDR_EOC15 (0x1u << 15) /**< \brief (AFEC_IDR) End of Conversion Interrupt Disable 15 */
#define AFEC_IDR_DRDY (0x1u << 24) /**< \brief (AFEC_IDR) Data Ready Interrupt Disable */
#define AFEC_IDR_GOVRE (0x1u << 25) /**< \brief (AFEC_IDR) General Overrun Error Interrupt Disable */
#define AFEC_IDR_COMPE (0x1u << 26) /**< \brief (AFEC_IDR) Comparison Event Interrupt Disable */
#define AFEC_IDR_ENDRX (0x1u << 27) /**< \brief (AFEC_IDR) End of Receive Buffer Interrupt Disable */
#define AFEC_IDR_RXBUFF (0x1u << 28) /**< \brief (AFEC_IDR) Receive Buffer Full Interrupt Disable */
#define AFEC_IDR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IDR) Temperature Change Interrupt Disable */
#define AFEC_IDR_EOCAL (0x1u << 31) /**< \brief (AFEC_IDR) End of Calibration Sequence Interrupt Disable */
/* -------- AFEC_IMR : (AFEC Offset: 0x2C) Interrupt Mask Register -------- */
#define AFEC_IMR_EOC0 (0x1u << 0) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 0 */
#define AFEC_IMR_EOC1 (0x1u << 1) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 1 */
#define AFEC_IMR_EOC2 (0x1u << 2) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 2 */
#define AFEC_IMR_EOC3 (0x1u << 3) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 3 */
#define AFEC_IMR_EOC4 (0x1u << 4) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 4 */
#define AFEC_IMR_EOC5 (0x1u << 5) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 5 */
#define AFEC_IMR_EOC6 (0x1u << 6) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 6 */
#define AFEC_IMR_EOC7 (0x1u << 7) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 7 */
#define AFEC_IMR_EOC8 (0x1u << 8) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 8 */
#define AFEC_IMR_EOC9 (0x1u << 9) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 9 */
#define AFEC_IMR_EOC10 (0x1u << 10) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 10 */
#define AFEC_IMR_EOC11 (0x1u << 11) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 11 */
#define AFEC_IMR_EOC12 (0x1u << 12) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 12 */
#define AFEC_IMR_EOC13 (0x1u << 13) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 13 */
#define AFEC_IMR_EOC14 (0x1u << 14) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 14 */
#define AFEC_IMR_EOC15 (0x1u << 15) /**< \brief (AFEC_IMR) End of Conversion Interrupt Mask 15 */
#define AFEC_IMR_DRDY (0x1u << 24) /**< \brief (AFEC_IMR) Data Ready Interrupt Mask */
#define AFEC_IMR_GOVRE (0x1u << 25) /**< \brief (AFEC_IMR) General Overrun Error Interrupt Mask */
#define AFEC_IMR_COMPE (0x1u << 26) /**< \brief (AFEC_IMR) Comparison Event Interrupt Mask */
#define AFEC_IMR_ENDRX (0x1u << 27) /**< \brief (AFEC_IMR) End of Receive Buffer Interrupt Mask */
#define AFEC_IMR_RXBUFF (0x1u << 28) /**< \brief (AFEC_IMR) Receive Buffer Full Interrupt Mask */
#define AFEC_IMR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_IMR) Temperature Change Interrupt Mask */
#define AFEC_IMR_EOCAL (0x1u << 31) /**< \brief (AFEC_IMR) End of Calibration Sequence Interrupt Mask */
/* -------- AFEC_ISR : (AFEC Offset: 0x30) Interrupt Status Register -------- */
#define AFEC_ISR_EOC0 (0x1u << 0) /**< \brief (AFEC_ISR) End of Conversion 0 */
#define AFEC_ISR_EOC1 (0x1u << 1) /**< \brief (AFEC_ISR) End of Conversion 1 */
#define AFEC_ISR_EOC2 (0x1u << 2) /**< \brief (AFEC_ISR) End of Conversion 2 */
#define AFEC_ISR_EOC3 (0x1u << 3) /**< \brief (AFEC_ISR) End of Conversion 3 */
#define AFEC_ISR_EOC4 (0x1u << 4) /**< \brief (AFEC_ISR) End of Conversion 4 */
#define AFEC_ISR_EOC5 (0x1u << 5) /**< \brief (AFEC_ISR) End of Conversion 5 */
#define AFEC_ISR_EOC6 (0x1u << 6) /**< \brief (AFEC_ISR) End of Conversion 6 */
#define AFEC_ISR_EOC7 (0x1u << 7) /**< \brief (AFEC_ISR) End of Conversion 7 */
#define AFEC_ISR_EOC8 (0x1u << 8) /**< \brief (AFEC_ISR) End of Conversion 8 */
#define AFEC_ISR_EOC9 (0x1u << 9) /**< \brief (AFEC_ISR) End of Conversion 9 */
#define AFEC_ISR_EOC10 (0x1u << 10) /**< \brief (AFEC_ISR) End of Conversion 10 */
#define AFEC_ISR_EOC11 (0x1u << 11) /**< \brief (AFEC_ISR) End of Conversion 11 */
#define AFEC_ISR_EOC12 (0x1u << 12) /**< \brief (AFEC_ISR) End of Conversion 12 */
#define AFEC_ISR_EOC13 (0x1u << 13) /**< \brief (AFEC_ISR) End of Conversion 13 */
#define AFEC_ISR_EOC14 (0x1u << 14) /**< \brief (AFEC_ISR) End of Conversion 14 */
#define AFEC_ISR_EOC15 (0x1u << 15) /**< \brief (AFEC_ISR) End of Conversion 15 */
#define AFEC_ISR_DRDY (0x1u << 24) /**< \brief (AFEC_ISR) Data Ready */
#define AFEC_ISR_GOVRE (0x1u << 25) /**< \brief (AFEC_ISR) General Overrun Error */
#define AFEC_ISR_COMPE (0x1u << 26) /**< \brief (AFEC_ISR) Comparison Error */
#define AFEC_ISR_ENDRX (0x1u << 27) /**< \brief (AFEC_ISR) End of RX Buffer */
#define AFEC_ISR_RXBUFF (0x1u << 28) /**< \brief (AFEC_ISR) RX Buffer Full */
#define AFEC_ISR_TEMPCHG (0x1u << 30) /**< \brief (AFEC_ISR) Temperature Change */
#define AFEC_ISR_EOCAL (0x1u << 31) /**< \brief (AFEC_ISR) End of Calibration Sequence */
/* -------- AFEC_OVER : (AFEC Offset: 0x4C) Overrun Status Register -------- */
#define AFEC_OVER_OVRE0 (0x1u << 0) /**< \brief (AFEC_OVER) Overrun Error 0 */
#define AFEC_OVER_OVRE1 (0x1u << 1) /**< \brief (AFEC_OVER) Overrun Error 1 */
#define AFEC_OVER_OVRE2 (0x1u << 2) /**< \brief (AFEC_OVER) Overrun Error 2 */
#define AFEC_OVER_OVRE3 (0x1u << 3) /**< \brief (AFEC_OVER) Overrun Error 3 */
#define AFEC_OVER_OVRE4 (0x1u << 4) /**< \brief (AFEC_OVER) Overrun Error 4 */
#define AFEC_OVER_OVRE5 (0x1u << 5) /**< \brief (AFEC_OVER) Overrun Error 5 */
#define AFEC_OVER_OVRE6 (0x1u << 6) /**< \brief (AFEC_OVER) Overrun Error 6 */
#define AFEC_OVER_OVRE7 (0x1u << 7) /**< \brief (AFEC_OVER) Overrun Error 7 */
#define AFEC_OVER_OVRE8 (0x1u << 8) /**< \brief (AFEC_OVER) Overrun Error 8 */
#define AFEC_OVER_OVRE9 (0x1u << 9) /**< \brief (AFEC_OVER) Overrun Error 9 */
#define AFEC_OVER_OVRE10 (0x1u << 10) /**< \brief (AFEC_OVER) Overrun Error 10 */
#define AFEC_OVER_OVRE11 (0x1u << 11) /**< \brief (AFEC_OVER) Overrun Error 11 */
#define AFEC_OVER_OVRE12 (0x1u << 12) /**< \brief (AFEC_OVER) Overrun Error 12 */
#define AFEC_OVER_OVRE13 (0x1u << 13) /**< \brief (AFEC_OVER) Overrun Error 13 */
#define AFEC_OVER_OVRE14 (0x1u << 14) /**< \brief (AFEC_OVER) Overrun Error 14 */
#define AFEC_OVER_OVRE15 (0x1u << 15) /**< \brief (AFEC_OVER) Overrun Error 15 */
/* -------- AFEC_CWR : (AFEC Offset: 0x50) Compare Window Register -------- */
#define AFEC_CWR_LOWTHRES_Pos 0
#define AFEC_CWR_LOWTHRES_Msk (0xfffu << AFEC_CWR_LOWTHRES_Pos) /**< \brief (AFEC_CWR) Low Threshold */
#define AFEC_CWR_LOWTHRES(value) ((AFEC_CWR_LOWTHRES_Msk & ((value) << AFEC_CWR_LOWTHRES_Pos)))
#define AFEC_CWR_HIGHTHRES_Pos 16
#define AFEC_CWR_HIGHTHRES_Msk (0xfffu << AFEC_CWR_HIGHTHRES_Pos) /**< \brief (AFEC_CWR) High Threshold */
#define AFEC_CWR_HIGHTHRES(value) ((AFEC_CWR_HIGHTHRES_Msk & ((value) << AFEC_CWR_HIGHTHRES_Pos)))
/* -------- AFEC_CGR : (AFEC Offset: 0x54) Channel Gain Register -------- */
#define AFEC_CGR_GAIN0_Pos 0
#define AFEC_CGR_GAIN0_Msk (0x3u << AFEC_CGR_GAIN0_Pos) /**< \brief (AFEC_CGR) Gain for channel 0 */
#define AFEC_CGR_GAIN0(value) ((AFEC_CGR_GAIN0_Msk & ((value) << AFEC_CGR_GAIN0_Pos)))
#define AFEC_CGR_GAIN1_Pos 2
#define AFEC_CGR_GAIN1_Msk (0x3u << AFEC_CGR_GAIN1_Pos) /**< \brief (AFEC_CGR) Gain for channel 1 */
#define AFEC_CGR_GAIN1(value) ((AFEC_CGR_GAIN1_Msk & ((value) << AFEC_CGR_GAIN1_Pos)))
#define AFEC_CGR_GAIN2_Pos 4
#define AFEC_CGR_GAIN2_Msk (0x3u << AFEC_CGR_GAIN2_Pos) /**< \brief (AFEC_CGR) Gain for channel 2 */
#define AFEC_CGR_GAIN2(value) ((AFEC_CGR_GAIN2_Msk & ((value) << AFEC_CGR_GAIN2_Pos)))
#define AFEC_CGR_GAIN3_Pos 6
#define AFEC_CGR_GAIN3_Msk (0x3u << AFEC_CGR_GAIN3_Pos) /**< \brief (AFEC_CGR) Gain for channel 3 */
#define AFEC_CGR_GAIN3(value) ((AFEC_CGR_GAIN3_Msk & ((value) << AFEC_CGR_GAIN3_Pos)))
#define AFEC_CGR_GAIN4_Pos 8
#define AFEC_CGR_GAIN4_Msk (0x3u << AFEC_CGR_GAIN4_Pos) /**< \brief (AFEC_CGR) Gain for channel 4 */
#define AFEC_CGR_GAIN4(value) ((AFEC_CGR_GAIN4_Msk & ((value) << AFEC_CGR_GAIN4_Pos)))
#define AFEC_CGR_GAIN5_Pos 10
#define AFEC_CGR_GAIN5_Msk (0x3u << AFEC_CGR_GAIN5_Pos) /**< \brief (AFEC_CGR) Gain for channel 5 */
#define AFEC_CGR_GAIN5(value) ((AFEC_CGR_GAIN5_Msk & ((value) << AFEC_CGR_GAIN5_Pos)))
#define AFEC_CGR_GAIN6_Pos 12
#define AFEC_CGR_GAIN6_Msk (0x3u << AFEC_CGR_GAIN6_Pos) /**< \brief (AFEC_CGR) Gain for channel 6 */
#define AFEC_CGR_GAIN6(value) ((AFEC_CGR_GAIN6_Msk & ((value) << AFEC_CGR_GAIN6_Pos)))
#define AFEC_CGR_GAIN7_Pos 14
#define AFEC_CGR_GAIN7_Msk (0x3u << AFEC_CGR_GAIN7_Pos) /**< \brief (AFEC_CGR) Gain for channel 7 */
#define AFEC_CGR_GAIN7(value) ((AFEC_CGR_GAIN7_Msk & ((value) << AFEC_CGR_GAIN7_Pos)))
#define AFEC_CGR_GAIN8_Pos 16
#define AFEC_CGR_GAIN8_Msk (0x3u << AFEC_CGR_GAIN8_Pos) /**< \brief (AFEC_CGR) Gain for channel 8 */
#define AFEC_CGR_GAIN8(value) ((AFEC_CGR_GAIN8_Msk & ((value) << AFEC_CGR_GAIN8_Pos)))
#define AFEC_CGR_GAIN9_Pos 18
#define AFEC_CGR_GAIN9_Msk (0x3u << AFEC_CGR_GAIN9_Pos) /**< \brief (AFEC_CGR) Gain for channel 9 */
#define AFEC_CGR_GAIN9(value) ((AFEC_CGR_GAIN9_Msk & ((value) << AFEC_CGR_GAIN9_Pos)))
#define AFEC_CGR_GAIN10_Pos 20
#define AFEC_CGR_GAIN10_Msk (0x3u << AFEC_CGR_GAIN10_Pos) /**< \brief (AFEC_CGR) Gain for channel 10 */
#define AFEC_CGR_GAIN10(value) ((AFEC_CGR_GAIN10_Msk & ((value) << AFEC_CGR_GAIN10_Pos)))
#define AFEC_CGR_GAIN11_Pos 22
#define AFEC_CGR_GAIN11_Msk (0x3u << AFEC_CGR_GAIN11_Pos) /**< \brief (AFEC_CGR) Gain for channel 11 */
#define AFEC_CGR_GAIN11(value) ((AFEC_CGR_GAIN11_Msk & ((value) << AFEC_CGR_GAIN11_Pos)))
#define AFEC_CGR_GAIN12_Pos 24
#define AFEC_CGR_GAIN12_Msk (0x3u << AFEC_CGR_GAIN12_Pos) /**< \brief (AFEC_CGR) Gain for channel 12 */
#define AFEC_CGR_GAIN12(value) ((AFEC_CGR_GAIN12_Msk & ((value) << AFEC_CGR_GAIN12_Pos)))
#define AFEC_CGR_GAIN13_Pos 26
#define AFEC_CGR_GAIN13_Msk (0x3u << AFEC_CGR_GAIN13_Pos) /**< \brief (AFEC_CGR) Gain for channel 13 */
#define AFEC_CGR_GAIN13(value) ((AFEC_CGR_GAIN13_Msk & ((value) << AFEC_CGR_GAIN13_Pos)))
#define AFEC_CGR_GAIN14_Pos 28
#define AFEC_CGR_GAIN14_Msk (0x3u << AFEC_CGR_GAIN14_Pos) /**< \brief (AFEC_CGR) Gain for channel 14 */
#define AFEC_CGR_GAIN14(value) ((AFEC_CGR_GAIN14_Msk & ((value) << AFEC_CGR_GAIN14_Pos)))
#define AFEC_CGR_GAIN15_Pos 30
#define AFEC_CGR_GAIN15_Msk (0x3u << AFEC_CGR_GAIN15_Pos) /**< \brief (AFEC_CGR) Gain for channel 15 */
#define AFEC_CGR_GAIN15(value) ((AFEC_CGR_GAIN15_Msk & ((value) << AFEC_CGR_GAIN15_Pos)))
/* -------- AFEC_CDOR : (AFEC Offset: 0x5C) Channel Calibration DC Offset Register -------- */
#define AFEC_CDOR_OFF0 (0x1u << 0) /**< \brief (AFEC_CDOR) Offset for channel 0, used in automatic calibration procedure */
#define AFEC_CDOR_OFF1 (0x1u << 1) /**< \brief (AFEC_CDOR) Offset for channel 1, used in automatic calibration procedure */
#define AFEC_CDOR_OFF2 (0x1u << 2) /**< \brief (AFEC_CDOR) Offset for channel 2, used in automatic calibration procedure */
#define AFEC_CDOR_OFF3 (0x1u << 3) /**< \brief (AFEC_CDOR) Offset for channel 3, used in automatic calibration procedure */
#define AFEC_CDOR_OFF4 (0x1u << 4) /**< \brief (AFEC_CDOR) Offset for channel 4, used in automatic calibration procedure */
#define AFEC_CDOR_OFF5 (0x1u << 5) /**< \brief (AFEC_CDOR) Offset for channel 5, used in automatic calibration procedure */
#define AFEC_CDOR_OFF6 (0x1u << 6) /**< \brief (AFEC_CDOR) Offset for channel 6, used in automatic calibration procedure */
#define AFEC_CDOR_OFF7 (0x1u << 7) /**< \brief (AFEC_CDOR) Offset for channel 7, used in automatic calibration procedure */
#define AFEC_CDOR_OFF8 (0x1u << 8) /**< \brief (AFEC_CDOR) Offset for channel 8, used in automatic calibration procedure */
#define AFEC_CDOR_OFF9 (0x1u << 9) /**< \brief (AFEC_CDOR) Offset for channel 9, used in automatic calibration procedure */
#define AFEC_CDOR_OFF10 (0x1u << 10) /**< \brief (AFEC_CDOR) Offset for channel 10, used in automatic calibration procedure */
#define AFEC_CDOR_OFF11 (0x1u << 11) /**< \brief (AFEC_CDOR) Offset for channel 11, used in automatic calibration procedure */
#define AFEC_CDOR_OFF12 (0x1u << 12) /**< \brief (AFEC_CDOR) Offset for channel 12, used in automatic calibration procedure */
#define AFEC_CDOR_OFF13 (0x1u << 13) /**< \brief (AFEC_CDOR) Offset for channel 13, used in automatic calibration procedure */
#define AFEC_CDOR_OFF14 (0x1u << 14) /**< \brief (AFEC_CDOR) Offset for channel 14, used in automatic calibration procedure */
#define AFEC_CDOR_OFF15 (0x1u << 15) /**< \brief (AFEC_CDOR) Offset for channel 15, used in automatic calibration procedure */
/* -------- AFEC_DIFFR : (AFEC Offset: 0x60) Channel Differential Register -------- */
#define AFEC_DIFFR_DIFF0 (0x1u << 0) /**< \brief (AFEC_DIFFR) Differential inputs for channel 0 */
#define AFEC_DIFFR_DIFF1 (0x1u << 1) /**< \brief (AFEC_DIFFR) Differential inputs for channel 1 */
#define AFEC_DIFFR_DIFF2 (0x1u << 2) /**< \brief (AFEC_DIFFR) Differential inputs for channel 2 */
#define AFEC_DIFFR_DIFF3 (0x1u << 3) /**< \brief (AFEC_DIFFR) Differential inputs for channel 3 */
#define AFEC_DIFFR_DIFF4 (0x1u << 4) /**< \brief (AFEC_DIFFR) Differential inputs for channel 4 */
#define AFEC_DIFFR_DIFF5 (0x1u << 5) /**< \brief (AFEC_DIFFR) Differential inputs for channel 5 */
#define AFEC_DIFFR_DIFF6 (0x1u << 6) /**< \brief (AFEC_DIFFR) Differential inputs for channel 6 */
#define AFEC_DIFFR_DIFF7 (0x1u << 7) /**< \brief (AFEC_DIFFR) Differential inputs for channel 7 */
#define AFEC_DIFFR_DIFF8 (0x1u << 8) /**< \brief (AFEC_DIFFR) Differential inputs for channel 8 */
#define AFEC_DIFFR_DIFF9 (0x1u << 9) /**< \brief (AFEC_DIFFR) Differential inputs for channel 9 */
#define AFEC_DIFFR_DIFF10 (0x1u << 10) /**< \brief (AFEC_DIFFR) Differential inputs for channel 10 */
#define AFEC_DIFFR_DIFF11 (0x1u << 11) /**< \brief (AFEC_DIFFR) Differential inputs for channel 11 */
#define AFEC_DIFFR_DIFF12 (0x1u << 12) /**< \brief (AFEC_DIFFR) Differential inputs for channel 12 */
#define AFEC_DIFFR_DIFF13 (0x1u << 13) /**< \brief (AFEC_DIFFR) Differential inputs for channel 13 */
#define AFEC_DIFFR_DIFF14 (0x1u << 14) /**< \brief (AFEC_DIFFR) Differential inputs for channel 14 */
#define AFEC_DIFFR_DIFF15 (0x1u << 15) /**< \brief (AFEC_DIFFR) Differential inputs for channel 15 */
/* -------- AFEC_CSELR : (AFEC Offset: 0x64) Channel Register Selection -------- */
#define AFEC_CSELR_CSEL_Pos 0
#define AFEC_CSELR_CSEL_Msk (0xfu << AFEC_CSELR_CSEL_Pos) /**< \brief (AFEC_CSELR) Channel Selection */
#define AFEC_CSELR_CSEL(value) ((AFEC_CSELR_CSEL_Msk & ((value) << AFEC_CSELR_CSEL_Pos)))
/* -------- AFEC_CDR : (AFEC Offset: 0x68) Channel Data Register -------- */
#define AFEC_CDR_DATA_Pos 0
#define AFEC_CDR_DATA_Msk (0xfffu << AFEC_CDR_DATA_Pos) /**< \brief (AFEC_CDR) Converted Data */
/* -------- AFEC_COCR : (AFEC Offset: 0x6C) Channel Offset Compensation Register -------- */
#define AFEC_COCR_AOFF_Pos 0
#define AFEC_COCR_AOFF_Msk (0xfffu << AFEC_COCR_AOFF_Pos) /**< \brief (AFEC_COCR) Analog Offset */
#define AFEC_COCR_AOFF(value) ((AFEC_COCR_AOFF_Msk & ((value) << AFEC_COCR_AOFF_Pos)))
/* -------- AFEC_TEMPMR : (AFEC Offset: 0x70) Temperature Sensor Mode Register -------- */
#define AFEC_TEMPMR_RTCT (0x1u << 0) /**< \brief (AFEC_TEMPMR) Temperature Sensor RTC Trigger mode */
#define AFEC_TEMPMR_TEMPCMPMOD_Pos 4
#define AFEC_TEMPMR_TEMPCMPMOD_Msk (0x3u << AFEC_TEMPMR_TEMPCMPMOD_Pos) /**< \brief (AFEC_TEMPMR) Temperature Comparison Mode */
#define   AFEC_TEMPMR_TEMPCMPMOD_LOW (0x0u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is lower than the low threshold of the window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_HIGH (0x1u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is higher than the high threshold of the window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_IN (0x2u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is in the comparison window. */
#define   AFEC_TEMPMR_TEMPCMPMOD_OUT (0x3u << 4) /**< \brief (AFEC_TEMPMR) Generates an event when the converted data is out of the comparison window. */
/* -------- AFEC_TEMPCWR : (AFEC Offset: 0x74) Temperature Compare Window Register -------- */
#define AFEC_TEMPCWR_TLOWTHRES_Pos 0
#define AFEC_TEMPCWR_TLOWTHRES_Msk (0xffffu << AFEC_TEMPCWR_TLOWTHRES_Pos) /**< \brief (AFEC_TEMPCWR) Temperature Low Threshold */
#define AFEC_TEMPCWR_TLOWTHRES(value) ((AFEC_TEMPCWR_TLOWTHRES_Msk & ((value) << AFEC_TEMPCWR_TLOWTHRES_Pos)))
#define AFEC_TEMPCWR_THIGHTHRES_Pos 16
#define AFEC_TEMPCWR_THIGHTHRES_Msk (0xffffu << AFEC_TEMPCWR_THIGHTHRES_Pos) /**< \brief (AFEC_TEMPCWR) Temperature High Threshold */
#define AFEC_TEMPCWR_THIGHTHRES(value) ((AFEC_TEMPCWR_THIGHTHRES_Msk & ((value) << AFEC_TEMPCWR_THIGHTHRES_Pos)))
/* -------- AFEC_ACR : (AFEC Offset: 0x94) Analog Control Register -------- */
#define AFEC_ACR_IBCTL_Pos 8
#define AFEC_ACR_IBCTL_Msk (0x3u << AFEC_ACR_IBCTL_Pos) /**< \brief (AFEC_ACR) AFEC Bias Current Control */
#define AFEC_ACR_IBCTL(value) ((AFEC_ACR_IBCTL_Msk & ((value) << AFEC_ACR_IBCTL_Pos)))
/* -------- AFEC_WPMR : (AFEC Offset: 0xE4) Write Protect Mode Register -------- */
#define AFEC_WPMR_WPEN (0x1u << 0) /**< \brief (AFEC_WPMR) Write Protect Enable */
#define AFEC_WPMR_WPKEY_Pos 8
#define AFEC_WPMR_WPKEY_Msk (0xffffffu << AFEC_WPMR_WPKEY_Pos) /**< \brief (AFEC_WPMR) Write Protect KEY */
#define   AFEC_WPMR_WPKEY_ADC (0x414443u << 8) /**< \brief (AFEC_WPMR) Should be written at value 0x414443 ("ADC" in ASCII). Writing any other value in this field aborts the write operation of the WPEN bit. Always reads as 0. */
/* -------- AFEC_WPSR : (AFEC Offset: 0xE8) Write Protect Status Register -------- */
#define AFEC_WPSR_WPVS (0x1u << 0) /**< \brief (AFEC_WPSR) Write Protect Violation Status */
#define AFEC_WPSR_WPVSRC_Pos 8
#define AFEC_WPSR_WPVSRC_Msk (0xffffu << AFEC_WPSR_WPVSRC_Pos) /**< \brief (AFEC_WPSR) Write Protect Violation Source */
/* -------- AFEC_RPR : (AFEC Offset: 0x100) Receive Pointer Register -------- */
#define AFEC_RPR_RXPTR_Pos 0
#define AFEC_RPR_RXPTR_Msk (0xffffffffu << AFEC_RPR_RXPTR_Pos) /**< \brief (AFEC_RPR) Receive Pointer Register */
#define AFEC_RPR_RXPTR(value) ((AFEC_RPR_RXPTR_Msk & ((value) << AFEC_RPR_RXPTR_Pos)))
/* -------- AFEC_RCR : (AFEC Offset: 0x104) Receive Counter Register -------- */
#define AFEC_RCR_RXCTR_Pos 0
#define AFEC_RCR_RXCTR_Msk (0xffffu << AFEC_RCR_RXCTR_Pos) /**< \brief (AFEC_RCR) Receive Counter Register */
#define AFEC_RCR_RXCTR(value) ((AFEC_RCR_RXCTR_Msk & ((value) << AFEC_RCR_RXCTR_Pos)))
/* -------- AFEC_RNPR : (AFEC Offset: 0x110) Receive Next Pointer Register -------- */
#define AFEC_RNPR_RXNPTR_Pos 0
#define AFEC_RNPR_RXNPTR_Msk (0xffffffffu << AFEC_RNPR_RXNPTR_Pos) /**< \brief (AFEC_RNPR) Receive Next Pointer */
#define AFEC_RNPR_RXNPTR(value) ((AFEC_RNPR_RXNPTR_Msk & ((value) << AFEC_RNPR_RXNPTR_Pos)))
/* -------- AFEC_RNCR : (AFEC Offset: 0x114) Receive Next Counter Register -------- */
#define AFEC_RNCR_RXNCTR_Pos 0
#define AFEC_RNCR_RXNCTR_Msk (0xffffu << AFEC_RNCR_RXNCTR_Pos) /**< \brief (AFEC_RNCR) Receive Next Counter */
#define AFEC_RNCR_RXNCTR(value) ((AFEC_RNCR_RXNCTR_Msk & ((value) << AFEC_RNCR_RXNCTR_Pos)))
/* -------- AFEC_PTCR : (AFEC Offset: 0x120) Transfer Control Register -------- */
#define AFEC_PTCR_RXTEN (0x1u << 0) /**< \brief (AFEC_PTCR) Receiver Transfer Enable */
#define AFEC_PTCR_RXTDIS (0x1u << 1) /**< \brief (AFEC_PTCR) Receiver Transfer Disable */
#define AFEC_PTCR_TXTEN (0x1u << 8) /**< \brief (AFEC_PTCR) Transmitter Transfer Enable */
#define AFEC_PTCR_TXTDIS (0x1u << 9) /**< \brief (AFEC_PTCR) Transmitter Transfer Disable */
/* -------- AFEC_PTSR : (AFEC Offset: 0x124) Transfer Status Register -------- */
#define AFEC_PTSR_RXTEN (0x1u << 0) /**< \brief (AFEC_PTSR) Receiver Transfer Enable */
#define AFEC_PTSR_TXTEN (0x1u << 8) /**< \brief (AFEC_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM4E_AFEC_COMPONENT_ */
