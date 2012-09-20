/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA External Bus Interface driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA EBI driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA EBI module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1312: Using the XMEGA External Bus Interface
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 479 $
 * $Date: 2007-03-05 16:30:19 +0100 (ma, 05 mar 2007) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef EBI_DRIVER_H
#define EBI_DRIVER_H

#include "avr_compiler.h"

/* Definitions of macros. */


/*! \brief Disable EBI module.
 *
 *  This function disables the EBI module.
 *  This will abort any on-going operations, such as DMA transfers.
 */
#define EBI_Disable() (EBI.CTRL = EBI.CTRL & ~EBI_IFMODE_gm | EBI_IFMODE_DISABLED_gc)



/*! \brief Disable a Chip Select module.
 *
 *  This function disables one Chip Select submodule.
 *
 * \param  _chipSelect  The EBI_CS_t struct instance
 */
#define EBI_DisableChipSelect(_chipSelect) ( (_chipSelect)->CTRLA = (_chipSelect)->CTRLA & ~EBI_CS_MODE_gm | EBI_CS_MODE_DISABLED_gc )



/*! \brief Switch between normal SDRAM mode and "Load Mode Register" operation.
 *
 *  This function selects normal operation or Load Mode Register for SDRAM operation.
 *  SDRAM must be enabled first.
 *
 *  \param  _sdramMode  Selects SDRAM mode.
 */
#define EBI_SetSDRAMMode(_sdramMode) ( EBI.CS3.CTRLB = EBI.CS3.CTRLB & ~EBI_CS_SDMODE_gm | (_sdramMode) )



/*!  \brief Enable SDRAM Self-refresh mode.
 *
 *  This function enables self-refresh for SDRAM. Use this before entering
 *  sleep mode so that SDRAM device refreshes itself. SDRAM will be inaccessible
 *  while in self-refresh mode.
 */
#define EBI_EnableSelfRefresh() (EBI.CS3.CTRLB |= EBI_CS_SDSREN_bm)



/*! \brief Disable SDRAM Self-refresh mode.
 *
 *  This function disables self-refresh for SDRAM. Use this after leaving
 *  sleep mode so that SDRAM is accessible again.
 */
#define EBI_DisableSelfRefresh() (EBI.CS3.CTRLB &= ~EBI_CS_SDSREN_bm)



/* Prototype functions. Detailed description in source file.*/

void EBI_Enable( EBI_SDDATAW_t sdramDataWidth,
                 EBI_LPCMODE_t lpcMode,
                 EBI_SRMODE_t sramMode,
                 EBI_IFMODE_t interfaceMode );

void EBI_EnableSRAM( volatile EBI_CS_t * chipSelect,
                     EBI_CS_ASPACE_t addrSpace,
                     void * baseAddr,
                     uint8_t waitStateCycles );

void EBI_EnableLPC( volatile EBI_CS_t * chipSelect,
                    EBI_CS_ASPACE_t addrSpace,
                    void * baseAddr,
                    uint8_t waitStateCycles );

void EBI_EnableSDRAM( EBI_CS_ASPACE_t addrSpace,
                      void * baseAddr,
                      bool casLatency,
                      bool rowBits,
                      EBI_SDCOL_t columnBits,
                      EBI_MRDLY_t modeRegisterDelay,
                      EBI_ROWCYCDLY_t rowCycleDelay,
                      EBI_RPDLY_t rowToPrechargeDelay,
                      EBI_WRDLY_t writeRecoveryDelay,
                      EBI_ESRDLY_t exitSelfRefreshToActiveDelay,
                      EBI_ROWCOLDLY_t rowToColumnDelay,
                      uint16_t refreshPeriod,
                      uint16_t initializationDelay );

#endif
