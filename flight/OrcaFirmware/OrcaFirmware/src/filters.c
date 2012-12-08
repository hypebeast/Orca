/*
 * filters.c
 *
 * Created: 23.09.2012 15:23:46
 *  Author: f-ruml
 *
 * The DCM code is based on the ardu-imu http://code.google.com/p/ardu-imu/
 */ 

#include "sysclk.h"

/*To add the "math.h" go to the Project properties under "Toolchain" look to the "AVR/GNU C Linker" 
the second subcategory there is "Libraries".
In the top of the two boxes make an entry for "libm.a" */
#include "math.h" 
#include "filters.h"
#include "vector.h"
#include "calculations.h"


FILTER_DATA_t *filterdata;
float dt = 0;
float accRoll = 0;
unsigned long lastMicros = 0;

#ifdef FILTER_USE_DCM
	float Accel_Vector[3] =
	{ 0, 0, 0 }; //Store the acceleration in a vector
	float Gyro_Vector[3] =
	{ 0, 0, 0 };//Store the gyros turn rate in a vector
	float Omega_Vector[3] =
	{ 0, 0, 0 }; //Corrected Gyro_Vector data
	float Omega_P[3] =
	{ 0, 0, 0 };//Omega Proportional correction
	float Omega_I[3] =
	{ 0, 0, 0 };//Omega Integrator
	float Omega[3] =
	{ 0, 0, 0 };
		
	float errorRollPitch[3] =
	{ 0, 0, 0 };
	float errorYaw[3] =
	{ 0, 0, 0 };

	static float DCM_Matrix[3][3] =
	{
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 } };
	static float Update_Matrix[3][3] =
	{
	{ 0, 1, 2 },
	{ 3, 4, 5 },
	{ 6, 7, 8 } }; //Gyros here	
	static float Temporary_Matrix[3][3] =
	{
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	{ 0, 0, 0 } };
#endif // FILTER_USE_DCM

static float filter_ars_update(const float angle_m);
static float filter_predict_accG_roll(void);
static void filter_ars_predict(void);
static float filter_conv_degrees_to_rad(float degrees);
static float filter_conv_rad_to_degrees(float degrees);
/*----------------------------------DCM Stuff-----------------------------------------*/
static void Matrix_Multiply(float a[3][3], float b[3][3],float mat[3][3]);
static void Normalize(void);
static void Drift_correction(void);
static void Accel_adjust(void);
static void Matrix_update(void);
static void Euler_angles(void);

/**************************************************************************
* \brief Filter Init
*	This function initialize the filter subsystem. /n
*
* \param *filter filter struct
* \param Q_angle kalman q angle setting
* \param q_gyro kalman q gyro setting
* \param r_angle kalman r angle setting
*
* \return  ---
***************************************************************************/
void filter_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle)
{	
	filterdata = filter;

	#ifdef FILTER_USE_DCM
		filterdata->Kp_rollPitch = Q_angle;
		filterdata->Ki_rollPitch  = Q_gyro;
		filterdata->Kp_yaw = 0;
		filterdata->Ki_yaw  = 0;
	#else /* Use kalman settings */
		filterdata->Q_angle = Q_angle;
		filterdata->Q_gyro  = Q_gyro;
		filterdata->R_angle = R_angle;
	#endif // FILTER_USE_DCM

}

void filter_update_constants(float Q_angle, float Q_gyro, float R_angle)
{	
	#ifdef FILTER_USE_DCM
		filterdata->Kp_rollPitch = Q_angle;
		filterdata->Ki_rollPitch  = Q_gyro;
		filterdata->Kp_yaw = 0;
		filterdata->Ki_yaw  = 0;
	#else /* Use kalman settings */
		filterdata->Q_angle = Q_angle;
		filterdata->Q_gyro  = Q_gyro;
		filterdata->R_angle = R_angle;
	#endif // FILTER_USE_DCM
}

float filter_get_roll_qangle(void)
{	
	#ifdef FILTER_USE_DCM
		return filterdata->Kp_rollPitch;
	#else /* Use kalman settings */
		return filterdata->Q_angle;
	#endif // FILTER_USE_DCM
}

float filter_get_roll_qgyro(void)
{
	#ifdef FILTER_USE_DCM
		return filterdata->Ki_rollPitch;
	#else /* Use kalman settings */
		return filterdata->Q_gyro;
	#endif // FILTER_USE_DCM
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
	#ifdef FILTER_USE_DCM
		filter_predict_accG_roll();
	#endif // FILTER_USE_DCM
	
	return filter_conv_rad_to_degrees(accRoll);
}

/*----------------------------------DCM Stuff-----------------------------------------*/

//Multiply two 3x3 matrixs. This function developed by Jordi can be easily adapted to multiple n*n matrix's. (Pero me da flojera!). 
static void Matrix_Multiply(float a[3][3], float b[3][3],float mat[3][3])
{
  float op[3]; 
  for(int x=0; x<3; x++)
  {
    for(int y=0; y<3; y++)
    {
      for(int w=0; w<3; w++)
      {
       op[w]=a[x][w]*b[w][y];
      } 
      mat[x][y]=0;
      mat[x][y]=op[0]+op[1]+op[2];
      
      float test=mat[x][y];
    }
  }
}

/**************************************************/
static void Normalize(void)
{
  float error=0;
  float temporary[3][3];
  float renorm=0;
  Bool problem=false;
  
  error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19

  Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
  Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19
  
  Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
  Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19
  
  Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20
  
  renorm= Vector_Dot_Product(&temporary[0][0],&temporary[0][0]); 
  if (renorm < 1.5625f && renorm > 0.64f) {
    renorm= .5 * (3-renorm);                                                 //eq.21
  } else if (renorm < 100.0f && renorm > 0.01f) {
    renorm= 1. / sqrt(renorm);
#if FILTER_PERFORMANCE_REPORTING == 1  
    //renorm_sqrt_count++;
#endif
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???SQT:1,RNM:");
    //Serial.print (renorm);
    //Serial.print (",ERR:");
    //Serial.print (error);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  } else {
    problem = true;
#if FILTER_PERFORMANCE_REPORTING == 1
    //renorm_blowup_count++;
#endif
	#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???PRB:1,RNM:");
    //Serial.print (renorm);
    //Serial.print (",ERR:");
    //Serial.print (error);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  }
  
  Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);
  
  renorm= Vector_Dot_Product(&temporary[1][0],&temporary[1][0]); 
  if (renorm < 1.5625f && renorm > 0.64f) {
    renorm= .5 * (3-renorm);                                                 //eq.21
  } else if (renorm < 100.0f && renorm > 0.01f) {
    renorm= 1. / sqrt(renorm);  
#if FILTER_PERFORMANCE_REPORTING == 1    
    //renorm_sqrt_count++;
#endif
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???SQT:2,RNM:");
    //Serial.print (renorm);
    //Serial.print (",ERR:");
    //Serial.print (error);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  } else {
    problem = true;
#if FILTER_PERFORMANCE_REPORTING == 1
    //renorm_blowup_count++;
#endif
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???PRB:2,RNM:");
    //Serial.print (renorm);
    //Serial.print (",ERR:");
    //Serial.print (error);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  }
  Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
  
  renorm= Vector_Dot_Product(&temporary[2][0],&temporary[2][0]); 
  if (renorm < 1.5625f && renorm > 0.64f) {
    renorm= .5 * (3-renorm);                                                 //eq.21
  } else if (renorm < 100.0f && renorm > 0.01f) {
    renorm= 1. / sqrt(renorm);   
#if FILTER_PERFORMANCE_REPORTING == 1 
    //renorm_sqrt_count++;
#endif
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???SQT:3,RNM:");
    //Serial.print (renorm);
    //Serial.print (",ERR:");
    //Serial.print (error);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  } else {
    problem = true;  
#if FILTER_PERFORMANCE_REPORTING == 1
    //renorm_blowup_count++;
#endif
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("???PRB:3,RNM:");
    //Serial.print (renorm);
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  }
  Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
  
  if (problem) {                // Our solution is blowing up and we will force back to initial condition.  Hope we are not upside down!
      DCM_Matrix[0][0]= 1.0f;
      DCM_Matrix[0][1]= 0.0f;
      DCM_Matrix[0][2]= 0.0f;
      DCM_Matrix[1][0]= 0.0f;
      DCM_Matrix[1][1]= 1.0f;
      DCM_Matrix[1][2]= 0.0f;
      DCM_Matrix[2][0]= 0.0f;
      DCM_Matrix[2][1]= 0.0f;
      DCM_Matrix[2][2]= 1.0f;
      problem = false;  
  }
}

/**************************************************/
static void Drift_correction(void)
{
  //Compensation the Roll, Pitch and Yaw drift. 
  //float mag_heading_x;
  //float mag_heading_y;
  //static float Scaled_Omega_P[3];
  static float Scaled_Omega_I[3];
  float Accel_magnitude;
  float Accel_weight;
  //float Integrator_magnitude;
  //float tempfloat;
  
  //*****Roll and Pitch***************

  // Calculate the magnitude of the accelerometer vector
  Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
  Accel_magnitude = Accel_magnitude / 4096; // Scale to gravity.
  // Dynamic weighting of accelerometer info (reliability filter)
  // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
  Accel_weight = constrain(1 - 2*abs(1 - Accel_magnitude),0,1);  //  
  
  #if FILTER_PERFORMANCE_REPORTING == 1
    //tempfloat = ((Accel_weight - 0.5) * 256.0f);    //amount added was determined to give imu_health a time constant about twice the time constant of the roll/pitch drift correction
    //imu_health += tempfloat;
    //imu_health = constrain(imu_health,129,65405);
  #endif
  
  Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
  Vector_Scale(&Omega_P[0],&errorRollPitch[0],filterdata->Kp_rollPitch*Accel_weight);
  
  Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],filterdata->Kp_rollPitch*Accel_weight);
  Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);     
  
  //*****YAW***************
  
  //#if FILTER_USE_MAGNETOMETER==1 
    //// We make the gyro YAW drift correction based on compass magnetic heading
    //#if BOARD_VERSION < 3
    //errorCourse=(DCM_Matrix[0][0]*APM_Compass.Heading_Y) - (DCM_Matrix[1][0]*APM_Compass.Heading_X);  //Calculating YAW error
    //#endif
    //#if BOARD_VERSION == 3
    //errorCourse=(DCM_Matrix[0][0]*Heading_Y) - (DCM_Matrix[1][0]*Heading_X);  //Calculating YAW error
    //#endif
    //Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
    //
    //Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);
    //Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.
    //
    //Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);
    //Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I   
  //#else  // Use GPS Ground course to correct yaw gyro drift
  //if(GPS.ground_speed>=SPEEDFILT*100)		// Ground speed from GPS is in m/s
  //{
	//COGX = cos(ToRad(GPS.ground_course/100.0));
	//COGY = sin(ToRad(GPS.ground_course/100.0));
    //errorCourse=(DCM_Matrix[0][0]*COGY) - (DCM_Matrix[1][0]*COGX);  //Calculating YAW error
    //Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
  //
    //Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);
    //Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.
  //
    //Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);
    //Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I   
  //}
  //#endif
  ////  Here we will place a limit on the integrator so that the integrator cannot ever exceed half the saturation limit of the gyros
  //Integrator_magnitude = sqrt(Vector_Dot_Product(Omega_I,Omega_I));
  //if (Integrator_magnitude > ToRad(300)) {
    //Vector_Scale(Omega_I,Omega_I,0.5f*ToRad(300)/Integrator_magnitude);
#if FILTER_PRINT_DEBUG != 0
    //Serial.print("!!!INT:1,MAG:");
    //Serial.print (ToDeg(Integrator_magnitude));
//
    //Serial.print (",TOW:");
    //Serial.print (GPS.time);  
    //Serial.println("***");    
#endif
  //}
  
  
}
/**************************************************/
static void Accel_adjust(void)
{
// Accel_Vector[1] += Accel_Scale((GPS.ground_speed/100)*Omega[2]);  // Centrifugal force on Acc_y = GPS ground speed (m/s) * GyroZ
// Accel_Vector[2] -= Accel_Scale((GPS.ground_speed/100)*Omega[1]);  // Centrifugal force on Acc_z = GPS ground speed (m/s) * GyroY 
}
/**************************************************/

static void Matrix_update(void)
{
  //Gyro_Vector[0]=Gyro_Scaled_X(read_adc(0)); //gyro x roll
  //Gyro_Vector[1]=Gyro_Scaled_Y(read_adc(1)); //gyro y pitch
  //Gyro_Vector[2]=Gyro_Scaled_Z(read_adc(2)); //gyro Z yaw
  //
  //Accel_Vector[0]=read_adc(3); // acc x
  //Accel_Vector[1]=read_adc(4); // acc y
  //Accel_Vector[2]=read_adc(5); // acc z
  
  Gyro_Vector[0] = ToRad(filterdata->xGyr); //+
  Gyro_Vector[1] = -ToRad(filterdata->yGyr); //-
  Gyro_Vector[2] = -ToRad(filterdata->zGyr); //-
  Accel_Vector[0] = -filterdata->xAcc/4096; //-
  Accel_Vector[1] = -filterdata->yAcc/4096; //-
  Accel_Vector[2] = -filterdata->zAcc/4096; //- richtig
  
  Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
  Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term

  //Accel_adjust();    //Remove centrifugal acceleration.
  
 #if OUTPUTMODE==1         
  Update_Matrix[0][0]=0;
  Update_Matrix[0][1]=-dt*Omega_Vector[2];//-z
  Update_Matrix[0][2]=dt*Omega_Vector[1];//y
  Update_Matrix[1][0]=dt*Omega_Vector[2];//z
  Update_Matrix[1][1]=0;
  Update_Matrix[1][2]=-dt*Omega_Vector[0];//-x
  Update_Matrix[2][0]=-dt*Omega_Vector[1];//-y
  Update_Matrix[2][1]=dt*Omega_Vector[0];//x
  Update_Matrix[2][2]=0;
 #else                    // Uncorrected data (no drift correction)
  Update_Matrix[0][0]=0;
  Update_Matrix[0][1]=-dt*Gyro_Vector[2];//-z
  Update_Matrix[0][2]=dt*Gyro_Vector[1];//y
  Update_Matrix[1][0]=dt*Gyro_Vector[2];//z
  Update_Matrix[1][1]=0;
  Update_Matrix[1][2]=-dt*Gyro_Vector[0];
  Update_Matrix[2][0]=-dt*Gyro_Vector[1];
  Update_Matrix[2][1]=dt*Gyro_Vector[0];
  Update_Matrix[2][2]=0;
 #endif

  Matrix_Multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c

  for(int x=0; x<3; x++) //Matrix Addition (update)
  {
    for(int y=0; y<3; y++)
    {
      DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
    } 
  }
}

static void Euler_angles(void)
{
  #if (OUTPUTMODE==2)         // Only accelerometer info (debugging purposes)
    filterdata->roll = 1.9*atan2(Accel_Vector[1],Accel_Vector[2]);    // atan2(acc_y,acc_z)
    filterdata->pitch = -1.9*asin((Accel_Vector[0])/(double)GRAVITY); // asin(acc_x)
    filterdata->yaw = 0;
  #else
    //filterdata->pitch = -1.9*asin(DCM_Matrix[2][0]);
    //filterdata->roll = filter_conv_rad_to_degrees(1.9*atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]));
    //filterdata->yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
	filterdata->pitch = ToDeg(-asin(DCM_Matrix[2][0]));
	filterdata->roll = -ToDeg(atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]));
	filterdata->yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
  #endif
}

void filter_task(unsigned long time)
{
	/* Calculate time elapsed since last call (dt) */
	/* Please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	dt = (float)(time - lastMicros) / 1000000;	
	lastMicros = time;						

	/* Execute DCM */
#ifdef FILTER_USE_DCM
	Matrix_update();
	Normalize();
	Drift_correction();
	Euler_angles();
#else /* Execute kalman filter */
	filter_predict_accG_roll();
	filter_ars_predict();		// Kalman predict
	filter_ars_update(accRoll);    // Kalman update + result (angle)
	filterdata->roll = ToDeg(filterdata->x_angle);
#endif // FILTER_USE_DCM

}

