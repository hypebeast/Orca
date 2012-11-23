/*
 * filters.h
 *
 * Created: 23.09.2012 15:23:56
 *  Author: f-ruml
 */ 


#ifndef FILTERS_H_
#define FILTERS_H_

//---------------------------------------------------------------------
//	Roll Filter Startup Settings
//  Edit this settings for different configurations
//---------------------------------------------------------------------
#define FILTER_R_ANGLE_CONF			0.3		/*!< brief 0.69Startup covariance, our observation noise from the accelerometer. */
#define FILTER_Q_ANGLE_CONF			0.003f		/*!< brief 0.0001 Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */
#define FILTER_Q_GYRO_CONF			0.001f		/*!< brief 0.0003 Startup prozess covariance. In this case, it indicates how much we trust the accelerometer relative to the gyros. */

#define PI 3.14159265358979f


/*! Filter struct */
typedef struct FILTER_DATA{	
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
void filter_update_constants(float Q_angle, float Q_gyro, float R_angle);
void filter_task(unsigned long time);
float filter_get_acc_roll(void);
float filter_get_roll_qangle(void);
float filter_get_roll_qgyro(void);
float filter_get_roll_rangle(void);

typedef struct _tilt tilt;

struct _tilt
{
	// Two states, angle and gyro bias. Unbiased angular rate is a byproduct.
	float bias;
	float rate;
	float angle;

	// Covariance of estimation error matrix.
	float P_00;
	float P_01;
	float P_10;
	float P_11;

	// State constants.
	float dt;
	float R_angle;
	float Q_gyro;
	float Q_angle;
};

void tilt_init(tilt *self, float dt, float R_angle, float Q_gyro, float Q_angle);
void tilt_state_update(tilt *self, float gyro_rate);
void tilt_kalman_update(tilt *self, float angle_measured);

inline static float tilt_get_bias(tilt *self)
// Get the bias.
{
	return self->bias;
}

inline static float tilt_get_rate(tilt *self)
// Get the rate.
{
	return self->rate;
}

inline static float tilt_get_angle(tilt *self)
// Get the angle.
{
	return self->angle;
}
#endif /* FILTERS_H_ */