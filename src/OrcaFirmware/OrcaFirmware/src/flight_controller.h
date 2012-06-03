/*
 * flight_controller.h
 *
 * Created: 22.04.2012 23:39:30
 *  Author: sruml
 */ 


#ifndef FLIGHT_CONTROLLER_H_
#define FLIGHT_CONTROLLER_H_

#include "servo_in.h"

//---------------------------------------------------------------------
//	Supported Flight Controller Modes
//---------------------------------------------------------------------
#define FLIGHT_CONTROLLER_MODE_RC		(0x01<<0)		/*!< brief External RC controlled */

#define FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH 750 // in ticks
#define FLIGHT_CONTROLLER_SERVO_UPPER_PULSE_WIDTH 2250 // in ticks
#define FLIGHT_CONTROLLER_SERVO_MIDDLE_PULSE_WIDTH (FLIGHT_CONTROLLER_SERVO_UPPER_PULSE_WIDTH-FLIGHT_CONTROLLER_SERVO_LOWER_PULSE_WIDTH) // in ticks

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


void flight_controller_init(BOARD_CONFIG_t *board, SERVO_IN_t *servo, FLIGHT_CONTROLLER_t *flightController);
int flight_controller_convert_rudder_in(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_convert_throttle_in(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_convert_aileron_in(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_convert_pitch_in(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_convert_elevator_in(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_left_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_left_servo(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_right_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_right_servo(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_calc_rear_edf(FLIGHT_CONTROLLER_t *flightController);
int flight_controller_task(FLIGHT_CONTROLLER_t *flightController);

#endif /* FLIGHT_CONTROLLER_H_ */