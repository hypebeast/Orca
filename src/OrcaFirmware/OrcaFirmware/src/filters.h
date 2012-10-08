/*
 * filters.h
 *
 * Created: 23.09.2012 15:23:56
 *  Author: f-ruml
 */ 


#ifndef FILTERS_H_
#define FILTERS_H_

//---------------------------------------------------------------------
//	MPU 6000 Startup Settings
//  Edit this settings for different configurations
//---------------------------------------------------------------------
#define FILTER_R_ANGLE_CONF			0.69f		/*!< brief Startup covariance, our observation noise from the accelerometer. */
#define FILTER_Q_ANGLE_CONF			0.0001f		/*!< brief Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */
#define FILTER_Q_GYRO_CONF			0.0003f		/*!< brief Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */

#define PI 3.14159265358979f


/*! Filter struct */
typedef struct FILTER_DATA{	
	float test;
	float xAcc;				/*!< brief Actual acceleration x-axis in g (should be towards the nose of the plane). */
	float yAcc;				/*!< brief Actual acceleration y-axis in g (should be towards the right wingtip). */
	float zAcc;				/*!< brief Actual acceleration z-axis in g (should be towards the ground).*/
	
	float xGyr;				/*!< brief Actual rotation x-axis in degrees per second */
	float yGyr;
	float zGyr;
	
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
	
}FILTER_DATA_t;

void filter_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle);
void filter_task(unsigned long time);

#endif /* FILTERS_H_ */