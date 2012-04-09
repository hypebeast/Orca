/**
 * \file servo.c
 *
 * \brief This file contains the code 
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */

#include "servo.h"

volatile uint16_t servo_compare_period[2];

volatile uint16_t test_counter = 0;
volatile uint16_t test_pos = 0;
volatile uint8_t test_direction = 0;


static void servo1_overflow_callback()
{
	//gpio_toggle_pin(LED1);
	
	/*
	if (((test_counter++) % 50) == 0) {
		gpio_toggle_pin(LED1);
		
		if (test_direction == 0) {
			test_pos = test_pos + 5;
			if (test_pos > 180) {
				test_pos == 180;
				test_direction = 1;
			}		
		}
		else {
			test_pos = test_pos - 5;
			if (test_pos < 0) {
				test_pos = 0;
				test_direction = 0;
			}
		}

		set_servo_pos(1, test_pos);
	}
	*/
}

static void servo1_cca_overflow_callback()
{
	
}

/**
* \brief This function initializes the servo subsystem.
*
*/
void servo_init()
{	
	servo_compare_period[0] = SERVO_TIMER_OVERFLOW_PERIOD / SERVO_TIMER_OVERFLOW_PERIOD_IN_MS;
	servo_compare_period[1] = SERVO_TIMER_OVERFLOW_PERIOD / SERVO_TIMER_OVERFLOW_PERIOD_IN_MS;
	
	// Enable timer/counter TCC0
	tc_enable(&SERVO1_TIMER);
	
	// Set the callback function for overflow interrupt
	tc_set_overflow_interrupt_callback(&SERVO1_TIMER, servo1_overflow_callback);
	
	// Set the waveform
	tc_set_wgm(&SERVO1_TIMER, TC_WG_SS);
	
	// Set the period
	tc_write_period(&SERVO1_TIMER, SERVO_TIMER_OVERFLOW_PERIOD);
	//tc_set_resolution(&SERVO1_TIMER, 2000000); // 2 MHz
	tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[0]);
	tc_enable_cc_channels(&SERVO1_TIMER, (enum tc_cc_channel_mask_enable_t)TC_CCAEN);
	
	// Set the overflow interrupt level
	tc_set_overflow_interrupt_level(&SERVO1_TIMER, TC_INT_LVL_LO);
	
	// Set the clock source
	tc_write_clock_source(&SERVO1_TIMER, TC_CLKSEL_DIV1_gc);
}

/**
* \brief This function sets the position for servo 1 or 2.
*/
void set_servo_pos(uint16_t servo_nr, uint16_t pos)
{
	if (pos < 0 || pos > 180)
		return;
		
	// 0 Grad entspricht 1ms und 180 Grad entspricht 2ms
	servo_compare_period[servo_nr-1] = (int)(SERVO_TIMER_OVERFLOW_PERIOD/SERVO_TIMER_OVERFLOW_PERIOD_IN_MS) * (1 + (CONVERSION_FACTOR * pos));
	tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
}

/**
* \brief This function returns the actual position of a servo.
*/
uint16_t get_servo_pos(uint16_t servo_nr)
{
	uint16_t pos = 0;
	
	pos = (servo_compare_period[servo_nr-1]);
	return pos;
}
