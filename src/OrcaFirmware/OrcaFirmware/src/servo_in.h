/*
 * servo_in.h
 *
 * Created: 08.04.2012 20:43:09
 *  Author: f-ruml
 */ 

		
#ifndef SERVO_IN_H_
#define SERVO_IN_H_

#include<stdint.h>
#include "user_board.h"
#include "system_info.h"

#define SERVO_IN_USED_TIMER				TCF0					/*!< brief Used Timer Module for Servo In counting */
#define SERVO_IN_DEFAULT_VALUE			0x00					/*!< brief Inittialisation default servo value */
#define SERVO_IN_ISR_LVL				PORT_INT0LVL_HI_gc		/*!< brief Servo Input ISR Level */		
#define SERVO_IN_WRONG_VALUE_ERR_LVL	ERR_LVL_LOW

#define SERVO_IN_TIMER_OVERFLOW_PERIOD	20000															/*!< brief PWM period (in us) */
#define SERVO_IN_F_CPU					32000000.0f														// TODO: Remove this constant!
#define SERVO_IN_PRESCALER_VALUE		64																/*!< brief Prescaler value */
#define SERVO_IN_PRESCALER				TC_CLKSEL_DIV64_gc												/*!< brief Prescaler */
#define SERVO_IN_TICK_DURATION			((1.0f * SERVO_IN_PRESCALER_VALUE/SERVO_IN_F_CPU )*1000000)		/*!< brief Time for one tick (in us) */
#define SERVO_IN_COUNTER_TOP_VALUE		((SERVO_IN_TIMER_OVERFLOW_PERIOD/ SERVO_IN_TICK_DURATION) - 1)	/*!< brief Counter Top Value */
#define SERVO_IN_LOWER_PULSE_WIDTH		500																/*!< brief Lower servo pulse width (in us) */
#define SERVO_IN_UPPER_PULSE_WIDTH		2500															/*!< brief Upper servo pulse width (in us) */

/*! Servo Input struct */
typedef struct SERVO_IN {
	volatile uint16_t servo1;						/*!< brief Actual value on Servo Port 1 IN */
	volatile uint16_t servo2;						/*!< brief Actual value on Servo Port 2 IN */
	volatile uint16_t servo3;						/*!< brief Actual value on Servo Port 3 IN */
	volatile uint16_t servo4;						/*!< brief Actual value on Servo Port 4 IN */
	volatile uint16_t servo5;						/*!< brief Actual value on Servo Port 5 IN */
	volatile uint16_t servo6;						/*!< brief Actual value on Servo Port 6 IN */
	volatile uint16_t servo7;						/*!< brief Actual value on Servo Port 7 IN */
	volatile uint16_t servo1StartCount;				/*!< brief Timer start value for Servo 1 */
	volatile uint16_t servo2StartCount;				/*!< brief Timer start value for Servo 2 */
	volatile uint16_t servo3StartCount;				/*!< brief Timer start value for Servo 3 */
	volatile uint16_t servo4StartCount;				/*!< brief Timer start value for Servo 4 */
	volatile uint16_t servo5StartCount;				/*!< brief Timer start value for Servo 5 */
	volatile uint16_t servo6StartCount;				/*!< brief Timer start value for Servo 6 */
	volatile uint16_t servo7StartCount;				/*!< brief Timer start value for Servo 7 */
	volatile uint8_t lastInput;						/*!< brief The last Servo Port input value (for detecting edges on the input pin) */
}SERVO_IN_t;

void servo_in_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo);
void isr_servo_in(SERVO_IN_t *servo);

#endif /* SERVO_IN_H_ */