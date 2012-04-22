/**
 * \file servo.h
 *
 * \brief Header file for servo.c
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */


#ifndef SERVO_H_
#define SERVO_H_

#include <asf.h>
#include <sysclk.h>
#include "pwm.h"

#define SERVO1_TIMER TCC1
#define SERVO_F_CPU 2000000.0f // TODO: Remove this constant!
#define SERVO_TIMER_OVERFLOW_PERIOD 20 // PWM period (in ms)
#define SERVO_PRESCALER_VALUE 1 // Prescaler value
#define SERVO_TICK_DURATION ((1.0f/(SERVO_F_CPU * SERVO_PRESCALER_VALUE)) * 1000) // Time for one tick (in ms)
#define SERVO_PWM_TOP_VALUE ((SERVO_TIMER_OVERFLOW_PERIOD/ SERVO_TICK_DURATION) - 1) // PRESCALER = 1
#define SERVO_LOWER_PULSE_WIDTH 0.49f // in ms
#define SERVO_UPPER_PULSE_WIDTH 2.35f // in ms
#define SERVO_PULSE_WIDTH_OFFSET SERVO_LOWER_PULSE_WIDTH
#define SERVO_PULSE_WIDTH (SERVO_UPPER_PULSE_WIDTH - SERVO_LOWER_PULSE_WIDTH)
#define SERVO_TICKS_PER_DEGREE (SERVO_PULSE_WIDTH/180.0f)

/**
* \brief Servo configuration
*/
struct servo_config {
	enum TC_CLKSEL_enum clk_sel;
};

void servo_init(void);
void servo_set_frequency(void);
void servo_set_pos_degree(uint8_t servo_nr, float pos);
uint16_t servo_get_pos_degree(uint8_t servo_nr);
void servo_set_pos_pulse_width(uint8_t servo_nr, uint16_t duration);

#endif /* SERVO_H_ */