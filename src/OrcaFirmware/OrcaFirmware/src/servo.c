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


static void servo1_overflow_callback()
{

}

static void servo1_cca_overflow_callback()
{
	
}

/**
* \brief Calculates and sets the right frequency for the timer.
*/
void servo_set_frequency()
{
	// TODO
	uint32_t cpu_hz = sysclk_get_cpu_hz();
}

/**
* \brief This function initializes the servo subsystem.
*
*/
void servo_init()
{	
	servo_compare_period[0] = SERVO_LOWER_PULSE_WIDTH / SERVO_TICK_DURATION;
	servo_compare_period[1] = SERVO_LOWER_PULSE_WIDTH / SERVO_TICK_DURATION;
	
	// Enable timer/counter TCC0
	tc_enable(&SERVO1_TIMER);
	
	// Set the callback function for overflow interrupt
	tc_set_overflow_interrupt_callback(&SERVO1_TIMER, servo1_overflow_callback);
	
	// Set the waveform
	tc_set_wgm(&SERVO1_TIMER, TC_WG_SS);
	
	// Set the period
	tc_write_period(&SERVO1_TIMER, SERVO_PWM_TOP_VALUE);
	tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[0]);
	tc_enable_cc_channels(&SERVO1_TIMER, (enum tc_cc_channel_mask_enable_t)TC_CCAEN);
	
	// Set the overflow interrupt level
	tc_set_overflow_interrupt_level(&SERVO1_TIMER, TC_INT_LVL_LO);
	
	// Set the clock source
	tc_write_clock_source(&SERVO1_TIMER, TC_CLKSEL_DIV8_gc);
}

/**
* \brief This function sets the position for servo 1 or 2.
*/
void servo_set_pos_degree(uint8_t servo_nr, float pos)
{
	if (pos < 0 || pos > 180)
		return;
		
	// Calculate the pulse width for the given position
	float pulse_width = SERVO_PULSE_WIDTH_OFFSET + (SERVO_TICKS_PER_DEGREE * pos);
	servo_compare_period[servo_nr-1] = pulse_width / SERVO_TICK_DURATION;
	
	if (servo_nr == 1) {
		tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	else {
		tc_write_cc(&SERVO1_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
}

/**
* \brief This function returns the actual position of a servo in degrees.
*/
uint16_t servo_get_pos_degree(uint8_t servo_nr)
{
	float pos = 0;
	
	pos =  servo_compare_period[servo_nr-1] * SERVO_TICK_DURATION; // Calculate the pulse width
	pos = pos - (SERVO_PULSE_WIDTH_OFFSET) / SERVO_TICKS_PER_DEGREE;
	
	return pos;
}

/**
* \brief This function sets a new position for the given servo. The new position
* is given in ticks (microseconds).
*/
void servo_set_pos_ticks(uint8_t servo_nr, uint16_t pos)
{
	// TODO
}

/**
* This function returns the current position of the specified servo in ticks (microseconds).
*/
uint16_t servo_get_pos_ticks(uint8_t servo_nr)
{
	// TODO
}
