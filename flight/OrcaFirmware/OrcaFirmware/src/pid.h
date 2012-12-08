/*
 * pid.h
 *
 * Created: 11.10.2012 16:07:08
 *  Author: f-ruml
 */ 


#ifndef PID_H_
#define PID_H_

#include "sysclk.h"


//---------------------------------------------------------------------
//	General
//---------------------------------------------------------------------
// Maximum value of variables 
//#define MAX_INT         INT16_MAX 
//#define MAX_LONG        INT32_MAX 
//#define MAX_I_TERM      (MAX_LONG / 2) 
//#define PID_SCALING_FACTOR  1024 

/*! \brief PID status and configuration variables.
 * 
 * Set points and data used by the PID control algorithm 
 */ 
typedef struct PID_DATA{ 
  float lastProcessValue;   /*!< brief Last process value, used to find derivative of process value. */
  float sumError;			/*!< brief Summation of errors, used for integrate calculations */  
  float P_Factor;			/*!< brief The Proportional tuning constant, multiplied with SCALING_FACTOR */
  float I_Factor;			/*!< brief The Integral tuning constant, multiplied with SCALING_FACTOR */  
  float D_Factor;			/*!< brief The Derivative tuning constant, multiplied with SCALING_FACTOR */  
  float I_Limit;			/*!< brief Maximum allowed I value */
  //float maxError;			/*!< brief Maximum allowed error, avoid overflow */  
  //float maxSumError;		/*!< brief Maximum allowed sumerror, avoid overflow */  
}PID_DATA_t; 

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

void pid_Init(float p_factor, float i_factor, float d_factor, float i_limit, PID_DATA_t *pid)  ; 
float pid_Controller(float setPoint, float processValue, unsigned long  time, PID_DATA_t *pid);
void pid_Reset_Integrator(PID_DATA_t *pid); 
void pid_update_tuning_constants(float p_factor, float i_factor, float d_factor, float i_limit, PID_DATA_t *pid);

#endif /* PID_H_ */