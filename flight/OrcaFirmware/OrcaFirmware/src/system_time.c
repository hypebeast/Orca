/**************************************************************************
 * \file system_time.c
 *
 * \brief Implements a system timer. The system timer keeps track until
 *
 * Created: 16.02.2013 20:53:50
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "system_time.h"


static SystemTime_t system_time;


/************************************************************************
* \brief Initializes the system time module.
************************************************************************/
void system_time_init()
{
	// Initialize everything to zero
	system_time.ticks = 0;
	system_time.milliseconds = 0;
	system_time.seconds = 0;
	system_time.minutes = 0;
	system_time.hours = 0;
	system_time.days = 0;
	
	// TODO: Initialize timer
}

/************************************************************************
* \brief Initializes the system time module.
************************************************************************/
uint32_t system_time_get_ticks()
{
	// TODO
}

/************************************************************************
* \brief Initializes the system time module.
************************************************************************/
void system_time_get_time(SystemTime_t* timeOut)
{
	// TODO
}