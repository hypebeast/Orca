/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \\file *********************************************************************
 *
 * \\brief
 *			Servo Input source file./n
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

//////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////
PORT_t *port = &BOARD_SERVO_IN_PORT;
TC0_t *tc = &SERVO_IN_USED_TIMER;
SERVO_IN_t * servoIn;

volatile uint16_t servo1StartCount;	// Timer start value for Servo 1		
volatile uint16_t servo2StartCount;	// Timer start value for Servo 2		
volatile uint16_t servo3StartCount;	// Timer start value for Servo 3		
volatile uint16_t servo4StartCount;	// Timer start value for Servo 4		
volatile uint16_t servo5StartCount;	// Timer start value for Servo 5		
volatile uint16_t servo6StartCount;	// Timer start value for Servo 6		
volatile uint16_t servo7StartCount;	// Timer start value for Servo 7
volatile uint8_t lastServoInput	= 0; //The last Servo Port input value on the port pins

//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////
static void isr_servo_in(void);
	
/**************************************************************************
* \\brief Servo In Initialization
*	Initialize the Servo Input Subsystem. Enable Interrupts for the 
*	Input Pins and configure the timer module for pulse width counting. /n
*
* \\note The right flight mode must be selected to activate the servo 
*		 input readings. /n
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
	
	servoIn = servo;
	servoIn->servo1 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo2 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo3 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo4 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo5 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo6 = SERVO_IN_DEFAULT_VALUE;
	servoIn->servo7 = SERVO_IN_DEFAULT_VALUE;
	
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
*	Starts the pulse width counting on a rising edge and safe the input
*	value during the falling edge. The Input value is stored in us (microseconds). /n
*
* \\todo failsafe? 
*
* \\param *servo Servo IN data structure
*
* \\return  ---
***************************************************************************/
static void isr_servo_in(void)
{	
	uint8_t readServoInPort = port->IN;
	uint16_t pulseWidthValue = 0;
	
	/* Check for a rising edge on the input pins and dave the timer value */
	if((readServoInPort & ~ lastServoInput) & 0b00000001)
		servo1StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b00000010)
		servo2StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b00000100)
		servo3StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b00001000)
		servo4StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b00010000)
		servo5StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b00100000)
		servo6StartCount = tc->CNT;
	if((readServoInPort & ~ lastServoInput) & 0b01000000)
		servo7StartCount = tc->CNT; 

	/* Check for a falling edge on the input pins*/
	if((~readServoInPort & lastServoInput) & 0b00000001)
	{
		/* If an overflow occurred substrate the timer end value */
		if((tc->CNT) < servo1StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo1StartCount) *SERVO_IN_TICK_DURATION;				
		/* Save the timer value directly */	
		else
			pulseWidthValue = (tc->CNT - servo1StartCount) *SERVO_IN_TICK_DURATION;

		/* Validate the pulse Width Value */		
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo1 = pulseWidthValue;				
	}
	
	if((~readServoInPort & lastServoInput) & 0b00000010)
	{
		if((tc->CNT) < servo2StartCount)	
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo2StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo2StartCount) *SERVO_IN_TICK_DURATION;	
		
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo2 = pulseWidthValue;
	}
	
	if((~readServoInPort & lastServoInput) & 0b00000100)
	{
		if((tc->CNT) < servo3StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo3StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo3StartCount) *SERVO_IN_TICK_DURATION;
					
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo3 = pulseWidthValue;	
	}
	
	if((~readServoInPort & lastServoInput) & 0b00001000)
	{
		if((tc->CNT) < servo4StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo4StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo4StartCount) *SERVO_IN_TICK_DURATION;
			
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo4 = pulseWidthValue;				
	}
	
	if((~readServoInPort & lastServoInput) & 0b00010000)
	{
		if((tc->CNT) < servo5StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo5StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo5StartCount) *SERVO_IN_TICK_DURATION;
						
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo5 = pulseWidthValue;	
	}
	
	if((~readServoInPort & lastServoInput) & 0b00100000)
	{
		if((tc->CNT) < servo6StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo6StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo6StartCount) *SERVO_IN_TICK_DURATION;
						
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo6 = pulseWidthValue;	
	}
	
	if((~readServoInPort & lastServoInput) & 0b01000000)
	{
		if((tc->CNT) < servo7StartCount)
			pulseWidthValue = (tc->CNT + SERVO_IN_COUNTER_TOP_VALUE - servo7StartCount) *SERVO_IN_TICK_DURATION;	
		else
			pulseWidthValue = (tc->CNT - servo7StartCount) *SERVO_IN_TICK_DURATION; 
									
		if(pulseWidthValue > SERVO_IN_UPPER_PULSE_WIDTH || pulseWidthValue < SERVO_IN_LOWER_PULSE_WIDTH)
			process_info_command(SYSTEM_INFO_WRONG_SERVO_INPUT_VALUE,SERVO_IN_WRONG_VALUE_ERR_LVL);
		else
			servoIn->servo7 = pulseWidthValue;	
	}
	
	/* Save the last input value */
	lastServoInput = readServoInPort;
}

/**************************************************************************
* \brief The ISR is called on every change on the input pins.
**************************************************************************/
ISR(PORTH_INT0_vect)
{
	isr_servo_in();	
}