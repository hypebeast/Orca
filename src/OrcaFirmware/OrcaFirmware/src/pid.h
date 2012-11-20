/*
 * pid.h
 *
 * Created: 11.10.2012 16:07:08
 *  Author: f-ruml
 */ 


#ifndef PID_H_
#define PID_H_

#include "sysclk.h"

#define PID_SCALING_FACTOR  1024 
 
/*! \brief PID status and configuration variables.
 * 
 * Setpoints and data used by the PID control algorithm 
 */ 
typedef struct PID_DATA{ 
  //! Last process value, used to find derivative of process value. 
  int16_t lastProcessValue; 
  //! Summation of errors, used for integrate calculations 
  float sumError; 
  //! The Proportional tuning constant, multiplied with SCALING_FACTOR 
  float P_Factor; 
  //! The Integral tuning constant, multiplied with SCALING_FACTOR 
  float I_Factor; 
  //! The Derivative tuning constant, multiplied with SCALING_FACTOR 
  float D_Factor; 
   //! Maximum allowed I value
  float I_Limit;
  //! Maximum allowed error, avoid overflow 
  float maxError; 
  //! Maximum allowed sumerror, avoid overflow 
  float maxSumError; 
} pidData_t; 
 
/*! \brief Maximum values 
 * 
 * Needed to avoid sign/overflow problems 
 */ 
// Maximum value of variables 
#define MAX_INT         INT16_MAX 
#define MAX_LONG        INT32_MAX 
#define MAX_I_TERM      (MAX_LONG / 2) 
 
// Boolean values 
#define FALSE           0 
#define TRUE            1 

void pid_Init(float p_factor, float i_factor, float d_factor, float i_limit, struct PID_DATA *pid)  ; 
float pid_Controller(float setPoint, float processValue, unsigned long  time, struct PID_DATA *pid_st);
void pid_Reset_Integrator(pidData_t *pid_st); 
void pid_update_tuning_constants(float p_factor, float i_factor, float d_factor, float i_limit, struct PID_DATA *pid);

#endif /* PID_H_ */