/**************************************************************************
 * \file system_time.c
 *
 * \brief Implements a system timer.
 *
 * The TCC0 TC is used for counting the system time.
 *
 * Created: 16.02.2013 20:53:50
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include <string.h>
#include <tc.h>
#include "system_time.h"


//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

static SystemTime_t system_time;


//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static void system_timer_overflow_callback();


/************************************************************************
* \brief Initializes the system time module.
************************************************************************/
void system_time_init()
{	
	// Enable timer/counter TCC0
	tc_enable(&SYSTEM_TIMER_TC);
	
	// Set the callback function for overflow interrupt
	tc_set_overflow_interrupt_callback(&SYSTEM_TIMER_TC, system_timer_overflow_callback);
	
	// Set the waveform mode (normal mode: count until TOP value is reached. Then
	// reset its count to 0.
	tc_set_wgm(&SYSTEM_TIMER_TC, TC_WG_NORMAL);
	
	// Set the period (top value of the counter)
	tc_write_period(&SYSTEM_TIMER_TC, SYSTEM_TIME_PERIOD);
	
	// Set the overflow interrupt level
	tc_set_overflow_interrupt_level(&SYSTEM_TIMER_TC, TC_INT_LVL_LO);
	
	// Set the clock source
	tc_write_clock_source(&SYSTEM_TIMER_TC, SYSTEM_TIME_PRESCALER);
	
	// Initialize time to zero
	system_time.jiffies = 0;
	system_time.ticks = 0;
	system_time.milliseconds = 0;
	system_time.seconds = 0;
	system_time.minutes = 0;
	system_time.hours = 0;
	system_time.days = 0;
}

/************************************************************************
* \brief Get the ticks (jiffies) since system start.
************************************************************************/
uint32_t system_time_get_ticks()
{
	return system_time.jiffies;
}

/************************************************************************
* \brief Get the system time.
************************************************************************/
void system_time_get_time(SystemTime_t* timeOut)
{
	memcpy(timeOut, &system_time, sizeof(SystemTime_t));	
}

/************************************************************************
* \brief Return the system time in milliseconds.
*
* \return		The system time in milliseconds.
************************************************************************/
uint16_t system_time_get_time_ms()
{
	return system_time_get_ticks() * SYSTEM_TIME_TICK_RATE_MS;
}

/************************************************************************
 \brief Callback function for the overflow interrupt.
************************************************************************/
static void system_timer_overflow_callback()
{
	// Increase the jiffies and ticks
	system_time.jiffies++;
	
	// Count ticks or wrap back to 0
	if (system_time.ticks < (SYSTEM_TIME_TICKS_PER_SECOND - 1))
	{
		system_time.ticks++;
	}
	else
	{
		system_time.ticks = 0;
		
		// Count seconds or wrap back to 0
		if (system_time.seconds < (SYSTEM_TIME_SECONDS_PER_MINUTE - 1))
		{
			system_time.seconds++;
		}
		else
		{
			system_time.seconds = 0;
			
			// Count minutes or wrap back to 0
			if (system_time.minutes < (SYSTEM_TIME_MINUTES_PER_HOUR - 1))
			{
				system_time.minutes++;
			}
			else
			{
				system_time.minutes = 0;
				
				// Count hours or wrap back to 0
				if (system_time.hours < (SYSTEM_TIME_HOURS_PER_DAY -1 ))
				{
					system_time.hours++;
				}
				else
				{
					system_time.hours = 0;
					
					// Count days
					system_time.days++;
				}
			}
		}
	}
}