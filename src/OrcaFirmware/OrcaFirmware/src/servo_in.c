/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \\file *********************************************************************
 *
 * \\brief
 *			Servo Input source file.
 *
 *			
 *
 *
 * \\par Application note:
 *
 * \\author
 *      f-ruml
 *
 * \\par Device
 * 		ATxmega 128A1 \n
 * \\par File
 * 		servo_in.c \\n
 * \\par Date
 * 		 05.06.2012  \\n
 * \\todo
 *
 * \\par History
 * 		 -v0.01 first version
 *****************************************************************************/

#include "servo_in.h"
#include <pmic.h>
#include <tc.h>

PORT_t *port = &BOARD_SERVO_IN_PORT;
TC0_t *tc = &SERVO_IN_USED_TIMER;
	
/**************************************************************************
* \\brief Servo In Initialization
*	Initialize the Servo Input Subsystem. Enable Interrupts for the Input Pins
*	and configure the timer module for pulse width counting. /n
*
* \\note The right flight mode must be selected
*
* \\param *board Board data structure
* \\param *servo Servo IN data structure
*
* \\return  ---
***************************************************************************/
void servo_in_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo)
{
	if(!(board->mode & BOARD_MODE_TRANSMITTER))
		return;
	
	servo->servo1 = SERVO_IN_DEFAULT_VALUE;
	servo->servo2 = SERVO_IN_DEFAULT_VALUE;
	servo->servo3 = SERVO_IN_DEFAULT_VALUE;
	servo->servo4 = SERVO_IN_DEFAULT_VALUE;
	servo->servo5 = SERVO_IN_DEFAULT_VALUE;
	servo->servo6 = SERVO_IN_DEFAULT_VALUE;
	servo->servo7 = SERVO_IN_DEFAULT_VALUE;
	servo->lastInput = 0x00;
	
	/* enable interrupt on servo input pins */
	port->INTCTRL = (port->INTCTRL & ~PORT_INT0LVL_gm) | SERVO_IN_ISR_LVL;
	port->INT0MASK = BOARD_SERVO_IN_1_PIN_bm | BOARD_SERVO_IN_2_PIN_bm | BOARD_SERVO_IN_3_PIN_bm |
									BOARD_SERVO_IN_4_PIN_bm | BOARD_SERVO_IN_5_PIN_bm | BOARD_SERVO_IN_6_PIN_bm |
									BOARD_SERVO_IN_7_PIN_bm;
									

	/* Tick every 2,0 us. The Timer is used to count the pulse width. */
	tc_enable(tc);
	tc_set_wgm(tc, TC_WG_NORMAL);
	tc_write_period(tc, SERVO_IN_COUNTER_TOP_VALUE);
	tc_write_clock_source(tc, SERVO_IN_PRESCALER);
}

/**************************************************************************
* \\brief Servo In ISR
*	Starts the pulse width counting with a rising edge and safe the input
*	value during the falling edge. The Input value is stored in us (microseconds). /n
*
* \\todo failsafe? 
*
* \\param *servo Servo IN data structure
*
* \\return  ---
***************************************************************************/
void isr_servo_in(SERVO_IN_t *servo)
{	
	uint8_t readServoInPort = port->IN;
	uint16_t pulseWidthValue = 0;
	
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
	if((~readServoInPort & (servo->lastInput)) & 0b00000001)
	{
		/* If an overflow occurred substrate the timer end value */
		if((tc->CNT) < servo->servo1StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo1StartCount) *SERVO_IN_TICK_DURATION;				
		/* Save the timer value directly */	
		else
			pulseWidthValue = (tc->CNT - servo->servo1StartCount) *SERVO_IN_TICK_DURATION;

		/* Validate the pulse Width Value */		
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo1 = pulseWidthValue;				
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b00000010)
	{
		if((tc->CNT) < servo->servo2StartCount)	
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo2StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo2StartCount) *SERVO_IN_TICK_DURATION;	
		
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo2 = pulseWidthValue;
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b00000100)
	{
		if((tc->CNT) < servo->servo3StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo3StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo3StartCount) *SERVO_IN_TICK_DURATION;
					
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo3 = pulseWidthValue;	
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b00001000)
	{
		if((tc->CNT) < servo->servo4StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo4StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo4StartCount) *SERVO_IN_TICK_DURATION;
			
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo4 = pulseWidthValue;				
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b00010000)
	{
		if((tc->CNT) < servo->servo5StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo5StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo5StartCount) *SERVO_IN_TICK_DURATION;
						
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo5 = pulseWidthValue;	
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b00100000)
	{
		if((tc->CNT) < servo->servo6StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo6StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo6StartCount) *SERVO_IN_TICK_DURATION;
						
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo6 = pulseWidthValue;	
	}
	
	if((~readServoInPort & (servo->lastInput)) & 0b01000000)
	{
		if((tc->CNT) < servo->servo7StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo->servo7StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo->servo7StartCount) *SERVO_IN_TICK_DURATION; 
									
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servo->servo7 = pulseWidthValue;	
	}
	
	/* Save the last input value */
	servo->lastInput = readServoInPort;
}