/**************************************************************************
 * \file serial_api.h
 *
 * \brief Header file for serial_api.c
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 **************************************************************************/


#ifndef SERIALAPI_H_
#define SERIALAPI_H_

#include <string.h>
#include <asf.h>
#include <stdlib.h>

#include "servo.h"
#include "user_board.h"

#define USART_SERIAL_API_INTERFACE	     BOARD_USART_USB_INTERFACE
#define USART_SERIAL_API_BAUDRATE		 9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            true

#define MAX_DATA_LENGTH 256
#define MAX_PACKET_LENGTH (1+1+2+1+MAX_DATA_LENGTH+1+1)
#define PACKET_START_BYTE 0x8D
#define PACKET_STOP_BYTE 0x7E


/******************************************************************************
* \brief Buffer structure for a received message.
******************************************************************************/
typedef struct command_buffer {
	unsigned int index; /** Index for the buffer */
	uint8_t buff[MAX_PACKET_LENGTH]; /** Buffer for the command packet */
	bool start_received; /** Flag that indicates if a start delimiter was received */
	bool processing_active; /** Flag that indicates if a messages is currently processed */
} Command_Buffer_t;

/******************************************************************************
* \brief This struct maps a specific command type to the processing function.
******************************************************************************/
struct api_command {
	uint16_t command_type;
	void (*function)();
};

/******************************************************************************
* \brief This struct defines a serial API command.
******************************************************************************/
typedef struct serial_api_packet {
	uint8_t start_delimiter; /** The start delimiter */
	uint8_t message_type; /** The type of the message */
	uint16_t command; /** The command ID */
	uint8_t data_length; /** The length of the data */
	uint8_t data[MAX_DATA_LENGTH]; /** The actual data */
	uint8_t crc; /** CRC checksum for this message */
	uint8_t stop_delimiter; /** The final stop delimiter */
} SERIAL_API_PACKET_t;


/**************************************************************************
* \brief This function initializes the serial interface.
**************************************************************************/
void serial_api_init(void);

/**************************************************************************
* \brief Main worker method for the serial API.
**************************************************************************/
void serial_api_task(void);

/**************************************************************************
* \brief Writes the given packet to the serial connection.
*
* \param data		Pointer to the data
* \param length		Length of the data in bytes
**************************************************************************/
void write_packet(uint8_t *data, uint16_t length);

/**************************************************************************
* \brief Writes a debug message to the serial line. The debug message contains
*        five integer (16 bit) values.
*
* \param val1		First value
* \param val2		Second value
* \param val3		Third value
* \param val4		Fourth value
* \param val5		Fifth value
**************************************************************************/
void write_debug_message_int(uint16_t val1, uint16_t val2, uint16_t val3,
								uint16_t val4, uint16_t val5);

/**************************************************************************
* \brief Writes a debug message to the serial line. The debug message contains
*        five float values.
*
* \param val1		First value
* \param val2		Second value
* \param val3		Third value
* \param val4		Fourth value
* \param val5		Fifth value
**************************************************************************/
void write_debug_message_float(float val1, float val2, float val3, float val4,
								float val5);

/**************************************************************************
* \brief Writes a debug message to the serial line. The message is a string.
*
* \param message			String to write to the serial line.
* \param message_length		The length of the message.
**************************************************************************/
void write_debug_message_string(char* message, uint8_t message_length);

#endif /* SERIALAPI_H_ */