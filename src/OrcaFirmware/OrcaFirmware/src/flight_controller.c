/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \\file *********************************************************************
 *
 * \\brief
 *			Flight Controller source file.
 *
 *			
 *
 *
 * Servo Input declarations:
 *	-Elevator -> servo input 1			(used)
 *	-Aileron -> servo input 2			(used)
 *	-Throttle -> servo input 3			(used)
 *	-Rudder -> servo input 4			(used)
 *	-Pitch (Gear) -> servo input 5		(not used)
 *  -Aux 1 -> servo input 6				(not used)
 *  -Aux 2 -> servo input 7				(not used)
 * \\par Application note:
 *
 * \\author
 *      f-ruml;s-ruml
 *
 * \\par Device
 * 		ATxmega 128A1 \n
 * \\par File
 * 		flight_controller.c \\n
 * \\par Date
 * 		 05.06.2012  \\n
 * \\todo
 *
 * \\par History
 * 		 -v0.01 first version
 *****************************************************************************/


#include "flight_controller.h"
#include "user_board.h"
#include "system_info.h"
#include "servo.h"

/**************************************************************************
* \\brief Flight Controller Initialization
*
*
* \\param *board Board data structure
* \\param *servo Servo IN data structure
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
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

/**************************************************************************
* \\brief Flight Controller Calculate Left EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_left_edf(FLIGHT_CONTROLLER_t *flightController)
{
	flightController->leftEdfSetPoint = FLIGHT_CONTROLLER_EDF_OFFSET + flightController->rcServoIn->servo3;
	
	/* Pitch/Aileron Control */
	if(flightController->rcServoIn->servo2 > FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
		flightController->leftEdfSetPoint -= (flightController->rcServoIn->servo2
												- FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_AILERON_FACTOR;
	 
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \\brief Flight Controller Calculate Right EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_right_edf(FLIGHT_CONTROLLER_t *flightController)
{
	flightController->rightEdfSetPoint = FLIGHT_CONTROLLER_EDF_OFFSET + flightController->rcServoIn->servo3;
	
	/* Pitch/Aileron Control */
	if(flightController->rcServoIn->servo2 < FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
		flightController->rightEdfSetPoint -= (FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH
												- flightController->rcServoIn->servo2)*FLIGHT_CONTROLLER_AILERON_FACTOR;
		
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \\brief Flight Controller Calculate Left Servo
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_left_servo(FLIGHT_CONTROLLER_t *flightController)
{
	
	if(flightController->rcServoIn->servo1 >= FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
	{	
		flightController->leftServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH -
												(flightController->rcServoIn->servo1 - FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_SERVO_MAX_WAY_FACTOR;
	}												
	else
	{
		flightController->leftServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH +
												(FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH - flightController->rcServoIn->servo1)*FLIGHT_CONTROLLER_SERVO_MAX_WAY_FACTOR;
	}
	
	/* Rudder Control */
	flightController->leftServoSetPoint += (flightController->rcServoIn->servo4 - FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_RUDDER_FACTOR;

	
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \\brief Flight Controller Calculate Right Servo
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_right_servo(FLIGHT_CONTROLLER_t *flightController)
{
	
	if(flightController->rcServoIn->servo1 >= FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
	{	
		flightController->rightServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH +
												(flightController->rcServoIn->servo1 - FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_SERVO_MAX_WAY_FACTOR;
	}												
	else
	{
		flightController->rightServoSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH -
												(FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH - flightController->rcServoIn->servo1)*FLIGHT_CONTROLLER_SERVO_MAX_WAY_FACTOR;
	}
	
	/* Rudder Control */
	flightController->rightServoSetPoint += (flightController->rcServoIn->servo4 - FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_RUDDER_FACTOR;
	
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \\brief Flight Controller Calculate Rear EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_rear_edf(FLIGHT_CONTROLLER_t *flightController)
{

	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \\brief Flight Controller Task
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_task(FLIGHT_CONTROLLER_t *flightController)
{
	if(flightController->mode && FLIGHT_CONTROLLER_MODE_RC)
	{
		//Note: Do not change the call sequence!
		flight_controller_calc_left_edf(flightController);
		flight_controller_calc_right_edf(flightController);
		flight_controller_calc_left_servo(flightController);
		flight_controller_calc_right_servo(flightController);
		flight_controller_calc_rear_edf(flightController);
		
		
		//TODO: Servo Out
		servo_set_pos_ticks(1,flightController->leftEdfSetPoint);
		servo_set_pos_ticks(2,flightController->rightEdfSetPoint);
		servo_set_pos_ticks(3,flightController->leftServoSetPoint);
		servo_set_pos_ticks(4,flightController->rightServoSetPoint);
		servo_set_pos_ticks(5,flightController->rearEdfSetPoint);
		
		return SYSTEM_INFO_TRUE;
	}
	
	else
		return SYSTEM_INFO_NO_FLIGHT_MODE;	
		
}