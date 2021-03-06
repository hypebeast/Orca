/*
 * serial_flash.h
 *
 * Created: 05.10.2012 15:14:14
 *  Author: f-ruml
 */ 


#ifndef SERIAL_FLASH_H_
#define SERIAL_FLASH_H_

#include "sysclk.h"

//---------------------------------------------------------------------
//	Serial Flash Settings
//---------------------------------------------------------------------
#define SERIAL_FLASH_SETTINGS_SECTOR				0x00002		/*!< \brief The sector used to store the flash settings */

//---------------------------------------------------------------------
//	General Makros
//---------------------------------------------------------------------
#define SERIAL_FLASH_EXECUTE_FACTORY_RESET			0xff		/*!< \brief After the next restart a factory reset will be executed */
#define SERIAL_FLASH_DISABLE_FACTORY_RESET			0x00		/*!< \brief After the next restart no factory reset will be executed */


/*! Orca flash settings struct
 *  Holds all Orca settings */
typedef struct ORCA_FLASH_SETTINGS
{
	uint8_t factoryReset;		/*!< \brief If != 0x00, will cause an factory reset */
	/* Filter Settings */
	float Q_angle;				/*!< brief Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */
	float Q_gyro;				/*!< brief Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */
	float R_angle;				/*!< brief Startup covariance, our observation noise from the accelerometer. */
	
	float Kp_rollPitch;			/*!< brief DCM Kp factor for the roll and pitch axis*/
	float Ki_rollPitch;			/*!< brief DCM Ki factor for the roll and pitch axis*/
	float Kp_yaw;				/*!< brief DCM Kp factor for the yaw axis*/
	float Ki_yaw;				/*!< brief DCM Ki factor for the yaw axis*/
	
	/* PID controller settings */
	float pid_roll_p_factor;	/*!< brief Startup p factor for the roll PID controller. */
	float pid_roll_i_factor;	/*!< brief Startup i factor for the roll PID controller. */
	float pid_roll_d_factor;	/*!< brief Startup d factor for the roll PID controller. */
	float pid_roll_i_limit;		/*!< brief Startup i limit value for the roll PID controller. */
	float pid_pitch_p_factor;	/*!< brief Startup p factor for the roll PID controller. */
	float pid_pitch_i_factor;	/*!< brief Startup i factor for the roll PID controller. */
	float pid_pitch_d_factor;	/*!< brief Startup d factor for the roll PID controller. */
	float pid_pitch_i_limit;	/*!< brief Startup i limit value for the roll PID controller. */
	float pid_yaw_p_factor;		/*!< brief Startup p factor for the yaw PID controller. */
	float pid_yaw_i_factor;		/*!< brief Startup i factor for the yaw PID controller. */
	float pid_yaw_d_factor;		/*!< brief Startup d factor for the yaw PID controller. */
	float pid_yaw_i_limit;		/*!< brief Startup i limit value for the yaw PID controller. */
} ORCA_FLASH_SETTINGS_t;


uint16_t serial_flash_init(void);
uint16_t serial_flash_init_settings(ORCA_FLASH_SETTINGS_t *data);
uint16_t serial_flash_factory_reset(void);
uint16_t serial_flash_read_settings(void);
uint16_t serial_flash_write_factory_settings(void);
void serial_flash_save_settings(void);
uint16_t serial_flash_save_settings_to_flash(void);

#endif /* SERIAL_FLASH_H_ */