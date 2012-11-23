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

tilt roll_tilt_state;

void filter_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle)
{
	tilt_init(&roll_tilt_state, 0.02, R_angle, Q_gyro, Q_angle);
	
	filterdata = filter;
	//
	//filterdata->Q_angle = Q_angle;
	//filterdata->Q_gyro  = Q_gyro;
	//filterdata->R_angle = R_angle;
}

void filter_update_constants(float Q_angle, float Q_gyro, float R_angle)
{	
	filterdata->Q_angle = Q_angle;
	filterdata->Q_gyro  = Q_gyro;
	filterdata->R_angle = R_angle;
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
	/*please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	dt = (float)(time - lastMicros) / 1000000;	
	lastMicros = time;						
	
	/* Execute kalman filter */
	//filter_predict_accG_roll();
	//filter_ars_predict();		// Kalman predict
	//filter_ars_update(accRoll);    // Kalman update + result (angle)
	//filterdata->roll = filter_conv_rad_to_degrees(filterdata->x_angle);
	
	// Pass the measured roll values and rates through the Kalman filter to
	// determine the estimated roll values in radians.
	filter_predict_accG_roll();
	tilt_state_update(&roll_tilt_state, filter_conv_degrees_to_rad(filterdata->xGyr));
	tilt_kalman_update(&roll_tilt_state, accRoll);
	filterdata->roll = filter_conv_rad_to_degrees(roll_tilt_state.angle);
}

// Initialize the kalman state.
void tilt_init(tilt *self, float dt, float R_angle, float Q_gyro, float Q_angle)
{
	// Initialize the two states, the angle and the gyro bias.  As a
	// byproduct of computing the angle, we also have an unbiased
	// angular rate available.
	self->bias = 0.0;
	self->rate = 0.0;
	self->angle = 0.0;

	// Initialize the delta in seconds between gyro samples.
	self->dt = dt;

	// Initialize the measurement noise covariance matrix values.
	// In this case, R is a 1x1 matrix tha represents expected
	// jitter from the accelerometer.  Original Autopilot software
	// had this value set to 0.3.
	self->R_angle = R_angle;

	// Initialize the process noise covariance matrix values.
	// In this case, Q indicates how much we trust the acceleromter
	// relative to the gyros.  Original Autopilot software had
	// Q_gyro set to 0.003 and Q_angle set to 0.001.
	self->Q_gyro = Q_gyro;
	self->Q_angle = Q_angle;

	// Initialize covariance of estimate state.  This is updated
	// at every time step to determine how well the sensors are
	// tracking the actual state.
	self->P_00 = 1.0;
	self->P_01 = 0.0;
	self->P_10 = 0.0;
	self->P_11 = 1.0;
}


// tilt_state_update() is called every dt with a biased gyro
// measurement by the user of the module.  It updates the current
// angle and rate estimate.
//
// The pitch gyro measurement should be scaled into real units, but
// does not need any bias removal.  The filter will track the bias.
//
// Our state vector is:
//
//  X = [ angle, gyro_bias ]
//
// It runs the state estimation forward via the state functions:
//
//  Xdot = [ angle_dot, gyro_bias_dot ]
//
//  angle_dot = gyro - gyro_bias
//  gyro_bias_dot = 0
//
// And updates the covariance matrix via the function:
//
//  Pdot = A*P + P*A' + Q
//
// A is the Jacobian of Xdot with respect to the states:
//
//  A = [ d(angle_dot)/d(angle)     d(angle_dot)/d(gyro_bias) ]
//      [ d(gyro_bias_dot)/d(angle) d(gyro_bias_dot)/d(gyro_bias) ]
//
//    = [ 0 -1 ]
//      [ 0  0 ]
//
// Due to the small CPU available on the microcontroller, we've
// hand optimized the C code to only compute the terms that are
// explicitly non-zero, as well as expanded out the matrix math
// to be done in as few steps as possible.  This does make it
// harder to read, debug and extend, but also allows us to do
// this with very little CPU time.
void tilt_state_update(tilt *self, float gyro_rate)
{
	// Static so these are kept off the stack.
	static float gyro_rate_unbiased;
	static float Pdot_00;
	static float Pdot_01;
	static float Pdot_10;
	static float Pdot_11;

	// Unbias our gyro.
	gyro_rate_unbiased = gyro_rate - self->bias;

	// Compute the derivative of the covariance matrix
	//
	// Pdot = A*P + P*A' + Q
	//
	// We've hand computed the expansion of A = [ 0 -1, 0 0 ] multiplied
	// by P and P multiplied by A' = [ 0 0, -1, 0 ].  This is then added
	// to the diagonal elements of Q, which are Q_angle and Q_gyro.
	Pdot_00 = self->Q_angle - self->P_01 - self->P_10;
	Pdot_01 = -self->P_11;
	Pdot_10 = -self->P_11;
	Pdot_11 = self->Q_gyro;

	// Store our unbiased gyro estimate.
	self->rate = gyro_rate_unbiased;

	// Update the angle estimate.
	//
	// angle += angle_dot * dt
	//       += (gyro - gyro_bias) * dt
	//       += q * dt
	//
	self->angle += gyro_rate_unbiased * self->dt;

	// Update the covariance matrix.
	self->P_00 += Pdot_00 * self->dt;
	self->P_01 += Pdot_01 * self->dt;
	self->P_10 += Pdot_10 * self->dt;
	self->P_11 += Pdot_11 * self->dt;
}

void tilt_kalman_update(tilt *self, float angle_measured)
// tilt_kalman_update() is called by a user of the module when a
// new accelerometer angle measurement is available.
//
// This does not need to be called every time step, but can be if
// the accelerometer data are available at the same rate as the
// rate gyro measurement.
//
// As commented in state_update, the math here is simplified to
// make it possible to execute on a small microcontroller with no
// floating point unit.  It will be hard to read the actual code and
// see what is happening.
//
// The C matrix is a 1x2 (measurements x states) matrix that
// is the Jacobian matrix of the measurement value with respect
// to the states.  In this case, C is:
//
//  C = [ d(angle_measured)/d(angle)  d(angle_measured)/d(gyro_bias) ]
//    = [ 1 0 ]
//
// because the angle measurement directly corresponds to the angle
// estimate and the angle measurement has no relation to the gyro
// bias.
{
	// Static so these are kept off the stack.
	static float angle_error;
	static float C_0;
	static float PCt_0;
	static float PCt_1;
	static float E;
	static float K_0;
	static float K_1;
	static float t_0;
	static float t_1;

	// Compute the error in the estimate.
	angle_error = angle_measured - self->angle;

	// C_0 shows how the state measurement directly relates to
	// the state estimate.
	//
	// The C_1 shows that the state measurement does not relate
	// to the gyro bias estimate.  We don't actually use this, so
	// we comment it out.
	C_0 = 1.0;
	// C_1 = 0.0;

	// PCt<2,1> = P<2,2> * C'<2,1>, which we use twice.  This makes
	// it worthwhile to precompute and store the two values.
	// Note that C[0,1] = C_1 is zero, so we do not compute that term.
	PCt_0 = C_0 * self->P_00; /* + C_1 * P_01 = 0 */
	PCt_1 = C_0 * self->P_10; /* + C_1 * P_11 = 0 */

	// Compute the error estimate.  From the Kalman filter paper:
	//
	// E = C P C' + R
	//
	// Dimensionally,
	//
	//  E<1,1> = C<1,2> P<2,2> C'<2,1> + R<1,1>
	//
	// Again, note that C_1 is zero, so we do not compute the term.
	E = self->R_angle + C_0 * PCt_0; /* + C_1 * PCt_1 = 0 */

	// Compute the Kalman filter gains.  From the Kalman paper:
	//
	// K = P C' inv(E)
	//
	// Dimensionally:
	//
	// K<2,1> = P<2,2> C'<2,1> inv(E)<1,1>
	//
	// Luckilly, E is <1,1>, so the inverse of E is just 1/E.
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	//
	// Update covariance matrix.  Again, from the Kalman filter paper:
	//
	// P = P - K C P
	//
	// Dimensionally:
	//
	// P<2,2> -= K<2,1> C<1,2> P<2,2>
	//
	// We first compute t<1,2> = C P.  Note that:
	//
	// t[0,0] = C[0,0] * P[0,0] + C[0,1] * P[1,0]
	//
	// But, since C_1 is zero, we have:
	//
	// t[0,0] = C[0,0] * P[0,0] = PCt[0,0]
	//
	// This saves us a floating point multiply.
	t_0 = PCt_0; /* C_0 * P[0][0] + C_1 * P[1][0] */
	t_1 = C_0 * self->P_01; /* + C_1 * P[1][1]  = 0 */

	self->P_00 -= K_0 * t_0;
	self->P_01 -= K_0 * t_1;
	self->P_10 -= K_1 * t_0;
	self->P_11 -= K_1 * t_1;

	//
	// Update our state estimate.  Again, from the Kalman paper:
	//
	// X += K * err
	//
	// And, dimensionally,
	//
	// X<2> = X<2> + K<2,1> * err<1,1>
	//
	// err is a measurement of the difference in the measured state
	// and the estimate state.  In our case, it is just the difference
	// between the two accelerometer measured angle and our estimated
	// angle.
	self->bias  += K_1 * angle_error;
	self->angle += K_0 * angle_error;
}


