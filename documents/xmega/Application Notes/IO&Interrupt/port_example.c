/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA I/O Port driver example source.
 *
 *      This file contains an example application that demonstrates the I/O Port
 *      driver. It shows how to set up the I/O Ports as described in the
 *      examples in the "Getting Started" section of the application note
 *      document.
 *
 * \par Application note:
 *      AVR1313: Using the XMEGA I/O pins and External Interrupts
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
#include "avr_compiler.h"
#include "port_driver.h"

void Example1( void );
void Example2( void );
void Example3( void );
void Example4( void );


void main( void )
{
	Example1();
	/*Example2();*/
	/*Example3();*/
	/*Example4();*/

	while(true){

	}
}


/*! \brief Getting started example 1.
 *
 *  This function implements the "Basic Digital I/O" example from the
 *  "Getting Started" section in the application note.
 *
 *  This function will continously read input from 8 switches connected to PORTC
 *  and output the pin state to 8 LEDs connected to PORTD.
 */
void Example1( void )
{
	/* Configure data direction for PORTC and PORTD. */
	PORT_SetPinsAsInput( &PORTC, 0xFF );
	PORT_SetPinsAsOutput( &PORTD, 0xFF );

	/* Continously copy the value from PORTC to PORTD. */
	do {
		uint8_t temp = PORT_GetPortValue( &PORTC );
		PORT_SetOutputValue( &PORTD, temp );
	} while (true);
}


/*! \brief Getting started example 2.
 *
 *  This function implements the "Configuring Several Pins in One Operation"
 *  example from the "Getting Started" section in the application note.
 *
 *  This function configures pins 0-3 on PORTC for Wired-AND with pull-up
 *  operation.
 */
void Example2( void )
{
	PORT_ConfigurePins( &PORTC,
	                    0x0F,
	                    false,
	                    false,
	                    PORT_OPC_WIREDANDPULL_gc,
	                    PORT_ISC_BOTHEDGES_gc );
}


/*! \brief Getting started example 3.
 *
 *  This function implements the "Mapping Real Ports to Virtual Ports" example
 *  from the "Getting Started" section in the application note.
 *
 *  This function maps PORTC to Virtual port 0 and PORTD to Virtual PORT1,
 *  and performs the same task as in Example1() using virtual ports.
 */
void Example3( void )
{
	/* Map the ports. */
	PORT_MapVirtualPort0( PORTCFG_VP0MAP_PORTC_gc );
	PORT_MapVirtualPort1( PORTCFG_VP1MAP_PORTD_gc );

	/* Configure VPORT0 as input and VPORT1 as output. */
	PORT_SetDirection( &VPORT0, 0x00 );
	PORT_SetDirection( &VPORT1, 0xFF );

	/* Continously copy the value from PORTC to PORTD using the
	 * virtual ports.
	 */
	do {
		uint8_t temp = PORT_GetPortValue( &VPORT0 );
		PORT_SetOutputValue( &VPORT1, temp );
	} while (true);
}


/*! \brief Getting started example 4.
 *
 *  This function implements the "Configuring an I/O Pin for Interrupt Generation"
 *  example from the "Getting Started" section in the application note.
 *
 *  This function configures PORTC interrupt0 as a medium level interrupt,
 *  triggered by the rising edge of PC0. The interrupt service routine toggles
 *  the output on PORTD.
 */
void Example4( void )
{
	/* Configure PC0 as input, triggered on rising edge. */
	PORT_ConfigurePins( &PORTC,
	                    0x01,
	                    false,
	                    false,
	                    PORT_OPC_TOTEM_gc,
	                    PORT_ISC_RISING_gc );

	PORT_SetPinsAsInput( &PORTC, 0x01 );

	/* Configure PORTD as output with the pattern 0x55. */
	PORT_SetPinsAsOutput( &PORTD, 0xFF );
	PORT_SetPins( &PORTD, 0x55 );

	/* Configure Interrupt0 to have medium interrupt level, triggered by pin 0. */
	PORT_ConfigureInterrupt0( &PORTC, PORT_INT0LVL_MED_gc, 0x01 );

	/* Enable medium level interrupts in the PMIC. */
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;

	/* Enable the global interrupt flag. */
	sei();
}


/*! \brief Interrupt service routine for Example4()
 *
 *  When initialized using Example4(), this ISR is called on every rising edge
 *  on PC0.
 */
ISR(PORTC_INT0_vect)
{
	PORT_TogglePins( &PORTD, 0xFF );
}

