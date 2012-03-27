/*
 * shell.h
 *
 * Created: 25.03.2012 21:22:36
 *  Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 */ 

/**
 * \file serial_api.h
 *
 * \brief Header file for serial_api.c
 *
 * Copyright (c) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
 *
 */


#ifndef SHELL_H_
#define SHELL_H_

#include <string.h>

#include "asf.h"

#define USART_SERIAL_API	             &USARTC0
#define USART_SERIAL_API_BAUDRATE		 9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

struct api_command {
	char *name;
	void (*function)(int argc, char **argv);
};

void serial_api_init(void);
void serial_api_task(void);
void parse_command(void);
void execute_command(int argc, char **argv);

#endif /* SHELL_H_ */