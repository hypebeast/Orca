/**************************************************************************
 * \file gps.c
 *
 * \brief This file contains functions for handling the Fastrax UP501 GPS
 *        module.
 *
 * Created: 04.12.2012 23:33:50
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "gps.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

static gps_data_t* gps_data;

/**************************************************************************
 * \brief USART data struct.
 **************************************************************************/
USART_data_t GPS_USART_data;

uint8_t nmea_state;
uint8_t gga_state;
uint8_t gsv_state;
uint8_t gsa_state;
uint8_t rmc_state;


//////////////////////////////////////////////////////////////////////////
// Private function declarations
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
* \brief Processes received bytes from the GPS unit.
**************************************************************************/
static void process_received_nmea_byte(void);

/**************************************************************************
* \brief Decodes a GGA NMEA message.
*
* Input:
* $GPGGA,114019.489,5048.9524,N,00511.8268,E,1,08,01.4,57.9,M,45.6,M,,*52
* where lat/long can be a variable number of digits: from xxx.y over xxx.yyyy to xxxxx.yyyy
* where height can be a variable number of digits: from x.y to xxxxx.y
*
* Output:
* gps_latitude_degrees[3],gps_longitude_degrees[3], where [0]*100 + [1]*10 + [2] = decimal degrees
* gps_latitude_minutes[2],gps_longitude_minutes[2], where [0]*10 + [1] = decimal minutes
* gps_latitude_decimal_seconds[4],gps_longitude_decimal_seconds[4], where ([0]*1000 + [1]*100 + [2]*10 + [3]) / 10000 * 60 = decimal seconds
* gps_height[5], gps_height_decimeters, where ([0]*10000 + [1]*1000 + [2]*100 + [3]*10 + [4]) + gps_height_decimeters/1° = decimal height
*
* \param received_byte	The received byte.
**************************************************************************/
static void decode_gga_message(uint8_t received_byte);

/**************************************************************************
* \brief Decodes a RMC NMEA message.
**************************************************************************/
static void decode_rmc_message(uint8_t received_byte);

/**************************************************************************
* \brief Decodes a GSV NMEA message.
**************************************************************************/
static void decode_gsv_message(uint8_t received_byte);

/**************************************************************************
* \brief Decodes a GSA NMEA message.
**************************************************************************/
static void decode_gsa_message(uint8_t received_byte);

/**************************************************************************
* \brief Calculates the CRC for the given message.
**************************************************************************/
static uint8_t nmea_calc_crc(uint8_t *s);

/**************************************************************************
* \brief Writes a NMEA message to the serial connection.
*
* \param message	The NMEA message to send.	
* \param length		The length of the message.
**************************************************************************/
static void nmea_write_message(uint8_t *message, uint8_t length);


//////////////////////////////////////////////////////////////////////////
// Function definitions
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
* \brief Decodes GGA NMEA messages.
*
* Input:
* $GPGGA,114019.489,5048.9524,N,00511.8268,E,1,08,01.4,57.9,M,45.6,M,,*52
* where lat/long can be a variable number of digits: from xxx.y over xxx.yyyy to xxxxx.yyyy
* where height can be a variable number of digits: from x.y to xxxxx.y
* Output:
* gps_latitude_degrees[3],gps_longitude_degrees[3], where [0]*100 + [1]*10 + [2] = decimal degrees
* gps_latitude_minutes[2],gps_longitude_minutes[2], where [0]*10 + [1] = decimal minutes
* gps_latitude_decimal_seconds[4],gps_longitude_decimal_seconds[4], where ([0]*1000 + [1]*100 + [2]*10 + [3]) / 10000 * 60 = decimal seconds
* gps_height[5], gps_height_decimeters, where ([0]*10000 + [1]*1000 + [2]*100 + [3]*10 + [4]) + gps_height_decimeters/1° = decimal height
*
* \param received_byte	The received byte.
**************************************************************************/
static void decode_gga_message(uint8_t received_byte)
{
	switch (gga_state) {
		case 1:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 2:
			// First digit of the UTC hours
			gps_data->gga.utc_time_hours[0] = received_byte;
			gga_state++;
			break;
			
		case 3:
			// Second digit of the UTC hours
			gps_data->gga.utc_time_hours[1] = received_byte;
			gga_state++;
			break;
			
		case 4:
			// First digit of the UTC minutes
			gps_data->gga.utc_time_minutes[0] = received_byte;
			gga_state++;
			break;
			
		case 5:
			// Second digit of the UTC minutes
			gps_data->gga.utc_time_minutes[1] = received_byte;
			gga_state++;
			break;
			
		case 6:
			// First digit of the UTC seconds
			gps_data->gga.utc_time_seconds[0] = received_byte;
			gga_state++;
			break;
			
		case 7:
			// Second digit of the UTC seconds
			gps_data->gga.utc_time_seconds[1] = received_byte;
			gga_state++;
			break;
			
		case 8:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				gga_state++;
			}
			break;
			
		case 9:
			// First digit of the decimal part of seconds
			gps_data->gga.utc_time_decimal_seconds[0] = received_byte;
			gga_state++;
			break;
		
		case 10:
			// First digit of the decimal part of seconds
			gps_data->gga.utc_time_decimal_seconds[1] = received_byte;
			gga_state++;
			break;
			
		case 11:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 12:
			// First digit of the latitude degrees
			gps_data->gga.latitude_degrees[0] = received_byte;
			gga_state++;
			break;
			
		case 13:
			// Second digit of the latitude degrees
			gps_data->gga.latitude_degrees[1] = received_byte;
			gga_state++;
			break;
			
		case 14:
			// First digit of the latitude minutes
			gps_data->gga.latitude_minutes[0] = received_byte;
			gga_state++;
			break;
			
		case 15:
			// Second digit of the latitude minutes
			gps_data->gga.latitude_minutes[1] = received_byte;
			gga_state++;
			break;
		
		case 16:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				gga_state++;
			}
			break;
		
		case 17:
			// First digit of the latitude decimal part of minutes
			gps_data->gga.utc_time_decimal_seconds[0] = received_byte;
			gga_state++;
			break;
		
		case 18:
			// Second digit of the latitude decimal part of minutes
			gps_data->gga.utc_time_decimal_seconds[1] = received_byte;
			gga_state++;
			break;
		
		case 19:
			// Third digit of the latitude decimal part of minutes
			gps_data->gga.utc_time_decimal_seconds[2] = received_byte;
			gga_state++;
			break;
			
		case 20:
			// Fourth digit of the latitude decimal part of minutes
			gps_data->gga.utc_time_decimal_seconds[3] = received_byte;
			gga_state++;
			break;
			
		case 21:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 22:
			// Latitude direction
			gps_data->gga.latitude_direction = received_byte;
			gga_state++;
			break;
			
		case 23:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 24:
			// First digit of the longitude degrees
			gps_data->gga.longitude_degrees[0] = received_byte;
			gga_state++;
			break;
			
		case 25:
			// Second digit of the longitude degrees
			gps_data->gga.longitude_degrees[1] = received_byte;
			gga_state++;
			break;
			
		case 26:
			// Third digit of the longitude degrees
			gps_data->gga.longitude_degrees[2] = received_byte;
			gga_state++;
			break;
			
		case 27:
			// First digit of the longitude minutes
			gps_data->gga.longitude_minutes[0] = received_byte;
			gga_state++;
			break;
			
		case 28:
			// Second digit of the longitude minutes
			gps_data->gga.longitude_minutes[1] = received_byte;
			gga_state++;
			break;
			
		case 29:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				gga_state++;
			}
			break;
			
		case 30:
			// First digit of the longitude decimal part of minutes
			gps_data->gga.longitude_decimal_minutes[0] = received_byte;
			gga_state++;
			break;
			
		case 31:
			// Second digit of the longitude decimal part of minutes
			gps_data->gga.longitude_decimal_minutes[1] = received_byte;
			gga_state++;
			break;
		
		case 32:
			// Third digit of the longitude decimal part of minutes
			gps_data->gga.longitude_decimal_minutes[2] = received_byte;
			gga_state++;
			break;
		
		case 33:
			// Fourth digit of the longitude decimal part of minutes
			gps_data->gga.longitude_decimal_minutes[3] = received_byte;
			gga_state++;
			break;
			
		case 34:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 35:
			// Longitude direction
			gps_data->gga.latitude_direction = received_byte;
			gga_state++;
			break;
			
		case 36:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 37:
			// Fix valid indicator
			gps_data->gga.fix_valid_indicator = received_byte;
			gga_state++;
			break;
			
		case 38:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 39:
			// First digit of number of satellites
			gps_data->gga.number_of_satellites[0] = received_byte;
			gga_state++;
			break;
			
		case 40:
			// Second digit of number of satellites
			gps_data->gga.number_of_satellites[1] = received_byte;
			gga_state++;
			break;
			
		case 41:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 42:
			// HDOP: Supported formats: x.x to x.xxx
			gps_data->gga.hdop = received_byte;
			gga_state++;
			break;
			
		case 43:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				gga_state++;
			}
			break;
			
		case 44:
			// HDOP fractional part: minimum digits before the next comma is 1
			gps_data->gga.hdop_fractional_part[0] = received_byte;
			gps_data->gga.hdop_fractional_part[1] = 0x30;
			gps_data->gga.hdop_fractional_part[2] = 0x30;
			gga_state++;
			break;
			
		case 45:
			// Check if we have one more digit or the next comma
			if (received_byte == 0x2c)
			{
				gga_state = 48;
			}
			else
			{
				gps_data->gga.hdop_fractional_part[1] = received_byte;
				gga_state++;	
			}
			break;
			
		case 46:
			// Check if we got the last digit or a comma
			if (received_byte == 0x2c)
			{
				gga_state = 48;
			}
			else
			{
				gps_data->gga.hdop_fractional_part[2] = received_byte;
				gga_state++;
			}
			break;
			
		case 47:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				gga_state++;
			}
			break;
			
		case 48:
			// Altitude: Supported formats are h.h to hhhhh.h
			gps_data->gga.altitude[0] = 0x30;
			gps_data->gga.altitude[1] = 0x30;
			gps_data->gga.altitude[2] = 0x30;
			gps_data->gga.altitude[3] = 0x30;
			gps_data->gga.altitude[4] = received_byte;
			gga_state++;
			break;
			
		case 49:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				gga_state = 54;
			}
			else
			{
				// We need to do some reordering
				gps_data->gga.altitude[3] = gps_data->gga.altitude[4];
				gps_data->gga.altitude[4] = received_byte;
				gga_state++;
			}
			break;
			
		case 50:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				gga_state = 54;
			}
			else
			{
				// We need to do some reordering
				gps_data->gga.altitude[2] = gps_data->gga.altitude[3];
				gps_data->gga.altitude[3] = gps_data->gga.altitude[4];
				gps_data->gga.altitude[4] = received_byte;
				gga_state++;
			}
			break;
			
		case 51:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				gga_state = 54;
			}
			else
			{
				// We need to do some reordering
				gps_data->gga.altitude[1] = gps_data->gga.altitude[2];
				gps_data->gga.altitude[2] = gps_data->gga.altitude[3];
				gps_data->gga.altitude[3] = gps_data->gga.altitude[4];
				gps_data->gga.altitude[4] = received_byte;
				gga_state++;
			}
			break;
			
		case 52:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				gga_state = 54;
			}
			else
			{
				// We need to do some reordering
				gps_data->gga.altitude[0] = gps_data->gga.altitude[1];
				gps_data->gga.altitude[1] = gps_data->gga.altitude[2];
				gps_data->gga.altitude[2] = gps_data->gga.altitude[3];
				gps_data->gga.altitude[3] = gps_data->gga.altitude[4];
				gps_data->gga.altitude[4] = received_byte;
				gga_state++;
			}
			break;
			
		case 53:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				gga_state++;
			}
			break;
			
		case 54:
			// Altitude decimeters
			gps_data->gga.altitude_decimeters = received_byte;
			gga_state++;
			break;
			
		case 55:
			// Where are done. The rest of the message is not interesting for us
			gga_state = 1;
			nmea_state = 1;
			break;
			
		default:
			gga_state = 1;
			nmea_state = 1;
			break;
	}
}

/**************************************************************************
* \brief Decodes a RMC NMEA message.
**************************************************************************/
static void decode_rmc_message(uint8_t received_byte)
{
	switch (rmc_state)
	{
		case 1:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
		
		case 2:
			// First digit of the UTC hours
			gps_data->rmc.utc_time_hours[0] = received_byte;
			rmc_state++;
			break;
		
		case 3:
			// Second digit of the UTC hours
			gps_data->rmc.utc_time_hours[1] = received_byte;
			rmc_state++;
			break;
		
		case 4:
			// First digit of the UTC minutes
			gps_data->rmc.utc_time_minutes[0] = received_byte;
			rmc_state++;
			break;
		
		case 5:
			// Second digit of the UTC minutes
			gps_data->rmc.utc_time_minutes[1] = received_byte;
			rmc_state++;
			break;
		
		case 6:
			// First digit of the UTC seconds
			gps_data->rmc.utc_time_seconds[0] = received_byte;
			rmc_state++;
			break;
		
		case 7:
			// Second digit of the UTC seconds
			gps_data->rmc.utc_time_seconds[1] = received_byte;
			rmc_state++;
			break;
		
		case 8:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				rmc_state++;
			}
			break;
		
		case 9:
			// First digit of the decimal part of seconds
			gps_data->rmc.utc_time_decimal_seconds[0] = received_byte;
			rmc_state++;
			break;
		
		case 10:
			// First digit of the decimal part of seconds
			gps_data->rmc.utc_time_decimal_seconds[1] = received_byte;
			rmc_state++;
			break;
		
		case 11:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
		
		case 12:
			// Status
			gps_data->rmc.status = received_byte;
			rmc_state++;
			break;
			
		case 13:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
		
		case 14:
			// First digit of the latitude degrees
			gps_data->rmc.latitude_degrees[0] = received_byte;
			rmc_state++;
			break;
		
		case 15:
			// Second digit of the latitude degrees
			gps_data->rmc.latitude_degrees[1] = received_byte;
			rmc_state++;
			break;
		
		case 16:
			// First digit of the latitude minutes
			gps_data->rmc.latitude_minutes[0] = received_byte;
			rmc_state++;
			break;
		
		case 17:
			// Second digit of the latitude minutes
			gps_data->rmc.latitude_minutes[1] = received_byte;
			rmc_state++;
			break;
		
		case 18:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				rmc_state++;
			}
			break;
		
		case 19:
			// First digit of the latitude decimal part of minutes
			gps_data->rmc.utc_time_decimal_seconds[0] = received_byte;
			rmc_state++;
			break;
		
		case 20:
			// Second digit of the latitude decimal part of minutes
			gps_data->rmc.utc_time_decimal_seconds[1] = received_byte;
			rmc_state++;
			break;
		
		case 21:
			// Third digit of the latitude decimal part of minutes
			gps_data->rmc.utc_time_decimal_seconds[2] = received_byte;
			rmc_state++;
			break;
		
		case 22:
			// Fourth digit of the latitude decimal part of minutes
			gps_data->rmc.utc_time_decimal_seconds[3] = received_byte;
			rmc_state++;
			break;
		
		case 23:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
		
		case 24:
			// Latitude direction
			gps_data->rmc.latitude_direction = received_byte;
			rmc_state++;
			break;
		
		case 25:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
			
		case 26:
			// First digit of the longitude degrees
			gps_data->rmc.longitude_degrees[0] = received_byte;
			rmc_state++;
			break;
		
		case 27:
			// Second digit of the longitude degrees
			gps_data->rmc.longitude_degrees[1] = received_byte;
			rmc_state++;
			break;
		
		case 28:
			// Third digit of the longitude degrees
			gps_data->rmc.longitude_degrees[2] = received_byte;
			rmc_state++;
			break;
		
		case 29:
			// First digit of the longitude minutes
			gps_data->rmc.longitude_minutes[0] = received_byte;
			rmc_state++;
			break;
		
		case 30:
			// Second digit of the longitude minutes
			gps_data->rmc.longitude_minutes[1] = received_byte;
			rmc_state++;
			break;
		
		case 31:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				rmc_state++;
			}
			break;
		
		case 32:
			// First digit of the longitude decimal part of minutes
			gps_data->rmc.longitude_decimal_minutes[0] = received_byte;
			rmc_state++;
			break;
		
		case 33:
			// Second digit of the longitude decimal part of minutes
			gps_data->rmc.longitude_decimal_minutes[1] = received_byte;
			rmc_state++;
			break;
		
		case 34:
			// Third digit of the longitude decimal part of minutes
			gps_data->rmc.longitude_decimal_minutes[2] = received_byte;
			rmc_state++;
			break;
		
		case 35:
			// Fourth digit of the longitude decimal part of minutes
			gps_data->rmc.longitude_decimal_minutes[3] = received_byte;
			rmc_state++;
			break;
		
		case 36:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
		
		case 37:
			// Longitude direction
			gps_data->rmc.latitude_direction = received_byte;
			rmc_state++;
			break;
			
		case 38:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
			
		case 39:
			// Speed: Supported formats are from x.x to xxxx.xx
			gps_data->rmc.speed[0] = 0x30;
			gps_data->rmc.speed[1] = 0x30;
			gps_data->rmc.speed[2] = 0x30;
			gps_data->rmc.speed[3] = received_byte;
			rmc_state++;
			break;
			
		case 40:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				rmc_state = 44;
			}
			else
			{
				// We need to do some reordering
				gps_data->rmc.speed[2] = gps_data->rmc.speed[3];
				gps_data->rmc.speed[3] = received_byte;
				rmc_state++;
			}
			break;
			
		case 41:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				rmc_state = 44;
			}
			else
			{
				// We need to do some reordering
				gps_data->rmc.speed[1] = gps_data->rmc.speed[2];
				gps_data->rmc.speed[2] = gps_data->rmc.speed[3];
				gps_data->rmc.speed[3] = received_byte;
				rmc_state++;
			}
			break;
			
		case 42:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				rmc_state = 44;
			}
			else
			{
				// We need to do some reordering
				gps_data->rmc.speed[0] = gps_data->rmc.speed[1];
				gps_data->rmc.speed[1] = gps_data->rmc.speed[2];
				gps_data->rmc.speed[2] = gps_data->rmc.speed[3];
				gps_data->rmc.speed[3] = received_byte;
				rmc_state++;
			}
			break;
			
		case 43:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				rmc_state++;
			}
			break;
			
		case 44:
			// Speed decimal part
			gps_data->rmc.speed_decimal[0] = received_byte;
			gps_data->rmc.speed_decimal[1] = 0x30;
			rmc_state++;
			break;
			
		case 45:
			// Check if we received on more digit or a comma
			if (received_byte == 0x2c)
			{
				rmc_state = 47;
			}
			else
			{
				gps_data->rmc.speed_decimal[1] = received_byte;
				rmc_state++;
			}
			break;
			
		case 46:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
			
		case 47:
			// Heading: Supported formats are from x.x to xxx.xx
			gps_data->rmc.heading[0] = 0x30;
			gps_data->rmc.heading[1] = 0x30;
			gps_data->rmc.heading[2] = received_byte;
			rmc_state++;
			break;
			
		case 48:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				rmc_state = 51;
			}
			else
			{
				// We need to do some reordering
				gps_data->rmc.heading[1] = gps_data->rmc.heading[2];
				gps_data->rmc.heading[2] = received_byte;
				rmc_state++;
			}
			break;
			
		case 49:
			// Check if we received one more digit or the decimal point
			if (received_byte == 0x2e)
			{
				rmc_state = 51;
			}
			else
			{
				// We need to do some reordering
				gps_data->rmc.heading[0] = gps_data->rmc.heading[1];
				gps_data->rmc.heading[1] = gps_data->rmc.heading[2];
				gps_data->rmc.heading[2] = received_byte;
				rmc_state++;
			}
			break;
			
		case 50:
			// Wait for '.'
			if (received_byte == 0x2e)
			{
				rmc_state++;
			}
			break;
			
		case 51:
			// Heading: decimal part
			gps_data->rmc.heading_decimal[0] = received_byte;
			gps_data->rmc.heading_decimal[1] = 0x30;
			break;
			
		case 52:
			// Check if we received on more digit or a comma
			if (received_byte == 0x2c)
			{
				rmc_state = 54;
			}
			else
			{
				gps_data->rmc.heading_decimal[1] = received_byte;
				rmc_state++;
			}
			break;
			
		case 53:
			// Wait for ','
			if (received_byte == 0x2c)
			{
				rmc_state++;
			}
			break;
			
		case 54:
			// First digit of the UTC date day
			gps_data->rmc.utc_date_day[0] = received_byte;
			rmc_state++;
			break;
			
		case 55:
			// Second digit of the UTC date day
			gps_data->rmc.utc_date_day[1] = received_byte;
			rmc_state++;
			break;
			
		case 56:
			// First digit of the UTC date month
			gps_data->rmc.utc_date_month[0] = received_byte;
			rmc_state++;
			break;
			
		case 57:
			// Second digit of the UTC date month
			gps_data->rmc.utc_date_month[1] = received_byte;
			rmc_state++;
			break;
			
		case 58:
			// First digit of the UTC date year
			gps_data->rmc.utc_date_year[0] = received_byte;
			rmc_state++;
			break;
			
		case 59:
			// Second digit of the UTC date year
			gps_data->rmc.utc_date_year[1] = received_byte;
			rmc_state++;
			break;
			
		case 60:
			// We're done. We don't care about the rest of the message
			rmc_state = 1;
			nmea_state = 1;
			break;
			
		default:
			// This should never happen, but in case we reset the state
			rmc_state = 1;
			nmea_state = 1;
			break;
	}
}

/**************************************************************************
* \brief Decodes a GSV NMEA message.
**************************************************************************/
static void decode_gsv_message(uint8_t received_byte)
{
	
}

/**************************************************************************
* \brief Decodes a GGA NMEA message.
**************************************************************************/
static void decode_gsa_message(uint8_t received_byte)
{
	
}

/**************************************************************************
* \brief Processes received bytes from the GPS unit.
**************************************************************************/
static void process_received_nmea_byte(void)
{
	uint8_t received_byte;
	
	while(USART_RXBufferData_Available(&GPS_USART_data)){
		received_byte = USART_RXBuffer_GetByte(&GPS_USART_data);
		
		switch (nmea_state) {
			case 1:
				// Wait for '$'
				if (received_byte == 0x24)
				{
					nmea_state++;
				}
				break;
				
			case 2:
				// Wait for 'G'
				if (received_byte == 0x47)
				{
					nmea_state++;
				}
				else nmea_state = 1;
				break;
				
			case 3:
				// Wait for 'P'
				if (received_byte == 0x50)
				{
					nmea_state++;
				}
				else nmea_state = 1;
				break;
				
			case 4:
				// Wait for GGA, RMC, GSV and GSA messages
				
				// Wait for 'G'
				if (received_byte == 0x47)
				{
					nmea_state = 5;
				}
				// Wait for 'R'
				else if (received_byte == 0x52)
				{
					nmea_state = 6;
				}
				else nmea_state = 1;
				break;
				
			case 5:
				// Wait for 'G'
				if (received_byte == 0x47)
				{
					nmea_state = 7;
				}
				// Wait for 'S'
				else if (received_byte == 0x53)
				{
					nmea_state = 8;
				}
				else nmea_state = 1;
				break;
				
			case 6:
				// Wait for 'M'
				if (received_byte == 0x4D)
				{
					nmea_state = 9;
				}
				else nmea_state = 1;
				break;
				
			case 7:
				// Wait for 'A'
				if (received_byte == 0x41)
				{
					// GGA message
					nmea_state = 10;
				}
				else nmea_state = 1;
				break;
				
			case 8:
				// Wait for 'V'
				if (received_byte == 0x56)
				{
					// GSV message
					nmea_state = 11;
				}
				// Wait for 'A'
				else if (received_byte == 0x41)
				{
					// GSA message
					nmea_state = 12;
				}
				else nmea_state = 1;
				break;
				
			case 9:
				// Wait for 'C'
				if (received_byte == 0x43)
				{
					// RMC message
					nmea_state = 13;
				}
				else nmea_state = 1;
				break;
				
			case 10:
				// Decode GGA message
				decode_gga_message(received_byte);
				break;
				
			case 11:
				// Decode GSV message
				decode_gsv_message(received_byte);
				break;
				
			case 12:
				// Decode GSA message
				decode_gsa_message(received_byte);
				break;
				
			case 13:
				// Decode RMC message
				decode_rmc_message(received_byte);
				break;
			
			default:
				break;
		}	
	}
}

/**************************************************************************
* \brief Writes a NMEA message to the serial connection.
*
* \param message	The NMEA message to send.
* \param length		The length of the message.
**************************************************************************/
static void nmea_write_message(uint8_t *message, uint8_t length)
{
	int i = 0;
	while (i < length)
	{
		bool byteToBuffer;
		byteToBuffer = USART_TXBuffer_PutByte(&GPS_USART_data, message[i]);
		if (byteToBuffer)
		{
			i++;
		}
	}
}

/**************************************************************************
* \brief Calculates the CRC for the given message.
**************************************************************************/
static uint8_t nmea_calc_crc(uint8_t *s)
{
	uint8_t c = 0;
	
	while(*s)
		c ^= *s++;
	
	return c;
}

/**************************************************************************
* \brief Initializes the GPS subsystem. Must be called during system startup.
*
* \param gps_data		Pointer to a gps_data_t structure.
**************************************************************************/
void gps_init(gps_data_t* gps)
{
	gps_data = gps;
	nmea_state = 1;
	gga_state = 1;
	gsv_state = 1;
	gsa_state = 1;
	rmc_state = 1;
	
	// Enable the system clock for the serial interface
	sysclk_enable_peripheral_clock(GPS_USART_INTERFACE);
	
	// Use USARTE0 and initialize buffers
	USART_InterruptDriver_Initialize(&GPS_USART_data, GPS_USART_INTERFACE,
										USART_DREINTLVL_HI_gc);
	
	// USARTE0, 8 data bits, no parity, 1 stop bit
	USART_Format_Set(GPS_USART_data.usart, USART_CHSIZE_8BIT_gc,
						USART_PMODE_DISABLED_gc, false);
	
	// Enable RXC interrupt
	USART_RxdInterruptLevel_Set(GPS_USART_data.usart, USART_RXCINTLVL_HI_gc);
	
	// Set baudrate to 9600 bps; scale factor is set to zero
	// BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
	// BSEL for 9600 bps => 207
	// BSEL for 57600 bps => 33 (33.722)
	USART_Baudrate_Set(GPS_USART_data.usart, 207, 0);
	
	// Enable both RX and TX
	USART_Rx_Enable(GPS_USART_data.usart);
	USART_Tx_Enable(GPS_USART_data.usart);
}

/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
ISR(USARTF0_RXC_vect)
{
	USART_RXComplete(&GPS_USART_data);
	
	// Process received byte(s)
	process_received_nmea_byte();
}

/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTF0_DRE_vect)
{
	USART_DataRegEmpty(&GPS_USART_data);
}