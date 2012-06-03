/*
 * servo_in.c
 *
 * Created: 08.04.2012 20:42:56
 *  Author: f-ruml
 */ 

#include "servo_in.h"
#include <pmic.h>
#include <tc.h>

void servo_in_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo)
{
	//if(!(board->mode & BOARD_MODE_TRANSMITTER))
	//return;
	
	servo->servo1 = SERVO_IN_DEFAULT_VALUE;
	servo->servo2 = SERVO_IN_DEFAULT_VALUE;
	servo->servo3 = SERVO_IN_DEFAULT_VALUE;
	servo->servo4 = SERVO_IN_DEFAULT_VALUE;
	servo->servo5 = SERVO_IN_DEFAULT_VALUE;
	servo->servo6 = SERVO_IN_DEFAULT_VALUE;
	servo->servo7 = SERVO_IN_DEFAULT_VALUE;
	servo->lastInput = 0x00;
	
	/* enable interrupt on servo input pins */
	BOARD_SERVO_IN_PORT.INTCTRL = (BOARD_SERVO_IN_PORT.INTCTRL & ~PORT_INT0LVL_gm) | PORT_INT0LVL_MED_gc;
	BOARD_SERVO_IN_PORT.INT0MASK = BOARD_SERVO_IN_1_PIN_bm | BOARD_SERVO_IN_2_PIN_bm | BOARD_SERVO_IN_3_PIN_bm |
									BOARD_SERVO_IN_4_PIN_bm | BOARD_SERVO_IN_5_PIN_bm | BOARD_SERVO_IN_6_PIN_bm |
									BOARD_SERVO_IN_7_PIN_bm;
									

	/* Tick every 2,0 us */
	tc_enable(&TCC0);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 10000);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV64_gc);
}

void isr_servo_in(SERVO_IN_t *servo)
{
	
	PORT_t *port = &PORTH;
	TC0_t *tc = &TCC0;
	uint8_t readServoInPort = port->IN;
	
	/* Check for a rising edge on the input pins*/
	if((readServoInPort & ~(servo->lastInput)) & 0b00000001)
		servo->servo1StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b00000010)
		servo->servo2StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b00000100)
		servo->servo3StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b00001000)
		servo->servo4StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b00010000)
		servo->servo5StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b00100000)
		servo->servo6StartCount = tc->CNT;
	if((readServoInPort & ~(servo->lastInput)) & 0b01000000)
		servo->servo7StartCount = tc->CNT;

	/* Check for a falling edge on the input pins*/
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00000001))
	{
		/* If an overflow occurred substrate the timer end value */
		if((tc->CNT) < servo->servo1StartCount)
			servo->servo1 = (tc->CNT + 10000 - servo->servo1StartCount) *2;	
		/* Save the timer value directly */	
		else
			servo->servo1 = (tc->CNT - servo->servo1StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00000010))
	{
		if((tc->CNT) < servo->servo2StartCount)
			servo->servo2 = (tc->CNT + 10000 - servo->servo1StartCount) *2;	
		else
			servo->servo2 = (tc->CNT - servo->servo2StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00000100))
	{
		if((tc->CNT) < servo->servo3StartCount)
			servo->servo3 = (tc->CNT + 10000 - servo->servo3StartCount) *2;	
		else
			servo->servo3 = (tc->CNT - servo->servo3StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00001000))
	{
		if((tc->CNT) < servo->servo4StartCount)
			servo->servo4 = (tc->CNT + 10000 - servo->servo4StartCount) *2;	
		else
			servo->servo4 = (tc->CNT - servo->servo4StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00010000))
	{
		if((tc->CNT) < servo->servo5StartCount)
			servo->servo5 = (tc->CNT + 10000 - servo->servo5StartCount) *2;	
		else
			servo->servo5 = (tc->CNT - servo->servo5StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b00100000))
	{
		if((tc->CNT) < servo->servo6StartCount)
			servo->servo6 = (tc->CNT + 10000 - servo->servo6StartCount) *2;	
		else
			servo->servo6 = (tc->CNT - servo->servo6StartCount) *2;	
	}
	if(!((readServoInPort & ~(servo->lastInput)) & 0b01000000))
	{
		if((tc->CNT) < servo->servo7StartCount)
			servo->servo7 = (tc->CNT + 10000 - servo->servo7StartCount) *2;	
		else
			servo->servo7 = (tc->CNT - servo->servo7StartCount) *2;	
	}
	
	/* Save the last input value */
	servo->lastInput = readServoInPort;
}