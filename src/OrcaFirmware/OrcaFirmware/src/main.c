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
#include <pmic.h>
#include <pll.h>
#include "serial_api.h"
#include <osc.h>

//just for test
#include "user_board.h"
#include "orca_init.h"
#include "servo_in.h"
#include "servo.h"
#include "flight_controller.h"

/* global variables */
BOARD_CONFIG_t board;  						/*!< \brief board module */
SERVO_IN_t servoIn;							/*!< \brief servo in module */
FLIGHT_CONTROLLER_t flightController;		/*!< \brief flight controller module */

 static void ISR_system_timer(void)
 {
	nop();
 }
 
int main (void)
{

	// Initialize all basic board functions
	orca_init();
	
	//testing stuff	
	Stat_LED_ON();
	Err_LED_ON();
	
	servo_set_pos_degree(1,90);
	while(1)
	{
		// Process incoming api commands
		serial_api_task();
	}
	
}

void orca_init()
{
	//struct pll_config pllcfg;
				
	osc_enable(OSC_ID_RC32MHZ);
    osc_wait_ready(OSC_ID_RC32MHZ); 
	//pll_enable_config_defaults(0); 
	
	/* init system timer */
	sysclk_init();
	
	/* board init */
	orca_board_init(&board);
	
	/* servo in subsystem init */
	servo_in_init(&board, &servoIn);
	
	/* servo out subsystem init */
	servo_init();
	
	/* flight controller subsystem init */
	flight_controller_init(&board, &servoIn, &flightController);

	/* Initialize the serial interface */
	serial_api_init();
	
	//tc_enable(&TCC0);
	//tc_set_overflow_interrupt_callback(&TCC0, ISR_system_timer);
	//tc_set_wgm(&TCC0, TC_WG_NORMAL);
	//tc_write_period(&TCC0, 1000);
	//tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_HI);
	cpu_irq_enable();

	
	/* enable interrupts */
	/* Enables all interrupt levels, with vectors located in the application section and fixed priority scheduling */
	pmic_init();
	sei();
	
	//tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}


 
ISR(PORTH_INT0_vect)
{
	isr_servo_in(&servoIn);	
}
