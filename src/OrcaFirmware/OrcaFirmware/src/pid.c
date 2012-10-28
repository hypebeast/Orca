/*
 * pid.c
 *
 * Created: 11.10.2012 16:06:53
 *  Author: f-ruml
 */ 

#include "pid.h"

float ta = 0;
unsigned long pidLastMicros = 0;

/*! \brief Initialization of PID controller parameters.  
 *  
 *  Initialize the variables used by the PID algorithm.  
 *  
 *  \param p_factor  Proportional term.  
 *  \param i_factor  Integral term.  
 *  \param d_factor  Derivate term.  
 *  \param pid  Struct with PID status.  
 */ 

// Set up PID controller parameters    
void pid_Init(float p_factor, float i_factor, float d_factor, struct PID_DATA *pid)   
{   
  // Start values for PID controller   
  pid->sumError = 0;   
  pid->lastProcessValue = 0;   
  // Tuning constants for PID loop   
  pid->P_Factor = p_factor /** PID_SCALING_FACTOR*/;   
  pid->I_Factor = i_factor /** PID_SCALING_FACTOR*/;   
  pid->D_Factor = d_factor /** PID_SCALING_FACTOR*/;   
  // Limits to avoid overflow   
  pid->maxError = MAX_INT / (pid->P_Factor + 1);   
  pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);   
} 
   
/*! \brief PID control algorithm.  
 *  
 *  Calculates output from setpoint, process value and PID status.  
 *  
 *  \param setPoint  Desired value.  
 *  \param processValue  Measured value.
 *   param time Abtastzeit (us) 
 *  \param pid_st  PID status struct.  
 */   
int16_t pid_Controller(int16_t setPoint, int16_t processValue, unsigned long  time, struct PID_DATA *pid_st)   
{   
  float error, p_term, d_term;   
  float i_term, ret, temp;   
   
   	/* Calculate time elapsed since last call (dt) */
	/*please note that overflows are ok, since for example 0x0001 - 0x00FE will be equal to 2 */
	ta = (float)(time) / 1000000;	

	//ta = (float)(time - pidLastMicros) / 1000000;
	//pidLastMicros = time;
		
  error = (setPoint - processValue);   
   
  // Calculate Pterm and limit error overflow   
  if (error > pid_st->maxError){   
    p_term = MAX_INT;   
  }   
  else if (error < -pid_st->maxError){   
    p_term = -MAX_INT;   
  }   
  else{   
    p_term = pid_st->P_Factor * error;   
  }   
   
  // Calculate Iterm and limit integral runaway   
  temp = (pid_st->sumError + error);   
  if(temp > pid_st->maxSumError){   
    i_term = MAX_I_TERM;   
    pid_st->sumError = pid_st->maxSumError;   
  }   
  else if(temp  < -pid_st->maxSumError){   
    i_term = -MAX_I_TERM;   
    pid_st->sumError = -pid_st->maxSumError;   
  }   
  else{   
    pid_st->sumError = temp;   
    i_term = (pid_st->I_Factor) * (pid_st->sumError) * ta;   
  }   
   
  // Calculate Dterm   
  d_term = pid_st->D_Factor * (processValue - (pid_st->lastProcessValue)) / ta;   
   
  pid_st->lastProcessValue = processValue;   
   
  ret = (p_term + i_term + d_term);   
  if(ret > MAX_INT){   
    ret = MAX_INT;   
  }   
  else if(ret < -MAX_INT){   
    ret = -MAX_INT;   
  }   
   
  return((int16_t)ret);   
}   
   
/*! \brief Resets the integrator.  
 *  
 *  Calling this function will reset the integrator in the PID regulator.  
 */   
void pid_Reset_Integrator(pidData_t *pid_st)   
{   
  pid_st->sumError = 0;   
}


/**************************************************************************
* \brief PID Update Tuning Constants
*
* Call this method to update the p, i and d factor of the PID controller.
*
* \param p_factor P factor tuning constant
* \param i_factor I factor tuning constant
* \param d_factor D factor tuning constant
* \param *pid PID controller data structure
*
* \return  status code
***************************************************************************/
void pid_update_tuning_constants(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid)
{
	pid->P_Factor = p_factor * PID_SCALING_FACTOR;
	pid->I_Factor = i_factor * PID_SCALING_FACTOR;
	pid->D_Factor = d_factor * PID_SCALING_FACTOR;
	
	pid->maxError = MAX_INT / (pid->P_Factor + 1);
	pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}   