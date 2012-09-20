/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DAC driver example source.
 *
 *      This file contains an example application that demonstrates the DAC
 *      driver. It shows how to set up the DAC in dual-channel mode with
 *      correct sample and refresh timing. The example outputs two triangle
 *      waves, phase shifted 180 degrees.
 *
 *      The sample and refresh timing settings are based on a CPU frequency
 *      of 2 MHz. Please refer to the module datasheet for more details on
 *      sample and refresh timing limits.
 *
 *      As this is a DAC example only, we do not use any Timer/counter to
 *      produce a specific waveform frequency. The code outputs the triangle
 *      wave with the highest frequency possible with this unoptimized C code.
 *
 * \par Application note:
 *      AVR1301: Using the XMEGA DAC
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
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
#include "dac_driver.h"


/*! \brief DAC example code.
 *
 *  This is an example code which output a triangle waveform on channel 0, and
 *  a 180 degree phase shift channel 1. The example code use a blocking while
 *  loop to ensure that the data register is empty before a new value is written.
 */
int main( void )
{
	uint16_t angle;

	/*  Given 2 MHz CPU speed, this is configured to 2µs separation time and
	 *  16µs refresh time.
	 */
	DAC_DualChannel_Enable( &DACA,
	                        DAC_REFSEL_AVCC_gc,
	                        false,
	                        DAC_CONINTVAL_4CLK_gc,
	                        DAC_REFRESH_32CLK_gc );

	/*  Use a 12 bit variable to output two triangle waves, separated by
	 *  180 degrees phase, as fast as possible.
	 */
	while (1) {
		for ( angle = 0; angle < 0x1000; ++angle ) {
			while ( DAC_Channel_DataEmpty( &DACA, CH0 ) == false ) {
				/* Blocking code waiting for empty register. */
			}
			DAC_Channel_Write( &DACA, angle, CH0 );
			while ( DAC_Channel_DataEmpty( &DACA, CH1 ) == false ) {
				/* Blocking code waiting for empty register. */
			}
			DAC_Channel_Write( &DACA, 0xFFF - angle, CH1 );
		}
		for ( angle = 0; angle < 0x1000; ++angle ) {
			while ( DAC_Channel_DataEmpty( &DACA, CH0 ) == false ) {
				/* Blocking code waiting for empty register. */
			}
			DAC_Channel_Write( &DACA, 0xFFF - angle, CH0 );
			while ( DAC_Channel_DataEmpty( &DACA, CH1 ) == false ) {
				/* Blocking code waiting for empty register. */
			}
			DAC_Channel_Write( &DACA, angle, CH1 );
		}
	}
}
