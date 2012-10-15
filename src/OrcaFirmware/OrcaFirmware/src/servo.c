/**
 * \file servo.c
 *
 * \brief This file contains the code 
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */

#include "servo.h"

// Timer compare values for all output channels
uint16_t servo_compare_period[SERVO_NUMBER_OF_OUTPUT_CHANNELS];


/**
* \brief Calculates and sets the right frequency for the timer.
*/
void servo_set_frequency()
{
	// TODO
	//uint32_t cpu_hz = sysclk_get_cpu_hz();
}

/**
* \brief This function initializes the servo subsystem.
*/
void servo_init()
{	
	for (int i = 0, i < SERVO_NUMBER_OF_OUTPUT_CHANNELS; i++) {
		servo_compare_period[i] = SERVO_LOWER_PULSE_WIDTH / SERVO_TICK_DURATION;
	}
	
	// Enable timers/counters TCC1, TCD1, TCE1
	tc_enable(&SERVO1_TIMER);
	tc_enable(&SERVO3_TIMER);
	tc_enable(&SERVO5_TIMER);
	
	// Set the callback function for overflow interrupt
	//tc_set_overflow_interrupt_callback(&SERVO1_TIMER, servo1_overflow_callback);
	//tc_set_overflow_interrupt_callback(&SERVO3_TIMER, servo3_overflow_callback);
	//tc_set_overflow_interrupt_callback(&SERVO5_TIMER, servo5_overflow_callback);
	
	// Set the waveform
	tc_set_wgm(&SERVO1_TIMER, TC_WG_SS);
	tc_set_wgm(&SERVO3_TIMER, TC_WG_SS);
	tc_set_wgm(&SERVO5_TIMER, TC_WG_SS);
	
	// Set the period
	tc_write_period(&SERVO1_TIMER, SERVO_PWM_TOP_VALUE);
	tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[0]);
	tc_write_cc(&SERVO1_TIMER, TC_CCB, servo_compare_period[1]);
	tc_enable_cc_channels(&SERVO1_TIMER, (enum tc_cc_channel_mask_enable_t)TC_CCAEN | TC_CCBEN);
	
	tc_write_period(&SERVO3_TIMER, SERVO_PWM_TOP_VALUE);
	tc_write_cc(&SERVO3_TIMER, TC_CCA, servo_compare_period[2]);
	tc_write_cc(&SERVO3_TIMER, TC_CCB, servo_compare_period[3]);
	tc_enable_cc_channels(&SERVO3_TIMER, (enum tc_cc_channel_mask_enable_t)TC_CCAEN | TC_CCBEN);
	
	tc_write_period(&SERVO5_TIMER, SERVO_PWM_TOP_VALUE);
	tc_write_cc(&SERVO5_TIMER, TC_CCA, servo_compare_period[4]);
	tc_write_cc(&SERVO5_TIMER, TC_CCB, servo_compare_period[5]);
	tc_enable_cc_channels(&SERVO5_TIMER, (enum tc_cc_channel_mask_enable_t)TC_CCAEN | TC_CCBEN);	
	
	// Set the overflow interrupt level
	tc_set_overflow_interrupt_level(&SERVO1_TIMER, TC_INT_LVL_LO);
	tc_set_overflow_interrupt_level(&SERVO3_TIMER, TC_INT_LVL_LO);
	tc_set_overflow_interrupt_level(&SERVO5_TIMER, TC_INT_LVL_LO);
	
	// Set the clock source
	tc_write_clock_source(&SERVO1_TIMER, TC_CLKSEL_DIV8_gc);
	tc_write_clock_source(&SERVO3_TIMER, TC_CLKSEL_DIV8_gc);
	tc_write_clock_source(&SERVO5_TIMER, TC_CLKSEL_DIV8_gc);
}

/**
* \brief This function sets the position for servo 1 or 2.
*/
void servo_set_pos_degree(uint8_t servo_nr, float pos)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	if (pos < 0 || pos > 180)
		return;
		
	// Calculate the pulse width for the given position
	float pulse_width = SERVO_PULSE_WIDTH_OFFSET + (SERVO_PULS_WIDTH_PER_DEGREE * pos);
	servo_compare_period[servo_nr-1] = pulse_width / SERVO_TICK_DURATION;
	
	if (servo_nr == 1) {
		tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 2) {
		tc_write_cc(&SERVO1_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 3) {
		tc_write_cc(&SERVO3_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 4) {
		tc_write_cc(&SERVO3_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 5) {
		tc_write_cc(&SERVO5_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 6) {
		tc_write_cc(&SERVO5_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
}

/**
* \brief This function returns the actual position of a servo in degrees.
*/
uint16_t servo_get_pos_degree(uint8_t servo_nr)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	uint16_t pos = 0;
	
	float pulse_width =  servo_compare_period[servo_nr-1] * SERVO_TICK_DURATION; // Calculate the pulse width
	pos = (pulse_width - SERVO_PULSE_WIDTH_OFFSET) / SERVO_PULS_WIDTH_PER_DEGREE; // Calculate the position
	
	return pos;
}

/**
* \brief This function sets a new position for the given servo. The new position
* is given in ticks (pulse width in microseconds).
*/
void servo_set_pos_ticks(uint8_t servo_nr, uint16_t pos)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	if (pos < 500 || pos > 2500)
		return;
		
	// Calculate the pulse width for the given position
	servo_compare_period[servo_nr-1] = pos * 4;

	if (servo_nr == 1) {
		tc_write_cc(&SERVO1_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 2) {
		tc_write_cc(&SERVO1_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 3) {
		tc_write_cc(&SERVO3_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 4) {
		tc_write_cc(&SERVO3_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 5) {
		tc_write_cc(&SERVO5_TIMER, TC_CCA, servo_compare_period[servo_nr-1]);
	}
	if (servo_nr == 6) {
		tc_write_cc(&SERVO5_TIMER, TC_CCB, servo_compare_period[servo_nr-1]);
	}
}

/**
* \brief This function returns the current position of the specified servo in
* ticks (microseconds).
*/
uint16_t servo_get_pos_ticks(uint8_t servo_nr)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	return servo_compare_period[servo_nr - 1];
}

/**************************************************************************
* \brief Sets the given output value to the specified output channel. 
* 
* The value range is from 0 to 100 %.
**************************************************************************/
void servo_set_pos_percentage(uint8_t servo_nr, uint8_t pos)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	// TODO
}	

/**************************************************************************
* \brief Returns current output value for tge specified output channel.
*
* Values are from 0 to 100 %.
**************************************************************************/
uint8_t servo_get_pos_percentage(uint8_t servo_nr)
{
	if (servo_nr < 1 || servo_nr > SERVO_NUMBER_OF_OUTPUT_CHANNELS) {
		return;
	}
	
	// TODO
}	
