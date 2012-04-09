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
#include "pwm.h"

#define SERVO1_TIMER TCC1
#define SERVO_TIMER_OVERFLOW_PERIOD_IN_MS 20
#define SERVO_TIMER_OVERFLOW_PERIOD 39999 // 20ms (2^16Hz / 50 = 20ms)
#define CONVERSION_FACTOR (1.0f/180.0f)

void servo_init(void);
void set_servo_pos(uint16_t servo_nr, uint16_t pos);
uint16_t get_servo_pos(uint16_t servo_nr);

#endif /* SERVO_H_ */