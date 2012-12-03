/*
 * filters.c
 *
 * Created: 23.09.2012 15:23:46
 *  Author: f-ruml
 */ 

#include "sysclk.h"

/*To add the "math.h" go to the Project properties under "Toolchain" look to the "AVR/GNU C Linker" the second subcategory there is "Libraries".
In the top of the two boxes make an entry for "libm.a" */
#include "math.h" 
#include "filters.h"

FILTER_DATA_t *filterdata;
float dt = 0;
float accRoll = 0;
unsigned long lastMicros = 0;

static float filter_ars_update(const float angle_m);
static float filter_predict_accG_roll(void);
static void filter_ars_predict(void);
static float filter_conv_degrees_to_rad(float degrees);
static float filter_conv_rad_to_degrees(float degrees);


void filter_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle)
{	
	filterdata = filter;

	filterdata->Q_angle = Q_angle;
	filterdata->Q_gyro  = Q_gyro;
	filterdata->R_angle = R_angle;
}

void filter_update_constants(float Q_angle, float Q_gyro, float R_angle)
{	
	filterdata->Q_angle = Q_angle;
	filterdata->Q_gyro  = Q_gyro;
	filterdata->R_angle = R_angle;
}

float filter_get_roll_qangle(void)
{
	return filterdata->Q_angle;
}

float filter_get_roll_qgyro(void)
{
	return filterdata->Q_gyro;
}

float filter_get_roll_rangle(void)
{
	return filterdata->R_angle;
}

/*
 * The predict function. Updates 2 variables:
 * our model-state x and the 2x2 matrix P
 *     
 * x = [ angle, bias ]' 
 * 
 *   = F x + B u
 *
 *   = [ 1 -dt, 0 1 ] [ angle, bias ] + [ dt, 0 ] [ dotAngle 0 ]
 *
 *   => angle = angle + dt (dotAngle - bias)
 *      bias  = bias
 *
 *
 * P = F P transpose(F) + Q
 *
 *   = [ 1 -dt, 0 1 ] * P * [ 1 0, -dt 1 ] + Q
 *
 *  P(0,0) = P(0,0) - dt * ( P(1,0) + P(0,1) ) + dt² * P(1,1) + Q(0,0)
 *  P(0,1) = P(0,1) - dt * P(1,1) + Q(0,1)
 *  P(1,0) = P(1,0) - dt * P(1,1) + Q(1,0)
 *  P(1,1) = P(1,1) + Q(1,1)
 *
 *
 */
static void filter_ars_predict(void)
{
	filterdata->x_angle += dt * (filter_conv_degrees_to_rad(filterdata->xGyr) - filterdata->x_bias);

	filterdata->P_00 +=  - dt * (filterdata->P_10 + filterdata->P_01) + filterdata->Q_angle * dt;
	filterdata->P_01 +=  - dt * filterdata->P_11;
	filterdata->P_10 +=  - dt * filterdata->P_11;
	filterdata->P_11 +=  + filterdata->Q_gyro * dt;
}

/*
 *  The update function updates our model using 
 *  the information from a 2nd measurement.
 *  Input angle_m is the angle measured by the accelerometer.
 *
 *  y = z - H x
 *
 *  S = H P transpose(H) + R
 *    = [ 1 0 ] P [ 1, 0 ] + R
 *    = P(0,0) + R
 * 
 *  K = P transpose(H) S^-1
 *    = [ P(0,0), P(1,0) ] / S
 *
 *  x = x + K y
 *
 *  P = (I - K H) P
 *
 *    = ( [ 1 0,    [ K(0),
 *          0 1 ] -   K(1) ] * [ 1 0 ] ) P
 *
 *    = [ P(0,0)-P(0,0)*K(0)  P(0,1)-P(0,1)*K(0),
 *        P(1,0)-P(0,0)*K(1)  P(1,1)-P(0,1)*K(1) ]
 */
static float filter_ars_update(const float angle_m)
{
	const float y = angle_m - filterdata->x_angle;
	
	const float S = filterdata->P_00 + filterdata->R_angle;
	const float K_0 = filterdata->P_00 / S;
	const float K_1 = filterdata->P_10 / S;
	
	filterdata->x_angle +=  K_0 * y;
	filterdata->x_bias  +=  K_1 * y;
	
	filterdata->P_00 -= K_0 * filterdata->P_00;
	filterdata->P_01 -= K_0 * filterdata->P_01;
	filterdata->P_10 -= K_1 * filterdata->P_00;
	filterdata->P_11 -= K_1 * filterdata->P_01;
	
	return filterdata->x_angle;
}

static float filter_predict_accG_roll(void)
{
	accRoll = -(atan2(-(filterdata->zAcc), filterdata->yAcc)-(PI/2.0));
}

static float filter_conv_degrees_to_rad(float degrees)
{
	return degrees/180*PI;
}

static float filter_conv_rad_to_degrees(float degrees)
{
	return degrees*180/PI;
}

float filter_get_acc_roll(void)
{
	return filter_conv_rad_to_degrees(accRoll);
}

void filter_task(unsigned long time)
{
	/* Calculate time elapsed since last call (dt) */
	/* Please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	dt = (float)(time - lastMicros) / 1000000;	
	lastMicros = time;						
	
	/* Execute kalman filter */
	filter_predict_accG_roll();
	filter_ars_predict();		// Kalman predict
	filter_ars_update(accRoll);    // Kalman update + result (angle)
	filterdata->roll = filter_conv_rad_to_degrees(filterdata->x_angle);
}



