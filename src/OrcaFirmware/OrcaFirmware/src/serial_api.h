/**
 * \file serial_api.h
 *
 * \brief Header file for serial_api.c
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */


#ifndef SERIALAPI_H_
#define SERIALAPI_H_

#include <string.h>
#include <asf.h>
#include <stdlib.h>

#include "servo.h"
#include "user_board.h"

#define USART_SERIAL_API	             BOARD_USART_USB_INTERFACE
#define USART_SERIAL_API_BAUDRATE		 9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

#define DATA_BUFFER_LENGTH 255
#define MAX_PACKET_LENGTH (1+1+2+DATA_BUFFER_LENGTH+1)
#define PACKET_START_STOP_DELIMITER 0x8D

/**
* This struct defines an API command and it's processing function.
*/
struct api_command {
	char *name;
	uint16_t code;
	void (*function)(int argc, char **argv);
};

/**
* This struct defines an serial API command packet.
*/
typedef struct serial_api_packet {
	uint8_t start_delimiter;
	uint8_t message_type;
	uint8_t data_length;
	uint16_t command_type;
	uint8_t data[DATA_BUFFER_LENGTH];
	uint8_t crc;
} SERIAL_API_PACKET_t;


/* Function declarations */

void serial_api_init(void);
void serial_api_task(void);
void parse_command_packet(void);
void execute_command(int argc, char **argv);
void write_command(char *data);
void create_command_packet(void);
void write_command_packet(void);

#endif /* SERIALAPI_H_ */