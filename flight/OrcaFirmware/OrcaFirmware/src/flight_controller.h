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
//	Factory Settings for the PID Controller
//  Edit this settings for different configurations
//---------------------------------------------------------------------
#define PID_ROLL_P_FACTOR_CONF			0.3f	/*!< brief Default p factor for the roll PID controller. */
#define PID_ROLL_I_FACTOR_CONF			0.02f		/*!< brief Default p factor for the roll PID controller. */
#define PID_ROLL_D_FACTOR_CONF			0.08f		/*!< brief Default p factor for the roll PID controller. */
#define PID_ROLL_I_LIMIT_CONF			1.0f		/*!< brief Default I limit value. */
#define PID_PITCH_P_FACTOR_CONF			10.0f		/*!< brief Default p factor for the pitch PID controller. */
#define PID_PITCH_I_FACTOR_CONF			0.02f		/*!< brief Default p factor for the pitch PID controller. */
#define PID_PITCH_D_FACTOR_CONF			0.00f		/*!< brief Default p factor for the pitch PID controller. */
#define PID_PITCH_I_LIMIT_CONF			8.0f		/*!< brief Default I limit value. */
#define PID_YAW_P_FACTOR_CONF			0.05f		/*!< brief Default p factor for the yaw PID controller. */
#define PID_YAW_I_FACTOR_CONF			0.002f		/*!< brief Default p factor for the yaw PID controller. */
#define PID_YAW_D_FACTOR_CONF			0.00f		/*!< brief Default p factor for the yaw PID controller. */
#define PID_YAW_I_LIMIT_CONF			5.0f		/*!< brief Default I limit value. */

//---------------------------------------------------------------------
//	General Servo Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH		1000	// in ticks
#define FLIGHT_CONTROLLER_SERVO_UPPER_PULSE_WIDTH		2000	// in ticks
#define FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH		1500	// in ticks

//---------------------------------------------------------------------
//	Front EDF Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_EDF_START_OFFSET				1200	/*!< brief Don't start EDF controlling until this offset from RC is reached */
#define FLIGHT_CONTROLLER_EDF_OFFSET					300		/*!< brief Additions-Offset für die EDF's links und Rechts */
#define FLIGHT_CONTROLLER_EDF_Factor					0.5f	/*!< brief External RC controlled */
#define FLIGHT_CONTROLLER_AILERON_FACTOR				0.1f	/*!< brief Factor für die EDF Subtraktion für die EDF's */

//---------------------------------------------------------------------
//	Rear EDF Definitions
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_EDF_REAR_OFFSET				FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH		/*!< brief Additions-Offset für rear EDF */
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
#define FLIGHT_CONTROLLER_AILERON_DELTA_VALUE_CONF			(FLIGHT_CONTROLLER_AILERON_MAX_VALUE_CONF-FLIGHT_CONTROLLER_AILERON_MIN_VALUE_CONF)/*!< brief Maximaler Roll Winkel über die Fernsteuerung */

#define FLIGHT_CONTROLLER_PITCH_MAX_ANGLE_CONF				10.0f	/*!< brief Maximaler Pitch Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_PITCH_PROPORTIONAL_RC_AILERON		0.85f	/*!< brief Maximaler Pitch Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_ELEVATOR_MIN_VALUE_CONF			1128	/*!< brief Maximaler Roll Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_ELEVATOR_MAX_VALUE_CONF			1944	/*!< brief Maximaler Roll Winkel über die Fernsteuerung */
#define FLIGHT_CONTROLLER_ELEVATOR_DELTA_VALUE_CONF			(FLIGHT_CONTROLLER_ELEVATOR_MAX_VALUE_CONF-FLIGHT_CONTROLLER_ELEVATOR_MIN_VALUE_CONF)

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
void flight_controller_calc_roll(FLIGHT_CONTROLLER_t *flightController);
void flight_controller_update_pid_roll_coefficients(float p_factor, float i_factor, float d_factor, float i_limit);
void flight_controller_update_pid_pitch_coefficients(float p_factor, float i_factor, float d_factor, float i_limit);
void flight_controller_update_pid_yaw_coefficients(float p_factor, float i_factor, float d_factor, float i_limit);
float flight_controller_get_actuating_roll_angle(void);
float flight_controller_get_actuating_pitch_angle(void);
float flight_controller_get_set_roll_angle(void);
float flight_controller_get_sensor_roll_angle(void);
float flight_controller_get_pid_roll_p_factor(void);
float flight_controller_get_pid_roll_i_factor(void);
float flight_controller_get_pid_roll_d_factor(void);
float flight_controller_get_pid_roll_i_limit(void);
float flight_controller_get_pid_pitch_p_factor(void);
float flight_controller_get_pid_pitch_i_factor(void);
float flight_controller_get_pid_pitch_d_factor(void);
float flight_controller_get_pid_pitch_i_limit(void);
float flight_controller_get_pid_yaw_p_factor(void);
float flight_controller_get_pid_yaw_i_factor(void);
float flight_controller_get_pid_yaw_d_factor(void);
float flight_controller_get_pid_yaw_i_limit(void);
float flight_controller_get_actuating_pitch_angle(void);
float flight_controller_get_actuating_yaw_angle(void);
float flight_controller_get_sensor_pitch_angle(void);
float flight_controller_get_set_pitch_angle(void);
float flight_controller_get_set_yaw_angle(void);
float flight_controller_get_sensor_yaw_angle(void);
#endif /* FLIGHT_CONTROLLER_H_ */