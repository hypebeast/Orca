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


//---------------------------------------------------------------------
//	DCM Settings
//---------------------------------------------------------------------
#define FILTER_KP_ROLLPITCH				0.02		/*!< brief Startup KP setting for roll and pitch calculation */
#define FILTER_KI_ROLLPITCH				0.00002		/*!< brief Startup KI setting for roll and pitch calculation */
#define FILTER_KP_YAW					1.2			/*!< brief Startup KP setting for yaw calculation */
#define FILTER_KI_YAW					0.00002		/*!< brief Startup KI setting for yaw calculation */

#define FILTER_PERFORMANCE_REPORTING	0
#define FILTER_PRINT_DEBUG				0
#define FILTER_USE_MAGNETOMETER			0

#define GRAVITY 248  //this equivalent to 1G in the raw data coming from the accelerometer

/*For debugging purposes*/
//OUTPUTMODE=1 will print the corrected data,
//OUTPUTMODE=0 will print uncorrected data of the gyros (with drift)
#define OUTPUTMODE 1

//---------------------------------------------------------------------
//	Roll Filter Startup Settings
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
	
	float xGyr;				/*!< brief Actual rotation x-axis in degrees per second */
	float yGyr;				/*!< brief Actual rotation y-axis in degrees per second */
	float zGyr;				/*!< brief Actual rotation z-axis in degrees per second */
	
	float pitch;
	float roll;
	float yaw;
	
	float x_angle;			/*!< brief These variables represent our state matrix x */	
	float x_bias;			/*!< brief These variables represent our state matrix x */
	float P_00;				/*!< brief Our error covariance matrix */
	float P_01;				/*!< brief Our error covariance matrix */
	float P_10;				/*!< brief Our error covariance matrix */
	float P_11;				/*!< brief Our error covariance matrix */
	
	/* 
	 * Q is a 2x2 matrix of the covariance. Because we
	 * assuma the gyro and accelero noise to be independend
	 * of eachother, the covariances on the / diagonal are 0.
	 *
	 * Covariance Q, the process noise, from the assumption
	 *    x = F x + B u + w
	 * with w having a normal distribution with covariance Q.
	 * (covariance = E[ (X - E[X])*(X - E[X])' ]
	 * We assume is linair with dt
	 */
	float Q_angle; 
	float Q_gyro;
	/*
	 * Covariance R, our observation noise (from the accelerometer)
	 * Also assumed to be linair with dt
	 */
	float R_angle;
	
	float Kp_rollPitch;
	float Ki_rollPitch;
	float Kp_yaw;
	float Ki_yaw;
	
}FILTER_DATA_t;

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define PI 3.14159265358979f
#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi

void filter_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle);
void filter_update_constants(float Q_angle, float Q_gyro, float R_angle);
void filter_task(unsigned long time);
float filter_get_acc_roll(void);
float filter_get_roll_qangle(void);
float filter_get_roll_qgyro(void);
float filter_get_roll_rangle(void);

#endif /* FILTERS_H_ */