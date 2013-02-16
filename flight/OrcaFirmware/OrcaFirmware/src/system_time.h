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

/** Ticks per milliseconds */
#define SYSTEM_TIME_TICK_RATE_MS 100


/************************************************************************
* \brief Represents the system time.
************************************************************************/
typedef struct {
	uint32_t ticks; /** Ticks */
	uint8_t days; /** Days */
	uint8_t hours; /** Hours */
	uint8_t minutes; /** Minutes */
	uint8_t seconds; /** Seconds */
	uint8_t milliseconds; /** Milliseconds */
} SystemTime_t;


void system_time_init();
uint32_t system_time_get_ticks();
void system_time_get_time(SystemTime_t* timeOut);

#endif /* SYSTEM_TIME_H_ */