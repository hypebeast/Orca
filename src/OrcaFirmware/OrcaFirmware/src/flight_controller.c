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
#include "pid.h"

pidData_t rollPid;
FILTER_DATA_t *actualSensorData;
int16_t actuatingRoll = 0;
float rollSetValue = 0;

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
void flight_controller_init(BOARD_CONFIG_t *board, ORCA_FLASH_SETTINGS_t *settings, SERVO_IN_t *servo, FILTER_DATA_t *filter, FLIGHT_CONTROLLER_t *flightController)
{
	//TODO: Do all init stuff here
	
	actualSensorData = filter;
	/* RC is used */
	if(board->mode && BOARD_MODE_TRANSMITTER)
	{
		flightController->mode |= FLIGHT_CONTROLLER_MODE_RC;
		flightController->rcServoIn = servo;
	}		
	
	/* Roll PID Init */
	pid_Init(settings->pid_roll_p_factor, settings->pid_roll_i_factor, settings->pid_roll_d_factor, &rollPid);	
}	

/**************************************************************************
* \\brief Flight Controller Calculate Left EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
int flight_controller_calc_roll(FLIGHT_CONTROLLER_t *flightController)
{
	/* Sollwert f�r Rollwinkel berechnen. Sollwert wird von der Fernsteuerung vorgegeben. */
	rollSetValue = ((float)(flightController->rcServoIn->servo2)-1534) * 
					(float)(FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF/FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF);
	
	/*  */
	actuatingRoll = pid_Controller((int16_t)(rollSetValue * 10), (int16_t)(actualSensorData->roll * 10), 10000, &rollPid) / 10;	
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
	//if(flightController->rcServoIn->servo2 > FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
		//flightController->leftEdfSetPoint -= (flightController->rcServoIn->servo2
												//- FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)*FLIGHT_CONTROLLER_AILERON_FACTOR;
	if(actuatingRoll>0)
	{
		flightController->leftEdfSetPoint -= (uint16_t)(actuatingRoll *(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));
	}
	 
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
	//if(flightController->rcServoIn->servo2 < FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH)
		//flightController->rightEdfSetPoint -= (FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH
												//- flightController->rcServoIn->servo2)*FLIGHT_CONTROLLER_AILERON_FACTOR;
	if(actuatingRoll<0)
	{
		flightController->rightEdfSetPoint -= (uint16_t)(-1*actuatingRoll *(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));
	}	
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

	if(flightController->rcServoIn->servo3 >= FLIGHT_CONTROLLER_EDF_REAR_START_OFFSET)
		flightController->rearEdfSetPoint = FLIGHT_CONTROLLER_EDF_REAR_OFFSET+ flightController->rcServoIn->servo3;

	else
		flightController->rearEdfSetPoint = FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH;	
		
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \brief Flight Controller Update PID Controller
*
* Call this method to update the setting of the specified PID Controller.
*
* \TODO: Auf mehrere PID Regler erweitern!
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
*
* \return  status code
***************************************************************************/
void flight_controller_update_pid_controller(int16_t p_factor, int16_t i_factor, int16_t d_factor)
{
	pid_update_tuning_constants(p_factor, i_factor, d_factor, &rollPid);
}

/**************************************************************************
* \brief Flight Controller Get Actuating Roll
*
*  Returns the actuating roll angle in degrees. This is the value calculated
*  by the PID controller.
*
* \param -
*
* \return  actuating roll angle
***************************************************************************/
float flight_controller_get_actuating_roll_angle(void)
{
	return actuatingRoll;
}

/**************************************************************************
* \brief Flight Controller Get Actuating Roll
*
*  Returns the estimated roll angle in degrees.
*
* \param -
*
* \return  actuating roll angle
***************************************************************************/
float flight_controller_get_sensor_roll_angle(void)
{
	return actualSensorData->roll;
}

/**************************************************************************
* \brief Flight Controller Get Set Roll Angle
*
*  Returns the set roll angle in degrees. The angle is calculated from the 
*  servo input signals.
*
* \param -
*
* \return  set roll angle
***************************************************************************/
float flight_controller_get_set_roll_angle(void)
{
	return rollSetValue;
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
		flight_controller_calc_roll(flightController);		
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