/**************************************************************************
 * \file gps.c
 *
 * \brief This file contains functions for handling the Fastrax UP501 GPS
 *        module.
 *
 * Created: 04.12.2012 23:33:50
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/


#ifndef GPS_H_
#define GPS_H_

#include "asf.h"

#define GPS_USART_INTERFACE			     BOARD_USART_GPS_INTERFACE
#define GPS_USART_BAUDRATE				 57600
#define GPS_USART_SERIAL_CHAR_LENGTH     USART_CHSIZE_8BIT_gc
#define GPS_USART_SERIAL_PARITY          USART_PMODE_DISABLED_gc
#define GPS_USART_SERIAL_STOP_BIT        true


/**************************************************************************
* \brief NMEA GGA message.
**************************************************************************/
typedef struct STRUCT_GGA_MESSAGE {
	uint8_t utc_time_hours[2]; /** UTC time hours */
	uint8_t utc_time_minutes[2]; /** UTC time minutes */
	uint8_t utc_time_seconds[2]; /** UTC time seconds */
	uint8_t utc_time_decimal_seconds[2]; /** UTC time decimal part of seconds*/
	uint8_t latitude_degrees[2]; /** Latitude degrees */
	uint8_t latitude_minutes[2]; /** Latitude minutes */
	uint8_t latitude_decimal_minutes[4]; /** Latitude decimal part of minutes */
	uint8_t latitude_direction; /** Latitude direction either N=North or S=South */
	uint8_t longitude_degrees[3]; /** Longitude degrees */
	uint8_t longitude_minutes[2]; /** Longitude minutes */
	uint8_t longitude_decimal_minutes[4]; /** Longitude decimal part of minutes */
	uint8_t longitude_direction; /** Longitude direction either E=East or W=West */
	uint8_t fix_valid_indicator; /** Fix valid indicator */
	uint8_t number_of_satellites[2]; /** Number of satellites used in position fix (0-12) */
	uint8_t hdop; /** HDOP (Horizontal Dilution of Precision) */
	uint8_t hdop_fractional_part[3]; /** HDOP fractional part */
	uint8_t altitude[5]; /** Altitude in meters, where ([0]*10000 + [1]*1000 + [2]*100 + [3]*10 + [4]*1) + altitude_decimeters */
	uint8_t altitude_decimeters; /** Altitude decimeters */
} gga_message_t;

/**************************************************************************
* \brief NMEA RMC message.
**************************************************************************/
typedef struct STRUCT_RMC_MESSAGE {
	uint8_t utc_time_hours[2]; /** UTC time hours */
	uint8_t utc_time_minutes[2]; /** UTC time minutes */
	uint8_t utc_time_seconds[2]; /** UTC time seconds */
	uint8_t utc_time_decimal_seconds[2]; /** UTC time decimal part of seconds*/
	uint8_t status; /** Status indicator: A = valid, V = invalid */
	uint8_t latitude_degrees[2]; /** Latitude degrees */
	uint8_t latitude_minutes[2]; /** Latitude minutes */
	uint8_t latitude_decimal_minutes[4]; /** Latitude decimal part of minutes */
	uint8_t latitude_direction; /** Latitude direction either N=North or S=South */
	uint8_t longitude_degrees[3]; /** Longitude degrees */
	uint8_t longitude_minutes[2]; /** Longitude minutes */
	uint8_t longitude_decimal_minutes[4]; /** Longitude decimal part of minutes */
	uint8_t longitude_direction; /** Longitude direction either E=East or W=West */
	uint8_t speed[4]; /** Speed in knots */
	uint8_t speed_decimal[2]; /** Decimal part of speed */
	uint8_t heading[3]; /** Heading */
	uint8_t heading_decimal[2]; /** Heading decimal part */
	uint8_t utc_date_day[2]; /** UTC date day */
	uint8_t utc_date_month[2]; /** UTC date month */
	uint8_t utc_date_year[2]; /** UTC date year */
} rmc_message_t;

/**************************************************************************
* \brief NMEA GSV message.
**************************************************************************/
typedef struct STRUCT_GSV_MESSAGE {
	
} gsv_message_t;

/**************************************************************************
* \brief NMEA GSA message.
**************************************************************************/
typedef struct STRUCT_GSA_MESSAGE {
	
} gsa_message_t;

/**************************************************************************
* \brief Contains all important GPS data.
**************************************************************************/
typedef struct STRUCT_GPS {
	gga_message_t gga; /** GGA message */
	rmc_message_t rmc; /** RMC message */
} gps_data_t;


/**************************************************************************
* \brief Initializes the GPS subsystem. Must be called during system startup.
*
* \param gps_data		Pointer to a gps_data_t structure.
**************************************************************************/
void gps_init(gps_data_t* gps_data);

#endif /* GPS_H_ */