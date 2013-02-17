/**************************************************************************
 * \file system_time.h
 *
 * \brief Header file for system_time.c
 *
 * Created: 16.02.2013 20:53:50
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#ifndef SYSTEM_TIME_H_
#define SYSTEM_TIME_H_

#include <asf.h>
#include <sysclk.h>
#include <stdlib.h>


#define SYSTEM_TIMER_TC TCC0
//#define SYSTEM_TIME_F_CPU 32000000.0f // 32 MHz
#define SYSTEM_TIME_F_CPU sysclk_get_cpu_hz()
#define SYSTEM_TIME_PERIOD 200 /** TC TOP value (in ticks) */
#define SYSTEM_TIME_PRESCALER TC_CLKSEL_DIV64_gc
#define SYSTEM_TIME_PRESCAKER_VALUE 64
/** Tick duration in milliseconds */
#define SYSTEM_TIME_TICK_RATE_MS (((1*SYSTEM_TIME_PRESCAKER_VALUE*SYSTEM_TIME_PERIOD)/SYSTEM_TIME_F_CPU)*1000)
#define SYSTEM_TIME_TICKS_PER_SECOND (1/(SYSTEM_TIME_TICK_RATE_MS/1000))
#define SYSTEM_TIME_SECONDS_PER_MINUTE 60
#define SYSTEM_TIME_MINUTES_PER_HOUR 60
#define SYSTEM_TIME_HOURS_PER_DAY 24


/************************************************************************
* \brief Represents the system time.
************************************************************************/
typedef struct {
	uint32_t jiffies; /** Total number of ticks since system start. */
	uint16_t ticks; /** Ticks part **/
	uint8_t days; /** Days */
	uint8_t hours; /** Hours */
	uint8_t minutes; /** Minutes */
	uint8_t seconds; /** Seconds */
	uint8_t milliseconds; /** Milliseconds */
} SystemTime_t;


void system_time_init();
uint32_t system_time_get_ticks();
void system_time_get_time(SystemTime_t* timeOut);
uint16_t system_time_get_time_ms();

#endif /* SYSTEM_TIME_H_ */