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

#ifndef _SAM4E_MATRIX_COMPONENT_
#define _SAM4E_MATRIX_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Bus Matrix */
/* ============================================================================= */
/** \addtogroup SAM4E_MATRIX Bus Matrix */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Matrix hardware registers */
typedef struct {
  RwReg MATRIX_MCFG[7]; /**< \brief (Matrix Offset: 0x0000) Master Configuration Register */
  RoReg Reserved1[9];
  RwReg MATRIX_SCFG[6]; /**< \brief (Matrix Offset: 0x0040) Slave Configuration Register */
  RoReg Reserved2[10];
  RwReg MATRIX_PRAS0;   /**< \brief (Matrix Offset: 0x0080) Priority Register A for Slave 0 */
  RoReg Reserved3[1];
  RwReg MATRIX_PRAS1;   /**< \brief (Matrix Offset: 0x0088) Priority Register A for Slave 1 */
  RoReg Reserved4[1];
  RwReg MATRIX_PRAS2;   /**< \brief (Matrix Offset: 0x0090) Priority Register A for Slave 2 */
  RoReg Reserved5[1];
  RwReg MATRIX_PRAS3;   /**< \brief (Matrix Offset: 0x0098) Priority Register A for Slave 3 */
  RoReg Reserved6[1];
  RwReg MATRIX_PRAS4;   /**< \brief (Matrix Offset: 0x00A0) Priority Register A for Slave 4 */
  RoReg Reserved7[1];
  RwReg MATRIX_PRAS5;   /**< \brief (Matrix Offset: 0x00A8) Priority Register A for Slave 5 */
  RoReg Reserved8[1];
  RoReg Reserved9[20];
  RwReg MATRIX_MRCR;    /**< \brief (Matrix Offset: 0x0100) Master Remap Control Register */
  RoReg Reserved10[4];
  RwReg CCFG_SYSIO;     /**< \brief (Matrix Offset: 0x0114) System I/O Configuration Register */
  RoReg Reserved11[3];
  RwReg CCFG_SMCNFCS;   /**< \brief (Matrix Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register */
  RoReg Reserved12[47];
  RwReg MATRIX_WPMR;    /**< \brief (Matrix Offset: 0x01E4) Write Protect Mode Register */
  RoReg MATRIX_WPSR;    /**< \brief (Matrix Offset: 0x01E8) Write Protect Status Register */
} Matrix;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- MATRIX_MCFG[7] : (MATRIX Offset: 0x0000) Master Configuration Register -------- */
#define MATRIX_MCFG_ULBT_Pos 0
#define MATRIX_MCFG_ULBT_Msk (0x7u << MATRIX_MCFG_ULBT_Pos) /**< \brief (MATRIX_MCFG[7]) Undefined Length Burst Type */
#define MATRIX_MCFG_ULBT(value) ((MATRIX_MCFG_ULBT_Msk & ((value) << MATRIX_MCFG_ULBT_Pos)))
/* -------- MATRIX_SCFG[6] : (MATRIX Offset: 0x0040) Slave Configuration Register -------- */
#define MATRIX_SCFG_SLOT_CYCLE_Pos 0
#define MATRIX_SCFG_SLOT_CYCLE_Msk (0x1ffu << MATRIX_SCFG_SLOT_CYCLE_Pos) /**< \brief (MATRIX_SCFG[6]) Maximum Bus Grant Duration for Masters */
#define MATRIX_SCFG_SLOT_CYCLE(value) ((MATRIX_SCFG_SLOT_CYCLE_Msk & ((value) << MATRIX_SCFG_SLOT_CYCLE_Pos)))
#define MATRIX_SCFG_DEFMSTR_TYPE_Pos 16
#define MATRIX_SCFG_DEFMSTR_TYPE_Msk (0x3u << MATRIX_SCFG_DEFMSTR_TYPE_Pos) /**< \brief (MATRIX_SCFG[6]) Default Master Type */
#define MATRIX_SCFG_DEFMSTR_TYPE(value) ((MATRIX_SCFG_DEFMSTR_TYPE_Msk & ((value) << MATRIX_SCFG_DEFMSTR_TYPE_Pos)))
#define MATRIX_SCFG_FIXED_DEFMSTR_Pos 18
#define MATRIX_SCFG_FIXED_DEFMSTR_Msk (0xfu << MATRIX_SCFG_FIXED_DEFMSTR_Pos) /**< \brief (MATRIX_SCFG[6]) Fixed Default Master */
#define MATRIX_SCFG_FIXED_DEFMSTR(value) ((MATRIX_SCFG_FIXED_DEFMSTR_Msk & ((value) << MATRIX_SCFG_FIXED_DEFMSTR_Pos)))
/* -------- MATRIX_PRAS0 : (MATRIX Offset: 0x0080) Priority Register A for Slave 0 -------- */
#define MATRIX_PRAS0_M0PR_Pos 0
#define MATRIX_PRAS0_M0PR_Msk (0x3u << MATRIX_PRAS0_M0PR_Pos) /**< \brief (MATRIX_PRAS0) Master 0 Priority */
#define MATRIX_PRAS0_M0PR(value) ((MATRIX_PRAS0_M0PR_Msk & ((value) << MATRIX_PRAS0_M0PR_Pos)))
#define MATRIX_PRAS0_M1PR_Pos 4
#define MATRIX_PRAS0_M1PR_Msk (0x3u << MATRIX_PRAS0_M1PR_Pos) /**< \brief (MATRIX_PRAS0) Master 1 Priority */
#define MATRIX_PRAS0_M1PR(value) ((MATRIX_PRAS0_M1PR_Msk & ((value) << MATRIX_PRAS0_M1PR_Pos)))
#define MATRIX_PRAS0_M2PR_Pos 8
#define MATRIX_PRAS0_M2PR_Msk (0x3u << MATRIX_PRAS0_M2PR_Pos) /**< \brief (MATRIX_PRAS0) Master 2 Priority */
#define MATRIX_PRAS0_M2PR(value) ((MATRIX_PRAS0_M2PR_Msk & ((value) << MATRIX_PRAS0_M2PR_Pos)))
#define MATRIX_PRAS0_M3PR_Pos 12
#define MATRIX_PRAS0_M3PR_Msk (0x3u << MATRIX_PRAS0_M3PR_Pos) /**< \brief (MATRIX_PRAS0) Master 3 Priority */
#define MATRIX_PRAS0_M3PR(value) ((MATRIX_PRAS0_M3PR_Msk & ((value) << MATRIX_PRAS0_M3PR_Pos)))
#define MATRIX_PRAS0_M4PR_Pos 16
#define MATRIX_PRAS0_M4PR_Msk (0x3u << MATRIX_PRAS0_M4PR_Pos) /**< \brief (MATRIX_PRAS0) Master 4 Priority */
#define MATRIX_PRAS0_M4PR(value) ((MATRIX_PRAS0_M4PR_Msk & ((value) << MATRIX_PRAS0_M4PR_Pos)))
#define MATRIX_PRAS0_M5PR_Pos 20
#define MATRIX_PRAS0_M5PR_Msk (0x3u << MATRIX_PRAS0_M5PR_Pos) /**< \brief (MATRIX_PRAS0) Master 5 Priority */
#define MATRIX_PRAS0_M5PR(value) ((MATRIX_PRAS0_M5PR_Msk & ((value) << MATRIX_PRAS0_M5PR_Pos)))
#define MATRIX_PRAS0_M6PR_Pos 24
#define MATRIX_PRAS0_M6PR_Msk (0x3u << MATRIX_PRAS0_M6PR_Pos) /**< \brief (MATRIX_PRAS0) Master 6 Priority */
#define MATRIX_PRAS0_M6PR(value) ((MATRIX_PRAS0_M6PR_Msk & ((value) << MATRIX_PRAS0_M6PR_Pos)))
/* -------- MATRIX_PRAS1 : (MATRIX Offset: 0x0088) Priority Register A for Slave 1 -------- */
#define MATRIX_PRAS1_M0PR_Pos 0
#define MATRIX_PRAS1_M0PR_Msk (0x3u << MATRIX_PRAS1_M0PR_Pos) /**< \brief (MATRIX_PRAS1) Master 0 Priority */
#define MATRIX_PRAS1_M0PR(value) ((MATRIX_PRAS1_M0PR_Msk & ((value) << MATRIX_PRAS1_M0PR_Pos)))
#define MATRIX_PRAS1_M1PR_Pos 4
#define MATRIX_PRAS1_M1PR_Msk (0x3u << MATRIX_PRAS1_M1PR_Pos) /**< \brief (MATRIX_PRAS1) Master 1 Priority */
#define MATRIX_PRAS1_M1PR(value) ((MATRIX_PRAS1_M1PR_Msk & ((value) << MATRIX_PRAS1_M1PR_Pos)))
#define MATRIX_PRAS1_M2PR_Pos 8
#define MATRIX_PRAS1_M2PR_Msk (0x3u << MATRIX_PRAS1_M2PR_Pos) /**< \brief (MATRIX_PRAS1) Master 2 Priority */
#define MATRIX_PRAS1_M2PR(value) ((MATRIX_PRAS1_M2PR_Msk & ((value) << MATRIX_PRAS1_M2PR_Pos)))
#define MATRIX_PRAS1_M3PR_Pos 12
#define MATRIX_PRAS1_M3PR_Msk (0x3u << MATRIX_PRAS1_M3PR_Pos) /**< \brief (MATRIX_PRAS1) Master 3 Priority */
#define MATRIX_PRAS1_M3PR(value) ((MATRIX_PRAS1_M3PR_Msk & ((value) << MATRIX_PRAS1_M3PR_Pos)))
#define MATRIX_PRAS1_M4PR_Pos 16
#define MATRIX_PRAS1_M4PR_Msk (0x3u << MATRIX_PRAS1_M4PR_Pos) /**< \brief (MATRIX_PRAS1) Master 4 Priority */
#define MATRIX_PRAS1_M4PR(value) ((MATRIX_PRAS1_M4PR_Msk & ((value) << MATRIX_PRAS1_M4PR_Pos)))
#define MATRIX_PRAS1_M5PR_Pos 20
#define MATRIX_PRAS1_M5PR_Msk (0x3u << MATRIX_PRAS1_M5PR_Pos) /**< \brief (MATRIX_PRAS1) Master 5 Priority */
#define MATRIX_PRAS1_M5PR(value) ((MATRIX_PRAS1_M5PR_Msk & ((value) << MATRIX_PRAS1_M5PR_Pos)))
#define MATRIX_PRAS1_M6PR_Pos 24
#define MATRIX_PRAS1_M6PR_Msk (0x3u << MATRIX_PRAS1_M6PR_Pos) /**< \brief (MATRIX_PRAS1) Master 6 Priority */
#define MATRIX_PRAS1_M6PR(value) ((MATRIX_PRAS1_M6PR_Msk & ((value) << MATRIX_PRAS1_M6PR_Pos)))
/* -------- MATRIX_PRAS2 : (MATRIX Offset: 0x0090) Priority Register A for Slave 2 -------- */
#define MATRIX_PRAS2_M0PR_Pos 0
#define MATRIX_PRAS2_M0PR_Msk (0x3u << MATRIX_PRAS2_M0PR_Pos) /**< \brief (MATRIX_PRAS2) Master 0 Priority */
#define MATRIX_PRAS2_M0PR(value) ((MATRIX_PRAS2_M0PR_Msk & ((value) << MATRIX_PRAS2_M0PR_Pos)))
#define MATRIX_PRAS2_M1PR_Pos 4
#define MATRIX_PRAS2_M1PR_Msk (0x3u << MATRIX_PRAS2_M1PR_Pos) /**< \brief (MATRIX_PRAS2) Master 1 Priority */
#define MATRIX_PRAS2_M1PR(value) ((MATRIX_PRAS2_M1PR_Msk & ((value) << MATRIX_PRAS2_M1PR_Pos)))
#define MATRIX_PRAS2_M2PR_Pos 8
#define MATRIX_PRAS2_M2PR_Msk (0x3u << MATRIX_PRAS2_M2PR_Pos) /**< \brief (MATRIX_PRAS2) Master 2 Priority */
#define MATRIX_PRAS2_M2PR(value) ((MATRIX_PRAS2_M2PR_Msk & ((value) << MATRIX_PRAS2_M2PR_Pos)))
#define MATRIX_PRAS2_M3PR_Pos 12
#define MATRIX_PRAS2_M3PR_Msk (0x3u << MATRIX_PRAS2_M3PR_Pos) /**< \brief (MATRIX_PRAS2) Master 3 Priority */
#define MATRIX_PRAS2_M3PR(value) ((MATRIX_PRAS2_M3PR_Msk & ((value) << MATRIX_PRAS2_M3PR_Pos)))
#define MATRIX_PRAS2_M4PR_Pos 16
#define MATRIX_PRAS2_M4PR_Msk (0x3u << MATRIX_PRAS2_M4PR_Pos) /**< \brief (MATRIX_PRAS2) Master 4 Priority */
#define MATRIX_PRAS2_M4PR(value) ((MATRIX_PRAS2_M4PR_Msk & ((value) << MATRIX_PRAS2_M4PR_Pos)))
#define MATRIX_PRAS2_M5PR_Pos 20
#define MATRIX_PRAS2_M5PR_Msk (0x3u << MATRIX_PRAS2_M5PR_Pos) /**< \brief (MATRIX_PRAS2) Master 5 Priority */
#define MATRIX_PRAS2_M5PR(value) ((MATRIX_PRAS2_M5PR_Msk & ((value) << MATRIX_PRAS2_M5PR_Pos)))
#define MATRIX_PRAS2_M6PR_Pos 24
#define MATRIX_PRAS2_M6PR_Msk (0x3u << MATRIX_PRAS2_M6PR_Pos) /**< \brief (MATRIX_PRAS2) Master 6 Priority */
#define MATRIX_PRAS2_M6PR(value) ((MATRIX_PRAS2_M6PR_Msk & ((value) << MATRIX_PRAS2_M6PR_Pos)))
/* -------- MATRIX_PRAS3 : (MATRIX Offset: 0x0098) Priority Register A for Slave 3 -------- */
#define MATRIX_PRAS3_M0PR_Pos 0
#define MATRIX_PRAS3_M0PR_Msk (0x3u << MATRIX_PRAS3_M0PR_Pos) /**< \brief (MATRIX_PRAS3) Master 0 Priority */
#define MATRIX_PRAS3_M0PR(value) ((MATRIX_PRAS3_M0PR_Msk & ((value) << MATRIX_PRAS3_M0PR_Pos)))
#define MATRIX_PRAS3_M1PR_Pos 4
#define MATRIX_PRAS3_M1PR_Msk (0x3u << MATRIX_PRAS3_M1PR_Pos) /**< \brief (MATRIX_PRAS3) Master 1 Priority */
#define MATRIX_PRAS3_M1PR(value) ((MATRIX_PRAS3_M1PR_Msk & ((value) << MATRIX_PRAS3_M1PR_Pos)))
#define MATRIX_PRAS3_M2PR_Pos 8
#define MATRIX_PRAS3_M2PR_Msk (0x3u << MATRIX_PRAS3_M2PR_Pos) /**< \brief (MATRIX_PRAS3) Master 2 Priority */
#define MATRIX_PRAS3_M2PR(value) ((MATRIX_PRAS3_M2PR_Msk & ((value) << MATRIX_PRAS3_M2PR_Pos)))
#define MATRIX_PRAS3_M3PR_Pos 12
#define MATRIX_PRAS3_M3PR_Msk (0x3u << MATRIX_PRAS3_M3PR_Pos) /**< \brief (MATRIX_PRAS3) Master 3 Priority */
#define MATRIX_PRAS3_M3PR(value) ((MATRIX_PRAS3_M3PR_Msk & ((value) << MATRIX_PRAS3_M3PR_Pos)))
#define MATRIX_PRAS3_M4PR_Pos 16
#define MATRIX_PRAS3_M4PR_Msk (0x3u << MATRIX_PRAS3_M4PR_Pos) /**< \brief (MATRIX_PRAS3) Master 4 Priority */
#define MATRIX_PRAS3_M4PR(value) ((MATRIX_PRAS3_M4PR_Msk & ((value) << MATRIX_PRAS3_M4PR_Pos)))
#define MATRIX_PRAS3_M5PR_Pos 20
#define MATRIX_PRAS3_M5PR_Msk (0x3u << MATRIX_PRAS3_M5PR_Pos) /**< \brief (MATRIX_PRAS3) Master 5 Priority */
#define MATRIX_PRAS3_M5PR(value) ((MATRIX_PRAS3_M5PR_Msk & ((value) << MATRIX_PRAS3_M5PR_Pos)))
#define MATRIX_PRAS3_M6PR_Pos 24
#define MATRIX_PRAS3_M6PR_Msk (0x3u << MATRIX_PRAS3_M6PR_Pos) /**< \brief (MATRIX_PRAS3) Master 6 Priority */
#define MATRIX_PRAS3_M6PR(value) ((MATRIX_PRAS3_M6PR_Msk & ((value) << MATRIX_PRAS3_M6PR_Pos)))
/* -------- MATRIX_PRAS4 : (MATRIX Offset: 0x00A0) Priority Register A for Slave 4 -------- */
#define MATRIX_PRAS4_M0PR_Pos 0
#define MATRIX_PRAS4_M0PR_Msk (0x3u << MATRIX_PRAS4_M0PR_Pos) /**< \brief (MATRIX_PRAS4) Master 0 Priority */
#define MATRIX_PRAS4_M0PR(value) ((MATRIX_PRAS4_M0PR_Msk & ((value) << MATRIX_PRAS4_M0PR_Pos)))
#define MATRIX_PRAS4_M1PR_Pos 4
#define MATRIX_PRAS4_M1PR_Msk (0x3u << MATRIX_PRAS4_M1PR_Pos) /**< \brief (MATRIX_PRAS4) Master 1 Priority */
#define MATRIX_PRAS4_M1PR(value) ((MATRIX_PRAS4_M1PR_Msk & ((value) << MATRIX_PRAS4_M1PR_Pos)))
#define MATRIX_PRAS4_M2PR_Pos 8
#define MATRIX_PRAS4_M2PR_Msk (0x3u << MATRIX_PRAS4_M2PR_Pos) /**< \brief (MATRIX_PRAS4) Master 2 Priority */
#define MATRIX_PRAS4_M2PR(value) ((MATRIX_PRAS4_M2PR_Msk & ((value) << MATRIX_PRAS4_M2PR_Pos)))
#define MATRIX_PRAS4_M3PR_Pos 12
#define MATRIX_PRAS4_M3PR_Msk (0x3u << MATRIX_PRAS4_M3PR_Pos) /**< \brief (MATRIX_PRAS4) Master 3 Priority */
#define MATRIX_PRAS4_M3PR(value) ((MATRIX_PRAS4_M3PR_Msk & ((value) << MATRIX_PRAS4_M3PR_Pos)))
#define MATRIX_PRAS4_M4PR_Pos 16
#define MATRIX_PRAS4_M4PR_Msk (0x3u << MATRIX_PRAS4_M4PR_Pos) /**< \brief (MATRIX_PRAS4) Master 4 Priority */
#define MATRIX_PRAS4_M4PR(value) ((MATRIX_PRAS4_M4PR_Msk & ((value) << MATRIX_PRAS4_M4PR_Pos)))
#define MATRIX_PRAS4_M5PR_Pos 20
#define MATRIX_PRAS4_M5PR_Msk (0x3u << MATRIX_PRAS4_M5PR_Pos) /**< \brief (MATRIX_PRAS4) Master 5 Priority */
#define MATRIX_PRAS4_M5PR(value) ((MATRIX_PRAS4_M5PR_Msk & ((value) << MATRIX_PRAS4_M5PR_Pos)))
#define MATRIX_PRAS4_M6PR_Pos 24
#define MATRIX_PRAS4_M6PR_Msk (0x3u << MATRIX_PRAS4_M6PR_Pos) /**< \brief (MATRIX_PRAS4) Master 6 Priority */
#define MATRIX_PRAS4_M6PR(value) ((MATRIX_PRAS4_M6PR_Msk & ((value) << MATRIX_PRAS4_M6PR_Pos)))
/* -------- MATRIX_PRAS5 : (MATRIX Offset: 0x00A8) Priority Register A for Slave 5 -------- */
#define MATRIX_PRAS5_M0PR_Pos 0
#define MATRIX_PRAS5_M0PR_Msk (0x3u << MATRIX_PRAS5_M0PR_Pos) /**< \brief (MATRIX_PRAS5) Master 0 Priority */
#define MATRIX_PRAS5_M0PR(value) ((MATRIX_PRAS5_M0PR_Msk & ((value) << MATRIX_PRAS5_M0PR_Pos)))
#define MATRIX_PRAS5_M1PR_Pos 4
#define MATRIX_PRAS5_M1PR_Msk (0x3u << MATRIX_PRAS5_M1PR_Pos) /**< \brief (MATRIX_PRAS5) Master 1 Priority */
#define MATRIX_PRAS5_M1PR(value) ((MATRIX_PRAS5_M1PR_Msk & ((value) << MATRIX_PRAS5_M1PR_Pos)))
#define MATRIX_PRAS5_M2PR_Pos 8
#define MATRIX_PRAS5_M2PR_Msk (0x3u << MATRIX_PRAS5_M2PR_Pos) /**< \brief (MATRIX_PRAS5) Master 2 Priority */
#define MATRIX_PRAS5_M2PR(value) ((MATRIX_PRAS5_M2PR_Msk & ((value) << MATRIX_PRAS5_M2PR_Pos)))
#define MATRIX_PRAS5_M3PR_Pos 12
#define MATRIX_PRAS5_M3PR_Msk (0x3u << MATRIX_PRAS5_M3PR_Pos) /**< \brief (MATRIX_PRAS5) Master 3 Priority */
#define MATRIX_PRAS5_M3PR(value) ((MATRIX_PRAS5_M3PR_Msk & ((value) << MATRIX_PRAS5_M3PR_Pos)))
#define MATRIX_PRAS5_M4PR_Pos 16
#define MATRIX_PRAS5_M4PR_Msk (0x3u << MATRIX_PRAS5_M4PR_Pos) /**< \brief (MATRIX_PRAS5) Master 4 Priority */
#define MATRIX_PRAS5_M4PR(value) ((MATRIX_PRAS5_M4PR_Msk & ((value) << MATRIX_PRAS5_M4PR_Pos)))
#define MATRIX_PRAS5_M5PR_Pos 20
#define MATRIX_PRAS5_M5PR_Msk (0x3u << MATRIX_PRAS5_M5PR_Pos) /**< \brief (MATRIX_PRAS5) Master 5 Priority */
#define MATRIX_PRAS5_M5PR(value) ((MATRIX_PRAS5_M5PR_Msk & ((value) << MATRIX_PRAS5_M5PR_Pos)))
#define MATRIX_PRAS5_M6PR_Pos 24
#define MATRIX_PRAS5_M6PR_Msk (0x3u << MATRIX_PRAS5_M6PR_Pos) /**< \brief (MATRIX_PRAS5) Master 6 Priority */
#define MATRIX_PRAS5_M6PR(value) ((MATRIX_PRAS5_M6PR_Msk & ((value) << MATRIX_PRAS5_M6PR_Pos)))
/* -------- MATRIX_MRCR : (MATRIX Offset: 0x0100) Master Remap Control Register -------- */
#define MATRIX_MRCR_RCB0 (0x1u << 0) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 0 */
#define MATRIX_MRCR_RCB1 (0x1u << 1) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 1 */
#define MATRIX_MRCR_RCB2 (0x1u << 2) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 2 */
#define MATRIX_MRCR_RCB3 (0x1u << 3) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 3 */
#define MATRIX_MRCR_RCB4 (0x1u << 4) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 4 */
#define MATRIX_MRCR_RCB5 (0x1u << 5) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 5 */
#define MATRIX_MRCR_RCB6 (0x1u << 6) /**< \brief (MATRIX_MRCR) Remap Command Bit for Master 6 */
/* -------- CCFG_SYSIO : (MATRIX Offset: 0x0114) System I/O Configuration Register -------- */
#define CCFG_SYSIO_SYSIO4 (0x1u << 4) /**< \brief (CCFG_SYSIO) PB4 or TDI Assignment */
#define CCFG_SYSIO_SYSIO5 (0x1u << 5) /**< \brief (CCFG_SYSIO) PB5 or TDO/TRACESWO Assignment */
#define CCFG_SYSIO_SYSIO6 (0x1u << 6) /**< \brief (CCFG_SYSIO) PB6 or TMS/SWDIO Assignment */
#define CCFG_SYSIO_SYSIO7 (0x1u << 7) /**< \brief (CCFG_SYSIO) PB7 or TCK/SWCLK Assignment */
#define CCFG_SYSIO_SYSIO10 (0x1u << 10) /**< \brief (CCFG_SYSIO) PB10 or DDM Assignment */
#define CCFG_SYSIO_SYSIO11 (0x1u << 11) /**< \brief (CCFG_SYSIO) PB11 or DDP Assignment */
#define CCFG_SYSIO_SYSIO12 (0x1u << 12) /**< \brief (CCFG_SYSIO) PB12 or ERASE Assignment */
/* -------- CCFG_SMCNFCS : (MATRIX Offset: 0x0124) SMC NAND Flash Chip Select Configuration Register -------- */
#define CCFG_SMCNFCS_SMC_NFCS0 (0x1u << 0) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 0 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS1 (0x1u << 1) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 1 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS2 (0x1u << 2) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 2 Assignment */
#define CCFG_SMCNFCS_SMC_NFCS3 (0x1u << 3) /**< \brief (CCFG_SMCNFCS) SMC NAND Flash Chip Select 3 Assignment */
/* -------- MATRIX_WPMR : (MATRIX Offset: 0x01E4) Write Protect Mode Register -------- */
#define MATRIX_WPMR_WPEN (0x1u << 0) /**< \brief (MATRIX_WPMR) Write Protect Enable */
#define MATRIX_WPMR_WPKEY_Pos 8
#define MATRIX_WPMR_WPKEY_Msk (0xffffffu << MATRIX_WPMR_WPKEY_Pos) /**< \brief (MATRIX_WPMR) Write Protect KEY (Write-only) */
#define MATRIX_WPMR_WPKEY(value) ((MATRIX_WPMR_WPKEY_Msk & ((value) << MATRIX_WPMR_WPKEY_Pos)))
/* -------- MATRIX_WPSR : (MATRIX Offset: 0x01E8) Write Protect Status Register -------- */
#define MATRIX_WPSR_WPVS (0x1u << 0) /**< \brief (MATRIX_WPSR) Write Protect Violation Status */
#define MATRIX_WPSR_WPVSRC_Pos 8
#define MATRIX_WPSR_WPVSRC_Msk (0xffffu << MATRIX_WPSR_WPVSRC_Pos) /**< \brief (MATRIX_WPSR) Write Protect Violation Source */

/*@}*/


#endif /* _SAM4E_MATRIX_COMPONENT_ */
