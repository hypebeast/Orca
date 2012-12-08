/*
 * pid.c
 *
 * Created: 11.10.2012 16:06:53
 *  Author: f-ruml
 */ 

#include "pid.h"

float ta = 0;
unsigned long pidLastMicros = 0;

/**************************************************************************
*! \brief Initialization of PID controller parameters.  
*  
*  Initialize the variables used by the PID algorithm.  
*  
*  \param p_factor  Proportional term.  
*  \param i_factor  Integral term.  
*  \param d_factor  Derivate term.  
*  \param pid  Struct with PID status.  
***************************************************************************/

void pid_Init(float p_factor, float i_factor, float d_factor, float i_limit, PID_DATA_t *pid)   
{   
	/* Start values for PID controller */   
	pid->sumError = 0;   
	pid->lastProcessValue = 0;
     
	/* Tuning constants for PID loop */   
	pid->P_Factor = p_factor /** PID_SCALING_FACTOR*/;   
	pid->I_Factor = i_factor /** PID_SCALING_FACTOR*/;   
	pid->D_Factor = d_factor /** PID_SCALING_FACTOR*/;  
	pid->I_Limit = i_limit;
   
	/* Limits to avoid overflow */  
	//pid->maxError = MAX_INT / (pid->P_Factor + 1);   
	//pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);   
} 
    
/**************************************************************************
* \brief PID control algorithm
*
* Calculates output from setpoint, process value and PID status.\n
*
* \param setPoint  Desired value.
* \param processValue  Measured value.
* \param time sampling time (us)
* \param *pid PID controller data structure
*
* \return ---
***************************************************************************/
float pid_Controller(float setPoint, float processValue, unsigned long  time, PID_DATA_t *pid)   
{   
  float error, p_term, d_term;   
  float i_term;   
   
   	/* Calculate time elapsed since last call (dt) */
	/*please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	ta = (float)(time) / 1000000;	
	//ta = (float)(time - pidLastMicros) / 1000000;
	//pidLastMicros = time;
		
	error = (setPoint - processValue);   
   
	/* Calculate P term */   
    p_term = pid->P_Factor * error;    
   
	/* Calculate I term */
	pid->sumError += error * ta;   
	i_term = pid->I_Factor * pid->sumError;   
	i_term = constrain(i_term,-pid->I_Limit,pid->I_Limit); // Limit i value
   
	/* Calculate D term */   
	d_term = pid->D_Factor * (processValue - (pid->lastProcessValue)) / ta;    
	pid->lastProcessValue = processValue;   
   
	return (p_term + i_term + d_term);     
}   
   
/**************************************************************************
* \brief Resets the integrator.  
*  
*  Calling this function will reset the integrator in the PID regulator.\n 
* 
* \param *pid PID controller data structure
*
* \return ---
***************************************************************************/
void pid_Reset_Integrator(PID_DATA_t *pid)   
{   
	pid->sumError = 0;   
}


/**************************************************************************
* \brief PID Update Tuning Constants
*
* Call this method to update the p, i and d factor of the PID controller.\n
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
* \param *pid PID controller data structure
*
* \return ---
***************************************************************************/
void pid_update_tuning_constants(float p_factor, float i_factor, float d_factor, float i_limit, PID_DATA_t *pid)
{
	pid->P_Factor = p_factor;
	pid->I_Factor = i_factor;
	pid->D_Factor = d_factor;
	
	pid->I_Limit = i_limit;
	pid->sumError = 0;
	//pid->maxError = MAX_INT / (pid->P_Factor + 1);
	//pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}   