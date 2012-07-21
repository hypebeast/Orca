/*
 * shell.c
 *
 * Created: 25.03.2012 21:22:48
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 */

#include "serial_api.h"

/*
 * Defines
 */
#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))
#define CMD_BUFFER_LENGTH 128

/*
 * Global variables
 */

/** \brief This structure represents an input buffer for incoming API commands.
 *
 */
static struct {
	/* Buffer for the command packet */
	char buff[MAX_PACKET_LENGTH];
	unsigned int index;
	bool start_received;
	unsigned int match;
} command_buff;

SERIAL_API_PACKET_t receive_command_packet;
SERIAL_API_PACKET_t transmit_command_packet;

/************************************************************************/
/* API command definitions                                              */
/************************************************************************/

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

/* This function parses all engine commands */
static void process_engine_command(int argc, char **argv);

/* This function sets a new value for the given engine */
static void serial_engine_set_value(int argc, char **argv);

/* Enabled API commands */
struct api_command commands[] = {
	{ "SERVO", process_servo_command},
	{ "ENGINE",	process_engine_command}
};

/* Enabled servo API commands */
struct api_command servo_commands[] = {
	{ "POS", serial_set_servo_pos},
	{ "GETPOS", serial_get_servo_pos}	
};

/* Enabled engine API commands */
struct api_command engine_commands[] = {
	{ "VALUE", serial_engine_set_value}	
};


/************************************************************************/
/* API commands implementation                                          */
/************************************************************************/


/**
* \brief This function parses all engine commands.
*/
static void process_engine_command(int argc, char **argv)
{
	// Get the length of the sub command
	uint16_t nl = strlen(argv[1]);
	uint16_t cl;
	
	for (int i = 0; i < ARRAY_SIZE(engine_commands); i++) {
		cl = strlen(engine_commands[i].name);
		
		if (cl == nl && engine_commands[i].function != NULL &&
		!strncmp(argv[1], engine_commands[i].name, nl)) {
			engine_commands[i].function(argc, argv);
		}
	}
}

/**
* \brief This functions sets a new value for the given engine.
*/
static void serial_engine_set_value(int argc, char **argv)
{
	
}

/**
* \brief This function parses all servo commands.
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
	servo_set_pos_degree(servo_nr, position);
}

/**
* \brief This function returns the actual position of a servo.
*/
static void serial_get_servo_pos(int argc, char **argv)
{
	//if (argc != 3)
		//return;
		
	//uint16_t servo_nr = atoi(argv[2]);
	//uint16_t position = servo_get_pos_degree(servo_nr);	
	//sprintf()
	usart_putchar(USART_SERIAL_API, '\n');
	usart_putchar(USART_SERIAL_API, '\r');
	usart_putchar(USART_SERIAL_API, 'c');
	//usart_putchar(USART_SERIAL_API, '\r');
	//usart_putchar(USART_SERIAL_API, '\n');
	//char resp[] = "200";
	//write_command(resp);
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
	command_buff.start_received = false;
}

/**
* This method checks if the given character is a whitespace character.
*/
static char is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
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
void write_command(char *data)
{
	uint16_t cl = strlen(data);
	
	for (int i = 0; i < cl; i++) {
		usart_putchar(USART_SERIAL_API, data[i]);
	}
	
	usart_putchar(USART_SERIAL_API, '\r');
	usart_putchar(USART_SERIAL_API, '\n');
}

/** \brief This method parses a received command packet.
* 
*/
void parse_command_packet(void)
{
	uint8_t i;
	char *argv[16];
	int argc = 0;
	char *in_arg = NULL;
	uint16_t index = 0;
	
	// The first byte is the start delimiter
	receive_command_packet.start_delimiter = command_buff.buff[index++];
	
	// The second byte is the message type
	receive_command_packet.message_type = command_buff.buff[index++];
	
	// The third byte is the data length
	receive_command_packet.data_length = command_buff.buff[index++];
	
	// Byte four and five contains the command type
	receive_command_packet.command_type = 0;
	receive_command_packet.command_type = (receive_command_packet.command_type << 8) | command_buff.buff[index++];
	receive_command_packet.command_type = (receive_command_packet.command_type << 8) | command_buff.buff[index++];
	
	//receive_command_packet.data = memcp
	
	for (i = 0; i < command_buff.index; i++) {
		// Remove all leading whitespaces
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
* Main worker method for the serial API.
*/
void serial_api_task(void)
{
	//uint8_t received_byte = usart_getchar(USART_SERIAL_API);
	uint8_t received_byte;
	
	if (usart_rx_is_complete(USART_SERIAL_API))
	{
		received_byte = (uint8_t) (USART_SERIAL_API)->DATA;

		// Process the received byte
		switch (received_byte) {
			case PACKET_START_STOP_DELIMITER:
				// Start delimiter received
				if (!command_buff.start_received) {
					command_buff.start_received = true;
					command_buff.buff[command_buff.index] = PACKET_START_STOP_DELIMITER;
					command_buff.index++;
				}
				// Stop delimiter received
				else if (command_buff.start_received) {
					if (command_buff.index > 0) {
						command_buff.buff[command_buff.index] = PACKET_START_STOP_DELIMITER;
						parse_command_packet();
						command_buff.index = 0;
						command_buff.start_received = false;
						memset(command_buff.buff, 0, sizeof(command_buff.buff));
					}
				}
				break;
			
			default:
				if (command_buff.start_received) {
					if (command_buff.index < MAX_PACKET_LENGTH - 1) {
						command_buff.buff[command_buff.index] = received_byte;
						command_buff.index++;
					}
				}
				break;
		}
	}
}