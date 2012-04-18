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

#define USART_SERIAL_API	             &USARTC0
#define USART_SERIAL_API_BAUDRATE		 9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

/**
* This struct defines an API command
*/
struct api_command {
	char *name;
	void (*function)(int argc, char **argv);
};

void serial_api_init(void);
void serial_api_task(void);
void parse_command(void);
void execute_command(int argc, char **argv);
void send_command(char *data);

#endif /* SERIALAPI_H_ */