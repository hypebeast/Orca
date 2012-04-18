/*
 * shell.c
 *
 * Created: 25.03.2012 21:22:48
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 */

#include "serial_api.h"


#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#define CMD_BUFFER_LENGTH 128

/* This structure represents an input buffer for incoming commands */
static struct {
	char buff[CMD_BUFFER_LENGTH];
	unsigned int index;
	unsigned int match;
} command_buff;

/************************************************************************/
/* API command definitions                                              */
/************************************************************************/

/* Test API command */
static void test_command(int argc, char **argv);

/* All LEDs on */
static void leds_on(int argc, char **argv);

/* All LEDs off */
static void leds_off(int argc, char **argv);

/* This function parses all servo commands*/
static void process_servo_command(int argc, char **argv);

/* Set new servo position */
static void serial_set_servo_pos(int argc, char **argv);

/* Get the actual position of a servo */
static void serial_get_servo_pos(int argc, char **argv);

/* Enabled API commands */
struct api_command commands[] = {
	{ "test",		test_command },
	{ "ledsoff",	leds_off },
	{ "ledson",		leds_on},
	{ "SERVO",		process_servo_command}
};

/* Enabled servo API commands */
struct api_command servo_commands[] = {
	{ "POS", serial_set_servo_pos},
	{ "GETPOS", serial_get_servo_pos}	
};


/************************************************************************/
/* API commands implementation                                          */
/************************************************************************/

/* Test API command */
static void test_command(int argc, char **argv)
{
	char tx_buffer[] = "Test Command";
	for (int i = 0; i < 14; i++) {
		usart_putchar(USART_SERIAL_API, tx_buffer[i]);
	}
}

static void leds_on(int argc, char **argv)
{
	gpio_set_pin_low(LED0);
	gpio_set_pin_low(LED1);
	gpio_set_pin_low(LED2);
	gpio_set_pin_low(LED3);
	gpio_set_pin_low(LED4);
	gpio_set_pin_low(LED5);
	gpio_set_pin_low(LED6);
	gpio_set_pin_low(LED7);
}

static void leds_off(int argc, char **argv)
{
	gpio_set_pin_high(LED0);
	gpio_set_pin_high(LED1);
	gpio_set_pin_high(LED2);
	gpio_set_pin_high(LED3);
	gpio_set_pin_high(LED4);
	gpio_set_pin_high(LED5);
	gpio_set_pin_high(LED6);
	gpio_set_pin_high(LED7);
}

/**
* \brief This function parses all servo commands
*/
static void process_servo_command(int argc, char **argv)
{
	uint16_t nl = strlen(argv[1]);
	uint16_t cl;
	
	for (int i = 0; i < ARRAY_SIZE(servo_commands); i++) {
		cl = strlen(servo_commands[i].name);
		
		if (cl == nl && servo_commands[i].function != NULL &&
			!strncmp(argv[1], servo_commands[i].name, nl)) {
			servo_commands[i].function(argc, argv);		
		}
	}
}

/**
* \brief This function sets a new position for a servo.
*/
static void serial_set_servo_pos(int argc, char **argv)
{
	if (argc != 4)
		return;
		
	uint16_t servo_nr = 0;
	uint16_t position = 0;

	servo_nr = atoi(argv[2]);
	position = atoi(argv[3]);
	
	// Set the new position
	set_servo_pos(servo_nr, position);
}

/**
* \brief This function returns the actual position of a servo.
*/
static void serial_get_servo_pos(int argc, char **argv)
{
	if (argc != 2)
		return;
		
	uint16_t servo_nr = atoi(argv[1]);
	uint16_t position = get_servo_pos(servo_nr);	
	
	//char cmd_buffer[] = "getservopos";
	//char tx_buffer[] = strtol(position);
}

/**
* This method initializes the serial interface.
*/
void serial_api_init(void)
{
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
* This method checks if the given character is a whitespace character.
*/
static char is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

/**
* This method parses an received command.
*/
void parse_command(void)
{
	uint8_t i;
	char *argv[16];
	int argc = 0;
	char *in_arg = NULL;
	
	for (i = 0; i < command_buff.index; i++) {
		if (is_whitespace(command_buff.buff[i]) && argc == 0)
			continue;
		
		if (is_whitespace(command_buff.buff[i])) {
			if (in_arg) {
				command_buff.buff[i] = '\0';
				in_arg = NULL;
			}
		} else if (!in_arg) {
			in_arg = &command_buff.buff[i];
			argv[argc] = in_arg;
			argc++;
		}
	}
	
	command_buff.buff[i] = '\0';
	
	if (argc > 0)
		execute_command(argc, argv);
}

/**
* This method executes an received command.
*/
void execute_command(int argc, char **argv)
{
	unsigned int i;
	unsigned int nl = strlen(argv[0]);
	unsigned int cl;
	
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		cl = strlen(commands[i].name);
		
		if (cl == nl && commands[i].function != NULL &&
			!strncmp(argv[0], commands[i].name, nl)) {
			commands[i].function(argc, argv);
			//usart_putchar(USART_SERIAL_API, '\n');		
		}
	}
}

/**
* \brief This function writes the given data to the serial connection.
*/
void send_command(char *data)
{
	uint16_t cl = strlen(data);
	
	for (int i = 0; i < cl; i++) {
		usart_putchar(USART_SERIAL_API, data[i]);
	}
	
	usart_putchar(USART_SERIAL_API, '\n');
}

/**
* Main worker method for the serial API.
*/
void serial_api_task(void)
{
	uint8_t received_byte = usart_getchar(USART_SERIAL_API);
	
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