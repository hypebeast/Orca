/*
 * shell.c
 *
 * Created: 25.03.2012 21:22:48
 *  Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 */

#include "serial_api.h"


#define CMD_BUFFER_LENGTH 64

static struct {
	char buff[CMD_BUFFER_LENGTH];
	unsigned int index;
	unsigned int match;
} command_buff;

/* API commands */
static void test_command(int argc, char **argv);

/* Enabled API commands */
struct api_command commands[] = {
	{ "test",		test_command}
};


/************************************************************************/
/* API commands implementation                                          */
/************************************************************************/

/* Test API command */
static void test_command(int argc, char **argv)
{
	// TODO
}

/**
* This method initializes the serial interface.
*/
void serial_api_init(void)
{
	// Initialize system clock
	sysclk_init();
	
	// USART options
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_API_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};

	// Initialize usart driver in RS232 mode
	usart_init_rs232(USART_SERIAL_API, &USART_SERIAL_OPTIONS);
	
	command_buff.index = 0;
}

/**
* This method parses an received command.
*/
void parse_command(void)
{
	// TODO	
}

/**
* This method executes an received command.
*/
void execute_command(int argc, char **argv)
{
	// TODO
}

/**
* Main worker method for the serial API.
*/
void serial_api_task(void)
{
	uint8_t received_byte = usart_getchar(USART_SERIAL_API);
	
	// Just for testing
	if (received_byte == '\r') {
		char tx_buffer[] = "\n\rHello from Orca!";
		for (int i = 0; i < 16; i++) {
			usart_putchar(USART_SERIAL_API, tx_buffer[i]);
		}
	}
	
	// Process the received byte
	switch (received_byte) {
		case '\n':
		case '\r':
			if (command_buff.index > 0) {
				parse_command();
				command_buff.index = 0;
				memset(command_buff.buff, 0, sizeof(command_buff.buff));
			}
			break;
			
		case '\b':
			break;
			
		default:
			if (command_buff.index < CMD_BUFFER_LENGTH - 1) {
				command_buff.buff[command_buff.index] = received_byte;
				command_buff.index++;
			}
	}
}