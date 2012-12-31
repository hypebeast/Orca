/*
 * filters.h
 *
 * Created: 23.09.2012 15:23:56
 *  Author: f-ruml
 */ 


#ifndef FILTERS_H_
#define FILTERS_H_

#include <stdlib.h>

//---------------------------------------------------------------------
//	General Filter Settings
//---------------------------------------------------------------------
#define FILTER_USE_DCM					/*!< brief Use either DCM or Kalman for the sensor fusion */
//#define FILTER_USE_MAGNETOMETER			/*!< brief Use the magnetometer */

//---------------------------------------------------------------------
//	DCM Settings
//---------------------------------------------------------------------
#define FILTER_KP_ROLLPITCH				0.02		/*!< brief Startup KP setting for roll and pitch calculation */
#define FILTER_KI_ROLLPITCH				0.00002		/*!< brief Startup KI setting for roll and pitch calculation */
#define FILTER_KP_YAW					1.2			/*!< brief Startup KP setting for yaw calculation */
#define FILTER_KI_YAW					0.00002		/*!< brief Startup KI setting for yaw calculation */

//---------------------------------------------------------------------
//	Kalman Roll Filter Startup Settings
//  Edit this settings for different configurations
//---------------------------------------------------------------------
#define FILTER_R_ANGLE_CONF			0.001		/*!< brief 0.69Startup covariance, our observation noise from the accelerometer. */
#define FILTER_Q_ANGLE_CONF			0.001f		/*!< brief 0.0001 Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */
#define FILTER_Q_GYRO_CONF			0.007f		/*!< brief 0.0003 Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */

/*! Filter struct */
typedef struct FILTER_DATA{	
	float xAcc;				/*!< brief Actual acceleration x-axis in g (should be towards the nose of the plane). */
	float yAcc;				/*!< brief Actual acceleration y-axis in g (should be towards the right wingtip). */
	float zAcc;				/*!< brief Actual acceleration z-axis in g (should be towards the ground).*/
	
	float xGyr;				/*!< brief Actual rotation x-axis in degrees per seconds */
	float yGyr;				/*!< brief Actual rotation y-axis in degrees per seconds */
	float zGyr;				/*!< brief Actual rotation z-axis in degrees per seconds */
	
	float pitch;			/*!< brief Estimated (current) pitch angle in degrees */
	float roll;				/*!< brief Estimated (current) roll angle in degrees */
	float yaw;				/*!< brief Estimated (current) yaw angle in degrees */
			
	/* 
	 * Q is a 2x2 matrix of the covariance. Because we
	 * assume the gyro and acceleration noise to be independent
	 * of each other, the covariances on the / diagonal are 0.
	 *
	 * Covariance Q, the process noise, from the assumption
	 *    x = F x + B u + w
	 * with w having a normal distribution with covariance Q.
	 * (covariance = E[ (X - E[X])*(X - E[X])' ]
	 * We assume is linear with dt
	 */
	float Q_angle; 
	float Q_gyro;
	/*
	 * Covariance R, our observation noise (from the accelerometer)
	 * Also assumed to be linear with dt
	 */
	float R_angle;			/*!< brief Covariance R */
	
	float Kp_rollPitch;		/*!< brief Kp Roll/Pitch */
	float Ki_rollPitch;		/*!< brief Ki Roll/Pitch */
	float Kp_yaw;			/*!< brief Kp Yaw */
	float Ki_yaw;			/*!< brief Ki Yaw */
	
}FILTER_DATA_t;

#define PI 3.14159265358979f
#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))


#ifdef FILTER_USE_DCM
	void filter_dcm_init(FILTER_DATA_t *filter, float KpRollPitch, float KiRollPitch, float KpYaw, float KiYaw);
#else /* Use Kalman */
	void filter_kalman_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle);
#endif // FILTER_USE_DCM

void filter_kalman_update_constants(float Q_angle, float Q_gyro, float R_angle);
void filter_dcm_update_constants(float KpRollPitch, float KiRollPitch, float KpYaw, float KiYaw);
void filter_task(unsigned long time);
float filter_get_acc_roll(void);
float filter_get_acc_pitch(void);
float filter_get_acc_yaw(void);
float filter_kalman_get_roll_qangle(void);
float filter_kalman_get_roll_qgyro(void);
float filter_kalman_get_roll_rangle(void);
float filter_dcm_get_rollPitch_kp(void);
float filter_dcm_get_rollPitch_ki(void);
float filter_dcm_get_yaw_kp(void);
float filter_dcm_get_yaw_ki(void);
#endif /* FILTERS_H_ */