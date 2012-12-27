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

//////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////
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
#else /* Use Kalman */
	float x_angle;			/*!< brief These variable represent our state matrix x */
	float x_bias;			/*!< brief These variable represent our state matrix x */
	
	float P_00;				/*!< brief Our error covariance matrix */
	float P_01;				/*!< brief Our error covariance matrix */
	float P_10;				/*!< brief Our error covariance matrix */
	float P_11;				/*!< brief Our error covariance matrix */
#endif // FILTER_USE_DCM


//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////
#ifdef FILTER_USE_DCM
	static void filter_dcm_matrix_multiply(float a[3][3], float b[3][3],float mat[3][3]);
	static void filter_dcm_normalize(void);
	static void filter_dcm_drift_correction(void);
	static void filter_dcm_accel_adjust(void);
	static void filter_dcm_matrix_update(void);
	static void filter_dcm_euler_angles(void);
#else /* Use Kalman */
	static float filter_kalman_ars_update(const float angle_m);
	static void filter_predict_accG_roll(void);
	static void filter_kalman_ars_predict(void);
#endif // FILTER_USE_DCM

/*-------------------------------------------------------------Kalman Functions---------------------------------------------------------------*/

#ifndef FILTER_USE_DCM
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
	void filter_kalman_init(FILTER_DATA_t *filter, float Q_angle, float Q_gyro, float R_angle)
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

	/**************************************************************************
	* \brief Filter DCM Matrix Multiply
	*	The predict function. Updates 2 variables:
	*	our model-state x and the 2x2 matrix P /n
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
	* \param ---
	*
	* \return  ---
	***************************************************************************/
	static void filter_kalman_ars_predict(void)
	{
		x_angle += dt * (ToRad(filterdata->xGyr) - x_bias);

		P_00 +=  - dt * (P_10 + P_01) + filterdata->Q_angle * dt;
		P_01 +=  - dt * P_11;
		P_10 +=  - dt * P_11;
		P_11 +=  + filterdata->Q_gyro * dt;
	}

	/**************************************************************************
	* \brief Filter Kalman ARS Update
	*	The update function updates our model using
	*	the information from a 2nd measurement.
	*	Input angle_m is the angle measured by the accelerometer.. /n
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
	*        P(1,0)-P(0,0)*K(1)  P(1,1)-P(0,1)*K(1)
	*
	* \param angle_m angle
	*
	* \return  ???
	***************************************************************************/
	static float filter_kalman_ars_update(const float angle_m)
	{
		const float y = angle_m - x_angle;
	
		const float S = P_00 + filterdata->R_angle;
		const float K_0 = P_00 / S;
		const float K_1 = P_10 / S;
	
		x_angle +=  K_0 * y;
		x_bias  +=  K_1 * y;
	
		P_00 -= K_0 * P_00;
		P_01 -= K_0 * P_01;
		P_10 -= K_1 * P_00;
		P_11 -= K_1 * P_01;
	
		return x_angle;
	}
#endif // FILTER_USE_DCM

/**************************************************************************
* \brief Filter Kalman Update Constants
*	Updates the Kalman Settings. \n
*
* \param ---
*
* \return ---
***************************************************************************/
void filter_kalman_update_constants(float Q_angle, float Q_gyro, float R_angle)
{
	filterdata->Q_angle = Q_angle;
	filterdata->Q_gyro  = Q_gyro;
	filterdata->R_angle = R_angle;
}

/**************************************************************************
* \brief Filter Kalman Get Q Angle
*	Returns the kalman y angle setting. \n
*
* \param ---
*
* \return  q angle
***************************************************************************/
float filter_kalman_get_roll_qangle(void)
{
	return filterdata->Q_angle;
}

/**************************************************************************
* \brief Filter Kalman Get Q Gyro
*	Returns the kalman y gyro setting. \n
*
* \param ---
*
* \return  q gyro
***************************************************************************/
float filter_kalman_get_roll_qgyro(void)
{
	return filterdata->Q_gyro;
}

/**************************************************************************
* \brief Filter Kalman Get R Angle
*	Returns the kalman r angle setting. \n
*
* \param ---
*
* \return r angle
***************************************************************************/
float filter_kalman_get_roll_rangle(void)
{
	return filterdata->R_angle;
}

/*-------------------------------------------------------------DCM Functions ------------------------------------------------------------*/

#ifdef FILTER_USE_DCM
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
	void filter_dcm_init(FILTER_DATA_t *filter, float KpRollPitch, float KiRollPitch, float KpYaw, float KiYaw)
	{
		filterdata = filter;

		filterdata->Kp_rollPitch = KpRollPitch;
		filterdata->Ki_rollPitch  = KiRollPitch;
		filterdata->Kp_yaw = KpYaw;
		filterdata->Ki_yaw  = KiYaw;
	}

	/**************************************************************************
	* \brief Filter DCM Matrix Multiply
	*	Multiply two 3x3 matrix's. This function developed by Jordi 
	*   can be easily adapted to multiple n*n matrix's. /n
	*
	* \param *filter filter struct
	* \param a matrix 1
	* \param b matrix 2
	* \param mat result
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_matrix_multiply(float a[3][3], float b[3][3],float mat[3][3])
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
		}
	  }
	}

	/**************************************************************************
	* \brief Filter DCM Normalize
	*
	* \param ---
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_normalize(void)
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
   
		if(renorm < 1.5625f && renorm > 0.64f) 
		{
			renorm= .5 * (3-renorm);                                                 //eq.21
		} 
		else if(renorm < 100.0f && renorm > 0.01f) 
		{
			renorm= 1. / sqrt(renorm);
		}
		else 
		{
			problem = true;
		}
  
		Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);  
		renorm= Vector_Dot_Product(&temporary[1][0],&temporary[1][0]); 
  
		if(renorm < 1.5625f && renorm > 0.64f)
		{
			renorm= .5 * (3-renorm);                                                 //eq.21
		}
		else if(renorm < 100.0f && renorm > 0.01f) 
		{
			renorm= 1. / sqrt(renorm);  
		}
		else
		{
		problem = true;
		}
  
		Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
		renorm= Vector_Dot_Product(&temporary[2][0],&temporary[2][0]);
   
		if(renorm < 1.5625f && renorm > 0.64f)
		{
			renorm= .5 * (3-renorm);                                                 //eq.21
		}
		else if(renorm < 100.0f && renorm > 0.01f)
		{
			renorm= 1. / sqrt(renorm);   
		}
		else
		{
			problem = true;  
		}
	
		Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
  
	  /* Our solution is blowing up and we will force back to initial condition.  Hope we are not upside down! */
		if (problem)
		{                
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

	/**************************************************************************
	* \brief Filter DCM Drift Correction
	*
	* \param ---
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_drift_correction(void)
	{
		//Compensation the Roll, Pitch and Yaw drift. 
		//float mag_heading_x;
		//float mag_heading_y;
		//static float Scaled_Omega_P[3];
		//float Integrator_magnitude;
		//float tempfloat;
  		static float Scaled_Omega_I[3];
  		float Accel_magnitude;
  		float Accel_weight;
	  
		//***** Roll and Pitch ***************
		/* Calculate the magnitude of the accelerometer vector */
		Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
		Accel_magnitude = Accel_magnitude / 4096; // Scale to gravity.
  
		/* Dynamic weighting of accelerometer info (reliability filter)
		* Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0) */
		Accel_weight = constrain(1 - 2*abs(1 - Accel_magnitude),0,1);  //  
   
		Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
		Vector_Scale(&Omega_P[0],&errorRollPitch[0],filterdata->Kp_rollPitch*Accel_weight);
  
		Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],filterdata->Kp_rollPitch*Accel_weight);
		Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);     
  
	  /*********** YAW ***************/
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
	  //}
	}

	/**************************************************************************
	* \brief Filter DCM Acceleration Adjust
	*
	* \param ---
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_accel_adjust(void)
	{
	// Accel_Vector[1] += Accel_Scale((GPS.ground_speed/100)*Omega[2]);  // Centrifugal force on Acc_y = GPS ground speed (m/s) * GyroZ
	// Accel_Vector[2] -= Accel_Scale((GPS.ground_speed/100)*Omega[1]);  // Centrifugal force on Acc_z = GPS ground speed (m/s) * GyroY 
	}

	/**************************************************************************
	* \brief Filter DCM Matrix Update
	*
	* \param ---
	
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_matrix_update(void)
	{  
		Gyro_Vector[0] = ToRad(filterdata->xGyr);	//+
		Gyro_Vector[1] = -ToRad(filterdata->yGyr);	//-
		Gyro_Vector[2] = -ToRad(filterdata->zGyr);	//-
		Accel_Vector[0] = -filterdata->xAcc/4096;	//-
		Accel_Vector[1] = -filterdata->yAcc/4096;	//-
		Accel_Vector[2] = -filterdata->zAcc/4096;	//- 
  
		Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
		Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term

		/* Remove centrifugal acceleration. */
		//filter_dcm_accel_adjust();    //Not yet used!
         
		Update_Matrix[0][0]=0;
		Update_Matrix[0][1]=-dt*Omega_Vector[2];//-z
		Update_Matrix[0][2]=dt*Omega_Vector[1];//y
		Update_Matrix[1][0]=dt*Omega_Vector[2];//z
		Update_Matrix[1][1]=0;
		Update_Matrix[1][2]=-dt*Omega_Vector[0];//-x
		Update_Matrix[2][0]=-dt*Omega_Vector[1];//-y
		Update_Matrix[2][1]=dt*Omega_Vector[0];//x
		Update_Matrix[2][2]=0;

		filter_dcm_matrix_multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c

		/* Matrix Addition (update) */
		for(int x=0; x<3; x++) 
		{
			for(int y=0; y<3; y++)
			{
				DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
			} 
		}
	}

	/**************************************************************************
	* \brief Filter DCM Euler Angels
	*	Calculate the Euler angels. \n
	*
	* \param ---
	*
	* \return  ---
	***************************************************************************/
	static void filter_dcm_euler_angles(void)
	{
		filterdata->pitch = ToDeg(-asin(DCM_Matrix[2][0]));
		filterdata->roll = -ToDeg(atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]));
		filterdata->yaw = ToDeg(atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]));
	}
#endif // FILTER_USE_DCM

/**************************************************************************
* \brief Filter DCM Update Constants
*	Updates the Kalman Settings. \n
*
* \param ---
*
* \return ---
***************************************************************************/
void filter_dcm_update_constants(float KpRollPitch, float KiRollPitch, float KpYaw, float KiYaw)
{
	filterdata->Kp_rollPitch = KpRollPitch;
	filterdata->Ki_rollPitch  = KiRollPitch;
	filterdata->Kp_yaw = KpYaw;
	filterdata->Ki_yaw  = KiYaw;
}

/**************************************************************************
* \brief Filter DCM Get Roll Pitch kp
*	Returns the Roll/Pitch Kp setting. \n
*
* \param ---
*
* \return  Roll/Pitch Kp
***************************************************************************/
float filter_dcm_get_rollPitch_kp(void)
{
	return filterdata->Kp_rollPitch;
}

/**************************************************************************
* \brief Filter DCM Get Roll Pitch Ki
*	Returns the Roll/Pitch Ki setting. \n
*
* \param ---
*
* \return  Roll/Pitch Ki
***************************************************************************/
float filter_dcm_get_rollPitch_ki(void)
{
	return filterdata->Ki_rollPitch;
}

/**************************************************************************
* \brief Filter DCM Get Yaw kp
*	Returns the yaw Kp setting. \n
*
* \param ---
*
* \return  Yaw Kp
***************************************************************************/
float filter_dcm_get_yaw_kp(void)
{
	return filterdata->Kp_yaw;
}

/**************************************************************************
* \brief Filter DCM Get Yaw ki
*	Returns the yaw Ki setting. \n
*
* \param ---
*
* \return  Yaw Ki
***************************************************************************/
float filter_dcm_get_yaw_ki(void)
{
	return filterdata->Ki_yaw;
}

/*---------------------------------------------------------- General Functions -----------------------------------------------------------*/

/**************************************************************************
* \brief Filter Predict Roll Angle
*	Calculate the roll angle from acceleration measurements. \n
*
* \param ---
*
* \return ---
***************************************************************************/	
static void filter_predict_accG_roll(void)
{
	accRoll = -(atan2(-(filterdata->zAcc), filterdata->yAcc)-(PI/2.0));
}

/**************************************************************************
* \brief Filter Get Acc Roll
*	Returns the calculated roll angle from the acceleration measurements. \n
*
* \param ---
*
* \return  accRoll angle
***************************************************************************/
float filter_get_acc_roll(void)
{
	#ifdef FILTER_USE_DCM
		filter_predict_accG_roll();
	#endif // FILTER_USE_DCM
	
	return ToDeg(accRoll);
}

/**************************************************************************
* \brief Filter Task
*	Process all filter calculations. \n
*
* \param ---
*
* \return ---
***************************************************************************/
void filter_task(unsigned long time)
{
	/* Calculate time elapsed since last call (dt) */
	/* Please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	dt = (float)(time - lastMicros) / 1000000;	
	lastMicros = time;						
	
#ifdef FILTER_USE_DCM /* Execute DCM */
	filter_dcm_matrix_update();
	filter_dcm_normalize();
	filter_dcm_drift_correction();
	filter_dcm_euler_angles();
#else /* Execute Kalman filter */
	filter_predict_accG_roll();
	filter_kalman_ars_predict();		// Kalman predict
	filter_kalman_ars_update(accRoll);    // Kalman update + result (angle)
	filterdata->roll = ToDeg(x_angle);
#endif // FILTER_USE_DCM
}

