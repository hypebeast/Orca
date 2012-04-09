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
#include <board.h>

#include "serial_api.h"
#include "servo.h"

void init_orca()
{
	ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 4), (IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH));
}

int main (void)
{
	/* Enable interrupt controller */
	pmic_init();
	
	// Initialize all basic board functions
	board_init();
	
	init_orca();
	
	// Initialize the clock system
	sysclk_init();
	
	// Initialize the sleep manager
	sleepmgr_init();
	
	// Enable interrupts
	cpu_irq_enable();
	
	// Initialize the serial interface
	serial_api_init();
	
	// Initialize servo subsystem
	servo_init();
	
	while (true) {
		// Process incoming API commands
		serial_api_task();
	}
	
	return 0;
}