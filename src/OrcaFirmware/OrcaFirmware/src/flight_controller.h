/*
 * flight_controller.h
 *
 * Created: 22.04.2012 23:39:30
 *  Author: sruml
 */ 


#ifndef FLIGHT_CONTROLLER_H_
#define FLIGHT_CONTROLLER_H_

#include "servo_in.h"
#include "filters.h"
#include "serial_flash.h"

//---------------------------------------------------------------------
//	Supported Flight Controller Modes
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_MODE_RC		(0x01<<0)		/*!< brief External RC controlled */
#define FLIGHT_CONTROLLER_MODE_AUTOMATIC (0x01<<1)		/*!< brief Automatic mode */

//---------------------------------------------------------------------
//	Roll Filter Startup Settings
//  Edit this settings for different configurations
//---------------------------------------------------------------------
#define PID_ROLL_P_FACTOR_CONF			5		/*!< brief Default p factor for the roll PID controller. */
#define PID_ROLL_I_FACTOR_CONF			2		/*!< brief Default p factor for the roll PID controller. */
#define PID_ROLL_D_FACTOR_CONF			2		/*!< brief Default p factor for the roll PID controller. */

//---------------------------------------------------------------------
//	General Servo Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH		1000	// in ticks
#define FLIGHT_CONTROLLER_SERVO_UPPER_PULSE_WIDTH		2000	// in ticks
#define FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH		1500	// in ticks

//---------------------------------------------------------------------
//	Front EDF Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_EDF_OFFSET					300		/*!< brief Additions-Offset für die EDF's links und Rechts */
#define FLIGHT_CONTROLLER_EDF_Factor					0.5f	/*!< brief External RC controlled */
#define FLIGHT_CONTROLLER_AILERON_FACTOR				0.1f	/*!< brief Factor für die EDF Subtraktion für die EDF's */

//---------------------------------------------------------------------
//	Rear EDF Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_EDF_REAR_OFFSET				000		/*!< brief Additions-Offset für rear EDF */
#define FLIGHT_CONTROLLER_EDF_REAR_START_OFFSET			1260

//---------------------------------------------------------------------
//	Servo Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_SERVO_MAX_WAY_FACTOR			1.7f	/*!< brief Aussteuerweg für Servo links und rechts durch Elevator */
#define FLIGHT_CONTROLLER_RUDDER_FACTOR					1.0f	/*!< brief Factor für die Rudder Addition für Servo links und rechts */

//---------------------------------------------------------------------
//	Supported Flight Controller Modes
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_ROLL_MAX_ANGLE_CONF				20.0f	/*!< brief Maximaler Roll Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_AILERON_MIN_VALUE_CONF			1130	/*!< brief Maximaler Roll Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_AILERON_MAX_VALUE_CONF			1948	/*!< brief Maximaler Roll Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF			(FLIGHT_CONTROLLER_AILERON_MAX_VALUE_CONF -	FLIGHT_CONTROLLER_AILERON_MIN_VALUE_CONF)/*!< brief Maximaler Roll Winkel über die Fernsteuerung */

/*! Flight Controller Struct */
typedef struct FLIGHT_CONTROLLER {
	volatile uint16_t mode;					/*!< brief Selected flight controller mode */	
	SERVO_IN_t *rcServoIn;					/*!< brief RC Servo Input Struct */
	uint16_t leftEdfSetPoint;				/*!< brief Setpoint for the left EDF */
	uint16_t leftServoSetPoint;				/*!< brief Setpoint for the left Servo */
	uint16_t rightEdfSetPoint;				/*!< brief Setpoint for the right EDF */
	uint16_t rightServoSetPoint;			/*!< brief Setpoint for the right Servo */
	uint16_t rearEdfSetPoint;				/*!< brief Setpoint for the rear EDF */
}FLIGHT_CONTROLLER_t;


void flight_controller_init(BOARD_CONFIG_t *board,ORCA_FLASH_SETTINGS_t *settings, SERVO_IN_t *servo, FILTER_DATA_t *filter, FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_left_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_left_servo(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_right_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_right_servo(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_rear_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_task(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_roll(FLIGHT_CONTROLLER_t *flightController);
void flight_controller_update_pid_controller(int16_t p_factor, int16_t i_factor, int16_t d_factor);
#endif /* FLIGHT_CONTROLLER_H_ */