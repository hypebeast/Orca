/**
 * \file main.c
 *
 * \brief Entry point for the Orca firmware program.
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

#include "serial_api.h"

int main (void)
{
	// Initialize all basic board functions
	board_init();
	
	// Initialize the serial interface
	serial_api_init();
	
	while (true) {
		
		// Process incoming api commands
		serial_api_task();
	}
}
