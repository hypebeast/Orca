/*
 * flight_controller.c
 *
 * Created: 22.04.2012 23:39:18
 *  Author: sruml
 *
 * Servo Input declarations:
 *	-Elevator -> servo input 1
 *	-Aileron -> servo input 2
 *	-Throttle -> servo input 3
 *	-Rudder -> servo input 4
 *	-Pitch (Gear) -> servo input 5
 *  -Aux 1 -> servo input 6
 *  -Aux 2 -> servo input 7
 */ 

#include "flight_controller.h"
#include "user_board.h"
#include "system_info.h"

void flight_controller_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo, FLIGHT_CONTROLLER_t *flightController)
{
	//TODO: Do all init stuff here
	
	/* RC is used */
	if(board->mode && BOARD_MODE_TRANSMITTER)
	{
		flightController->mode |= FLIGHT_CONTROLLER_MODE_RC;
		flightController->rcServoIn = servo;
	}		
		
}	


int flight_controller_convert_rudder_in(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
		return SYSTEM_INFO_WRONG_FLIGHT_MODE;
		
	flightController->leftServoSetPoint = flightController->rcServoIn->servo4;
	
	if(flightController->rcServoIn->servo4 >= FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
	{	
		flightController->leftServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH-
												(flightController->rcServoIn->servo4 - FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH);
	}												
	else
	{
		flightController->leftServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH+
												(FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH - flightController->rcServoIn->servo4);
	}
		
	return SYSTEM_INFO_TRUE;
}

int flight_controller_convert_throttle_in(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
		return SYSTEM_INFO_WRONG_FLIGHT_MODE;
	
	flightController->leftEdfSetPoint = flightController->rcServoIn->servo1;
	flightController->rightEdfSetPoint = flightController->rcServoIn->servo1;
		
	return SYSTEM_INFO_TRUE;	
}

int flight_controller_convert_aileron_in(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
		return SYSTEM_INFO_WRONG_FLIGHT_MODE;
		
	return SYSTEM_INFO_TRUE;	
}

int flight_controller_convert_pitch_in(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
		return SYSTEM_INFO_WRONG_FLIGHT_MODE;
		
	return SYSTEM_INFO_TRUE;	
}

int flight_controller_convert_elevator_in(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
		return SYSTEM_INFO_WRONG_FLIGHT_MODE;
		
	return SYSTEM_INFO_TRUE;	
}

int flight_controller_calc_left_edf(FLIGHT_CONTROLLER_t *flightController)
{
	flight_controller_convert_throttle_in(flightController);
	return SYSTEM_INFO_TRUE;	
}

int flight_controller_calc_left_servo(FLIGHT_CONTROLLER_t *flightController)
{

	return SYSTEM_INFO_TRUE;	
}

int flight_controller_calc_right_edf(FLIGHT_CONTROLLER_t *flightController)
{

	return SYSTEM_INFO_TRUE;	
}

int flight_controller_calc_right_servo(FLIGHT_CONTROLLER_t *flightController)
{

	return SYSTEM_INFO_TRUE;	
}

int flight_controller_calc_rear_edf(FLIGHT_CONTROLLER_t *flightController)
{

	return SYSTEM_INFO_TRUE;	
}

int flight_controller_task(FLIGHT_CONTROLLER_t *flightController)
{
	if(!(flightController->mode && FLIGHT_CONTROLLER_MODE_RC))
	{
		//Note: Do not change the call sequence!
		flight_controller_convert_throttle_in(flightController);
		flight_controller_convert_rudder_in(flightController);
		flight_controller_convert_aileron_in(flightController);
		flight_controller_convert_pitch_in(flightController);
		flight_controller_convert_elevator_in(flightController);
		
		
		//TODO: Servo Out
		return SYSTEM_INFO_TRUE;
	}
	
	else
		return SYSTEM_INFO_NO_FLIGHT_MODE;	
		
}