/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA EBI driver SRAM example source.
 *
 *      This file contains an example application that demonstrates the EBI
 *      driver. It shows how to configure the EBI for 3-port SRAM operation.
 *      The example fills a data pattern in the memory, copies back
 *      and compares the copied block.
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
 * $Revision: 595 $
 * $Date: 2007-03-22 14:43:03 +0100 (to, 22 mar 2007) $  \n
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
#include "avr_compiler.h"
#include "ebi_driver.h"

/*! \brief Address where we want SRAM to be accessed.
 *
 *  \note This address is not on an address space boundry, and it will truncated
 *        down to the closest address space boundry. In this example where the
 *        address space is 128K, the base address will be 0x0000 thus loosing
 *        the first 0x4000 (16K) of this address space.
 */
#define SRAM_ADDR 0x4000


/*! \brief Macro to access SRAM. */
#define SRAM(addr) ((uint8_t *) SRAM_ADDR)[addr]

/*! \brief Number of test bytes to store to SRAM */
#define VECTOR_SIZE 1000


/*! \brief Test function for EBI with SRAM.
 *
 *  Hardware setup for 3-port SRAM interface:
 *
 *  PORTK[7:0] - A[7:0]/A[15:8]/A[23:16] (BYTE 2 and 3 connected through ALE1 and ALE2)
 *
 *  PORTJ[7:0] - D[7:0]
 *
 *  PORTH[7:0] - {CS3,CS2,CS1,CS0,ALE2,ALE1,RE,WE} (CS0 used for SRAM)
 *
 *  In this example a 128KB SRAM is used, which does not require any wait states
 *  at 2 MHz. Pull-up resistors should be connected to the Chip Select line to
 *  prevent garbage from being written to the SRAM during start-up, while the
 *  control lines are still in an unknown state.
 *
 *  The setup is tested by writing a set of data to the SRAM. The data is then
 *  read back and verified. At the end, the program will be stuck in one of
 *  two infinite loops, dependent on whether the test passed or not.
 */
int main( void )
{
	/* Flag indicating correct data transfer to and from SRAM */
	bool equal = true;

	/* Set signals which are active-low to high value */
	PORTH.OUT = 0xFF;

	/* Configure bus pins as outputs(except for data lines). */
	PORTH.DIR = 0xFF;
	PORTK.DIR = 0xFF;

	/* Initialize EBI. */
	EBI_Enable( EBI_SDDATAW_8BIT_gc,
	            EBI_LPCMODE_ALE1_gc,
	            EBI_SRMODE_ALE12_gc,
	            EBI_IFMODE_3PORT_gc );

	/* Initialize SRAM */
	EBI_EnableSRAM( &EBI.CS0,               /* Chip Select 0. */
	                EBI_CS_ASPACE_128KB_gc, /* 128 KB Address space. */
	                (void *) SRAM_ADDR,     /* Base address. */
	                0 );                    /* 0 wait states. */

	/* Fill SRAM with data. */
	for (uint16_t i = 0; i < VECTOR_SIZE; i++) {
		SRAM(i) =  (uint8_t) i & 0xFF;
	}

	/* Read back from SRAM and verify */
	for (uint16_t i = 0; i < VECTOR_SIZE; i++) {
		if (SRAM(i) != ((uint8_t) i & 0xFF)){
			equal = false;
			break;
		}
	}

	/* Report success or failure. */

	if (equal) {
		while(true) {
		/* Breakpoint for success. */
			nop();
		}
	}
	else {
		while(true) {
		/* Breakpoint for failure. */
			nop();
		}
	}
}
