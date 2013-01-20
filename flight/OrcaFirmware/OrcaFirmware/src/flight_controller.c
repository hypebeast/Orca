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

//////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////
PID_DATA_t rollPid;
PID_DATA_t pitchPid;
PID_DATA_t yawPid;
FILTER_DATA_t *actualSensorData;
float actuatingRoll = 0;
float rollSetValue = 0;
float actuatingPitch = 0;
float pitchSetValue = 0;
float actuatingYaw = 0;
float yawSetValue = 0;

//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////
static void flight_controller_calc_pitch(FLIGHT_CONTROLLER_t *flightController);

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
void flight_controller_init(BOARD_CONFIG_t *board, ORCA_FLASH_SETTINGS_t *settings,
							SERVO_IN_t *servo, FILTER_DATA_t *filter,
							FLIGHT_CONTROLLER_t *flightController)
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
	pid_Init(settings->pid_roll_p_factor, settings->pid_roll_i_factor,
			settings->pid_roll_d_factor, settings->pid_roll_i_limit,
			&rollPid);	
			
	/* Pitch PID Init */
	pid_Init(settings->pid_pitch_p_factor, settings->pid_pitch_i_factor,
			settings->pid_pitch_d_factor, settings->pid_pitch_i_limit,
			&pitchPid);
			
	/* Yaw PID Init */
	pid_Init(settings->pid_yaw_p_factor, settings->pid_yaw_i_factor,
			settings->pid_yaw_d_factor, settings->pid_yaw_i_limit,
			&yawPid);
}	

/**************************************************************************
* \\brief Flight Controller Calculate Left EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
void flight_controller_calc_roll(FLIGHT_CONTROLLER_t *flightController)
{
	/* Sollwert für Rollwinkel berechnen. Sollwert wird von der Fernsteuerung vorgegeben. */
	rollSetValue = ((float)(flightController->rcServoIn->servo2)-1534) * 
					(float)(FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF/FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF);
	
	/* Process the roll PID controller */
	actuatingRoll = pid_Controller(rollSetValue, actualSensorData->roll, 10000, &rollPid);	
}

/**************************************************************************
* \\brief Flight Controller Calculate Left EDF
*
*
* \\param *flightController Flight Controller data structure
*
* \\return  ---
***************************************************************************/
static void flight_controller_calc_pitch(FLIGHT_CONTROLLER_t *flightController)
{
	/* Sollwert für Rollwinkel berechnen. Sollwert wird von der Fernsteuerung vorgegeben. */
	pitchSetValue = ((float)(flightController->rcServoIn->servo1)-1558) *
	(float)(FLIGHT_CONTROLLER_PITCH_MAX_ANGLE_CONF/FLIGHT_CONTROLLER_ELEVATOR_DELTA_VALUE_CONF);
	
	/* Process the pitch PID controller */
	actuatingPitch = pid_Controller(pitchSetValue, actualSensorData->pitch, 10000, &pitchPid);
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
	/* Wait for EDF controlling until the RC throttle stick gives enough input.
	 * This avoid unwanted EDF running from the INS. */
	if(flightController->rcServoIn->servo3 > FLIGHT_CONTROLLER_EDF_START_OFFSET)
	{
		/* Use the throttle RC input as the main EDF speed */
		flightController->leftEdfSetPoint = FLIGHT_CONTROLLER_EDF_OFFSET + flightController->rcServoIn->servo3;
		
		#ifdef FLIGHT_CONTROLLER_USE_STABILIZATION
			/* Reduce and add the roll PID output to the EDF speed */
			if(actuatingRoll>0)
			{
				flightController->leftEdfSetPoint -= (uint16_t)actuatingRoll;
				//flightController->leftEdfSetPoint -= (uint16_t)(actuatingRoll *
					//(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));
			}
			else
			{
				flightController->leftEdfSetPoint += (uint16_t)(-1*actuatingRoll);
				//flightController->leftEdfSetPoint += (uint16_t)(-1*actuatingRoll *
				//(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));
			}
		#endif
	}
	else
		flightController->leftEdfSetPoint = FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH;		
	 
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
	/* Wait for EDF controlling until the RC throttle stick gives enough input.
	 * This avoid unwanted EDF running from the INS. */
	if(flightController->rcServoIn->servo3 > FLIGHT_CONTROLLER_EDF_START_OFFSET)
	{
		/* Use the throttle RC input as the main EDF speed */
		flightController->rightEdfSetPoint = FLIGHT_CONTROLLER_EDF_OFFSET + flightController->rcServoIn->servo3;
	
		#ifdef FLIGHT_CONTROLLER_USE_STABILIZATION
			/* Reduce and add the roll PID output to the EDF speed */
			if(actuatingRoll<0)
			{
				flightController->rightEdfSetPoint -= (uint16_t)(-1*actuatingRoll);
				//flightController->rightEdfSetPoint -= (uint16_t)(-1*actuatingRoll *
					//(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));
			}
			else
			{
				flightController->rightEdfSetPoint += (uint16_t)(actuatingRoll);
				//flightController->rightEdfSetPoint += (uint16_t)(actuatingRoll *
				//(FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF/FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF));		
			}
		#endif
	}
	else
		flightController->rightEdfSetPoint = FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH;	
				
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
	/* Elevator Control */
	flightController->leftServoSetPoint = (uint16_t)(FLIGHT_CONTROLLER_RUDDER_MIDDLE_PULSE_WIDTH_CONF -
											(((float)flightController->rcServoIn->servo1-FLIGHT_CONTROLLER_ELEVATOR_MIDDLE_PULSE_WIDTH_CONF)/
											(90.0/FLIGHT_CONTROLLER_ELEVATOR_MAX_ANGLE_CONF)));
	
	/* Rudder Control */
	flightController->leftServoSetPoint += (uint16_t)((((float)flightController->rcServoIn->servo4-FLIGHT_CONTROLLER_RUDDER_MIDDLE_PULSE_WIDTH_CONF)/
											(90.0/FLIGHT_CONTROLLER_RUDDER_MAX_ANGLE_CONF)));
											
	/* Limit the servo output. This should never happen during a normal flight! Otherwise limit the MAX_ANGLE_CONF!
	 * Note: The servo ouput is a sum of rudder and elevator control! */
	constrain(flightController->leftServoSetPoint,FLIGHT_CONTROLLER_MIN_SERVO_OUTPUT,FLIGHT_CONTROLLER_MAX_SERVO_OUTPUT);
	
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
	
	/* Elevator Control */
	flightController->rightServoSetPoint = (uint16_t)(FLIGHT_CONTROLLER_RUDDER_MIDDLE_PULSE_WIDTH_CONF -
											(((float)flightController->rcServoIn->servo1-FLIGHT_CONTROLLER_ELEVATOR_MIDDLE_PULSE_WIDTH_CONF)/
											(90.0/FLIGHT_CONTROLLER_ELEVATOR_MAX_ANGLE_CONF)));
	
	/* Rudder Control */
	flightController->rightServoSetPoint -= (uint16_t)((((float)flightController->rcServoIn->servo4-FLIGHT_CONTROLLER_RUDDER_MIDDLE_PULSE_WIDTH_CONF)/
											(90.0/FLIGHT_CONTROLLER_RUDDER_MAX_ANGLE_CONF)));
											
	/* Limit the servo output. This should never happen during a normal flight! Otherwise limit the MAX_ANGLE_CONF!
	 * Note: The servo ouput is a sum of rudder and elevator control! */
	constrain(flightController->rightServoSetPoint,FLIGHT_CONTROLLER_MIN_SERVO_OUTPUT,FLIGHT_CONTROLLER_MAX_SERVO_OUTPUT);
	
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
	/* Minimum PWM Output */
	flightController->rearEdfSetPoint = FLIGHT_CONTROLLER_EDF_REAR_START_OFFSET;

	/* Only control the EDF if we get enough throttle from the RC */
	if(flightController->rcServoIn->servo3 >= FLIGHT_CONTROLLER_EDF_REAR_START_OFFSET)
	{
		/* Proportionalwe Anteil vom RC Aileron  */
		//flightController->rearEdfSetPoint = (uint16_t)((float)(flightController->rcServoIn->servo3)*FLIGHT_CONTROLLER_PITCH_PROPORTIONAL_RC_AILERON);
		
		flightController->rearEdfSetPoint += 370;
		#ifdef FLIGHT_CONTROLLER_USE_STABILIZATION		
		/* Reduce and add the pitch PID output to the rear EDF speed */		
			if(actuatingPitch<0)
			{
				flightController->rearEdfSetPoint -= (uint16_t)(-1*actuatingPitch);
			}
			else
			{
				flightController->rearEdfSetPoint += (uint16_t)(actuatingPitch);
			}
		#endif	
	}
	
	/* Limit the EDF output*/
	constrain(flightController->rearEdfSetPoint,FLIGHT_CONTROLLER_EDF_REAR_START_OFFSET,FLIGHT_CONTROLLER_MAX_SERVO_OUTPUT);
			
	return SYSTEM_INFO_TRUE;	
}

/**************************************************************************
* \brief Flight Controller Update roll PID controller
*
* Call this method to update the setting of the roll PID controller.
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
* \param i_limit I maximum value
***************************************************************************/
void flight_controller_update_pid_roll_coefficients(float p_factor, float i_factor, float d_factor, float i_limit)
{
	pid_update_tuning_constants(p_factor, i_factor, d_factor, i_limit, &rollPid);
}

/**************************************************************************
* \brief Flight Controller Update pitch PID controller
*
* Call this method to update the setting of the pitch PID controller.
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
* \param i_limit I maximum value
***************************************************************************/
void flight_controller_update_pid_pitch_coefficients(float p_factor, float i_factor, float d_factor, float i_limit)
{
	pid_update_tuning_constants(p_factor, i_factor, d_factor, i_limit, &pitchPid);
}

/**************************************************************************
* \brief Flight Controller Update yaw PID controller
*
* Call this method to update the setting of the yaw PID controller.
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
* \param i_limit I maximum value
***************************************************************************/
void flight_controller_update_pid_yaw_coefficients(float p_factor, float i_factor, float d_factor, float i_limit)
{
	// TODO
}

/**************************************************************************
* \brief Returns the actuating roll value
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
* \brief Returns the actuating pitch value
*
*  Returns the actuating pitch angle in degrees. This is the value calculated
*  by the PID controller.
*
* \param -
*
* \return  actuating pitch angle
***************************************************************************/
float flight_controller_get_actuating_pitch_angle(void)
{
	return actuatingPitch;
}

/**************************************************************************
* \brief Flight Controller Get Actuating Yaw
*
*  Returns the actuating yaw angle in degrees. This is the value calculated
*  by the PID controller.
*
* \param -
*
* \return  actuating yaw angle
***************************************************************************/
float flight_controller_get_actuating_yaw_angle(void)
{
	return actuatingYaw;
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
* \brief Flight Controller Get Actuating Pitch
*
*  Returns the estimated pitch angle in degrees.
*
* \param -
*
* \return  actuating pitch angle
***************************************************************************/
float flight_controller_get_sensor_pitch_angle(void)
{
	return actualSensorData->pitch;
}

/**************************************************************************
* \brief Flight Controller Get Actuating Yaw
*
*  Returns the estimated yaw angle in degrees.
*
* \param -
*
* \return  actuating yaw angle
***************************************************************************/
float flight_controller_get_sensor_yaw_angle(void)
{
	return actualSensorData->yaw;
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
* \brief Flight Controller Get Set Pitch Angle
*
*  Returns the set pitch angle in degrees. The angle is calculated from the
*  servo input signals.
*
* \param -
*
* \return  set pitch angle
***************************************************************************/
float flight_controller_get_set_pitch_angle(void)
{
	return pitchSetValue;
}

/**************************************************************************
* \brief Flight Controller Get Set Yaw Angle
*
*  Returns the set yaw angle in degrees. The angle is calculated from the
*  servo input signals.
*
* \param -
*
* \return  set yaw angle
***************************************************************************/
float flight_controller_get_set_yaw_angle(void)
{
	return yawSetValue;
}

/**************************************************************************
* \\brief Returns the P-Factor for the roll PID controller.
*
* \\return The P-Factor.
***************************************************************************/
float flight_controller_get_pid_roll_p_factor(void)
{
	return rollPid.P_Factor;
}

/**************************************************************************
* \\brief Returns the I-Factor for the roll PID controller.
*
* \\return The I-Factor.
***************************************************************************/
float flight_controller_get_pid_roll_i_factor(void)
{
	return rollPid.I_Factor;
}

/**************************************************************************
* \\brief Returns the D-Factor for the roll PID controller.
*
* \\return The D-Factor.
***************************************************************************/
float flight_controller_get_pid_roll_d_factor(void)
{
	return rollPid.D_Factor;
}

/**************************************************************************
* \\brief Returns the I-Limit for the roll PID controller.
*
* \\return The I-Limit.
***************************************************************************/
float flight_controller_get_pid_roll_i_limit(void)
{
	return rollPid.I_Limit;
}

/**************************************************************************
* \\brief Returns the P-Factor for the pitch PID controller.
*
* \\return The P-Factor.
***************************************************************************/
float flight_controller_get_pid_pitch_p_factor(void)
{
	return pitchPid.P_Factor;
}

/**************************************************************************
* \\brief Returns the I-Factor for the pitch PID controller.
*
* \\return The I-Factor.
***************************************************************************/
float flight_controller_get_pid_pitch_i_factor(void)
{
	return pitchPid.I_Factor;
}

/**************************************************************************
* \\brief Returns the D-Factor for the pitch PID controller.
*
* \\return The D-Factor.
***************************************************************************/
float flight_controller_get_pid_pitch_d_factor(void)
{
	return pitchPid.D_Factor;
}

/**************************************************************************
* \\brief Returns the I-Limit for the pitch PID controller.
*
* \\return The I-Limit.
***************************************************************************/
float flight_controller_get_pid_pitch_i_limit(void)
{
	return pitchPid.I_Limit;
}

/**************************************************************************
* \\brief Returns the P-Factor for the yaw PID controller.
*
* \\return The P-Factor.
***************************************************************************/
float flight_controller_get_pid_yaw_p_factor(void)
{
	return yawPid.P_Factor;
}

/**************************************************************************
* \\brief Returns the I-Factor for the yaw PID controller.
*
* \\return The I-Factor.
***************************************************************************/
float flight_controller_get_pid_yaw_i_factor(void)
{
	return yawPid.I_Factor;
}

/**************************************************************************
* \\brief Returns the D-Factor for the yaw PID controller.
*
* \\return The D-Factor.
***************************************************************************/
float flight_controller_get_pid_yaw_d_factor(void)
{
	return yawPid.D_Factor;
}

/**************************************************************************
* \\brief Returns the I-Limit for the yaw PID controller.
*
* \\return The I-Limit.
***************************************************************************/
float flight_controller_get_pid_yaw_i_limit(void)
{
	return yawPid.I_Limit;
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
		flight_controller_calc_pitch(flightController);
		
		/* Reset PID Controller if we get no input signal */
		if(flightController->rcServoIn->servo3 <= 1200)
		{
			pid_Reset_Integrator(&rollPid);			
		}
			
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