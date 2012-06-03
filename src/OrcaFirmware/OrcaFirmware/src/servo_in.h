/*
 * servo_in.h
 *
 * Created: 08.04.2012 20:43:09
 *  Author: f-ruml
 */ 

		
#ifndef SERVO_IN_H_
#define SERVO_IN_H_

#include<stdint.h>
#include "orca_board.h"

#define SERVO_IN_DEFAULT_VALUE			0x00		/*!< brief Inittialisation default servo value */		

/*! Servo Input struct */
typedef struct SERVO_IN {
	volatile uint16_t servo1;					/*!< brief Actual value on Servo Port 1 IN */
	volatile uint16_t servo2;					/*!< brief Actual value on Servo Port 2 IN */
	volatile uint16_t servo3;					/*!< brief Actual value on Servo Port 3 IN */
	volatile uint16_t servo4;					/*!< brief Actual value on Servo Port 4 IN */
	volatile uint16_t servo5;					/*!< brief Actual value on Servo Port 5 IN */
	volatile uint16_t servo6;					/*!< brief Actual value on Servo Port 6 IN */
	volatile uint16_t servo7;					/*!< brief Actual value on Servo Port 7 IN */
	volatile uint16_t servo1StartCount;				/*!< brief Actual value on Servo Port 1 IN */
	volatile uint16_t servo2StartCount;				/*!< brief Actual value on Servo Port 2 IN */
	volatile uint16_t servo3StartCount;				/*!< brief Actual value on Servo Port 3 IN */
	volatile uint16_t servo4StartCount;				/*!< brief Actual value on Servo Port 4 IN */
	volatile uint16_t servo5StartCount;				/*!< brief Actual value on Servo Port 5 IN */
	volatile uint16_t servo6StartCount;				/*!< brief Actual value on Servo Port 6 IN */
	volatile uint16_t servo7StartCount;				/*!< brief Actual value on Servo Port 7 IN */
	volatile uint8_t lastInput;	
}SERVO_IN_t;

void servo_in_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo);
void isr_servo_in(SERVO_IN_t *servo);

#endif /* SERVO_IN_H_ */