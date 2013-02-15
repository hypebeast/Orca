/**************************************************************************
 * \file serial_api.c
 *
 * \brief Serial command interface implementation.
 *
 * Created: 25.03.2012 21:22:48
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include <string.h>

#include "serial_api.h"
#include "crc8.h"
#include "servo_in.h"
#include "MPU6000.h"
#include "flight_controller.h"
#include "filters.h"
#include "serial_flash.h"
#include "usart.h"


//////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))


//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
 * \brief USART data struct.
 **************************************************************************/
USART_data_t USART_data;

/**************************************************************************
 * \brief Input buffer for incoming packets.
 **************************************************************************/
Command_Buffer_t command_buff;

/**************************************************************************
 * \brief RX command packet
 **************************************************************************/
SERIAL_API_PACKET_t rx_command_packet;

/**************************************************************************
 * \brief TX command packet
 **************************************************************************/
SERIAL_API_PACKET_t tx_command_packet;


//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
* \brief This method parses a received command packet.
**************************************************************************/
static void parse_command_packet(void);

/******************************************************************************
* \brief This method executes the appropriate function for the received command.
*******************************************************************************/
static void execute_command(void);

/**************************************************************************
* \brief Sets new servo position
**************************************************************************/
static void command_set_servo_pos(void);

/**************************************************************************
* \brief Gets the actual position of a servo
**************************************************************************/
static void command_get_servo_pos(void);

/**************************************************************************
* \brief Returns the position of all servos
**************************************************************************/
static void command_get_all_servo_pos(void);

/**************************************************************************
* \brief Returns the board status.
**************************************************************************/
static void command_get_board_status(void);

/**************************************************************************
* \brief Returns the board settings.
**************************************************************************/
static void command_get_board_settings(void);

/**************************************************************************
* \brief Saves the board settings to the flash memory.
**************************************************************************/
static void command_save_board_settings(void);

/**************************************************************************
* \brief Sets the roll PID coefficients.
**************************************************************************/
static void command_set_roll_pid_coefficients(void);

/**************************************************************************
* \brief Sets the pitch PID coefficients.
**************************************************************************/
static void command_set_pitch_pid_coefficients(void);

/**************************************************************************
* \brief Sets the yaw PID coefficients.
**************************************************************************/
static void command_set_yaw_pid_coefficients(void);

/**************************************************************************
* \brief Sets the DCM roll constants.
**************************************************************************/
static void command_set_dcm_roll_coefficients(void);

/**************************************************************************
* \brief Sets the DCM pitch constants.
**************************************************************************/
static void command_set_dcm_pitch_coefficients(void);

/**************************************************************************
* \brief Sets the DCM yaw constants.
**************************************************************************/
static void command_set_dcm_yaw_coefficients(void);

/* Enabled API commands */
struct api_command commands[] = {
	{ 0x0001, command_set_servo_pos},
	{ 0x0002, command_get_servo_pos},
	{ 0x0003, command_get_all_servo_pos},
	{ 0x0010, command_get_board_status},
	{ 0x0020, command_get_board_settings},
	{ 0x0021, command_save_board_settings},
	{ 0x0022, command_set_roll_pid_coefficients},
	{ 0x0023, command_set_pitch_pid_coefficients},
	{ 0x0024, command_set_yaw_pid_coefficients},
	{ 0x0025, command_set_dcm_roll_coefficients},
	{ 0x0026, command_set_dcm_pitch_coefficients},
	{ 0x0027, command_set_dcm_yaw_coefficients}
};


//////////////////////////////////////////////////////////////////////////
// API commands implementation                                          
//////////////////////////////////////////////////////////////////////////

/**
* \brief This function sets a new position for a servo.
*
* Command Type: 0x00001
*/
static void command_set_servo_pos(void)
{
	uint8_t servo_nr = 0;
	uint16_t position = 0;
	
	// Get the parameters
	servo_nr = rx_command_packet.data[0];
	position = (position << 8) | rx_command_packet.data[2];
	position = (position << 8) | rx_command_packet.data[1];
	
	if (servo_nr < 0 || position > 180)
	{
		return;
	}
	
	// Set the new position
	servo_set_pos_degree(servo_nr, position);
}


/**
* \brief This function returns the actual position of a servo.
*
* Command Type: 0x0002
*/
static void command_get_servo_pos(void)
{
	//if (argc != 3)
		//return;
		
	//uint16_t servo_nr = atoi(argv[2]);
	//uint16_t position = servo_get_pos_degree(servo_nr);	
	//sprintf()
	//usart_putchar(USART_SERIAL_API, '\n');
	//usart_putchar(USART_SERIAL_API, '\r');
	//usart_putchar(USART_SERIAL_API, 'c');
	//usart_putchar(USART_SERIAL_API, '\r');
	//usart_putchar(USART_SERIAL_API, '\n');
	//char resp[] = "200";
	//write_command(resp);
}


/**************************************************************************
* \brief Returns the position of all servos
*
* Command Type: 0x0003
**************************************************************************/
static void command_get_all_servo_pos(void)
{
	// TODO
}

/**************************************************************************
* \brief Returns the board status.
*
* Command Type: 0x0010
**************************************************************************/
static void command_get_board_status(void)
{
	uint8_t packet_length = 1 + 1 + 2 + 1 + 98 + 1 + 1;
	uint8_t data[packet_length];
	int index = 0;
	
	// Get all status values
	uint16_t outChannel1 = servo_get_pos_degree(1);
	uint16_t outChannel2 = servo_get_pos_degree(2);
	uint16_t outChannel3 = servo_get_pos_degree(3);
	uint16_t outChannel4 = servo_get_pos_degree(4);
	uint16_t outChannel5 = servo_get_pos_degree(5);
	uint16_t outChannel6 = servo_get_pos_degree(6);
	uint16_t inChannel1 = servo_in_get_current_value(1);
	uint16_t inChannel2 = servo_in_get_current_value(2);
	uint16_t inChannel3 = servo_in_get_current_value(3);
	uint16_t inChannel4 = servo_in_get_current_value(4);
	uint16_t inChannel5 = servo_in_get_current_value(5);
	uint16_t inChannel6 = servo_in_get_current_value(6);
	uint16_t inChannel7 = servo_in_get_current_value(7);
	float accX = mpu_6000_get_x_acc();
	float accY = mpu_6000_get_y_acc();
	float accZ = mpu_6000_get_z_acc();
	float gyroX = mpu_6000_get_x_gyr();
	float gyroY = mpu_6000_get_y_gyr();
	float gyroZ = mpu_6000_get_z_gyr();
	float sensorRollAngle = filter_get_acc_roll();
	float sensorPitchAngle = filter_get_acc_pitch();
	float sensorYawAngle = filter_get_acc_yaw();	
	float dcmOutputRoll = flight_controller_get_sensor_roll_angle();
	float dcmOutputPitch = flight_controller_get_sensor_pitch_angle();
	float dcmOutputYaw = flight_controller_get_sensor_yaw_angle();
	float setRollAngle = flight_controller_get_set_roll_angle();
	float setPitchAngle = flight_controller_get_set_pitch_angle();
	float setYawAngle = flight_controller_get_set_yaw_angle();
	float pidRollActuatingValue = flight_controller_get_actuating_roll_angle();
	float pidPitchActuatingValue = flight_controller_get_actuating_pitch_angle();
	float pidYawActuatingValue = flight_controller_get_actuating_yaw_angle();
		
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x20;
	// Command type
	uint16_t cmdtype = 0x0010;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = 98;
	// Output Channel 1
	memcpy(data + index, &outChannel1, 2);
	index += 2;
	// Output channel 2
	memcpy(data + index, &outChannel2, 2);
	index += 2;
	// Output channel 3
	memcpy(data + index, &outChannel3, 2);
	index += 2;
	// Output channel 4
	memcpy(data + index, &outChannel4, 2);
	index += 2;
	// Output channel 5
	memcpy(data + index, &outChannel5, 2);
	index += 2;
	// Output channel 6
	memcpy(data + index, &outChannel6, 2);
	index += 2;
	// Input Channel 1
	memcpy(data + index, &inChannel1, 2);
	index += 2;
	// Input Channel 2
	memcpy(data + index, &inChannel2, 2);
	index += 2;
	// Input Channel 3
	memcpy(data + index, &inChannel3, 2);
	index += 2;
	// Input Channel 4
	memcpy(data + index, &inChannel4, 2);
	index += 2;
	// Input Channel 5
	memcpy(data + index, &inChannel5, 2);
	index += 2;
	// Input Channel 6
	memcpy(data + index, &inChannel6, 2);
	index += 2;
	// Input Channel 7
	memcpy(data + index, &inChannel7, 2);
	index += 2;
	// Acceleration X
	memcpy(data + index, &accX, 4);
	index += 4;
	// Acceleration Y
	memcpy(data + index, &accY, 4);
	index += 4;
	// Acceleration Z
	memcpy(data + index, &accZ, 4);
	index += 4;
	// Gyro X
	memcpy(data + index, &gyroX, 4);
	index += 4;
	// Gyro Y
	memcpy(data + index, &gyroY, 4);
	index += 4;
	// Gyro Z
	memcpy(data + index, &gyroZ, 4);
	index += 4;
	// Sensor Roll Angle
	memcpy(data + index, &sensorRollAngle, 4);
	index += 4;
	// Sensor Pitch Angle
	memcpy(data + index, &sensorPitchAngle, 4);
	index += 4;
	// Sensor Yaw Angle
	memcpy(data + index, &sensorYawAngle, 4);
	index += 4;
	// DCM Output Roll
	memcpy(data + index, &dcmOutputRoll, 4);
	index += 4;
	// DCM Output Pitch
	memcpy(data + index, &dcmOutputPitch, 4);
	index += 4;
	// DCM Output Yaw
	memcpy(data + index, &dcmOutputYaw, 4);
	index += 4;
	// Set Roll Angle
	memcpy(data + index, &setRollAngle, 4);
	index += 4;
	// Set Pitch Angle
	memcpy(data + index, &setPitchAngle, 4);
	index += 4;
	// Set Yaw Angle
	memcpy(data + index, &setYawAngle, 4);
	index += 4;
	// PID Roll Actuating Value
	memcpy(data + index, &pidRollActuatingValue, 4);
	index += 4;
	// PID Pitch Actuating Value
	memcpy(data + index, &pidPitchActuatingValue, 4);
	index += 4;
	// PID Yaw Actuating Value
	memcpy(data + index, &pidYawActuatingValue, 4);
	index += 4;
	// CRC
	data[index++] = 0x88;
	// Stop byte
	data[index] = PACKET_STOP_BYTE;
		
	write_packet(data, index+1);
}

/**************************************************************************
* \brief Returns the board settings.
**************************************************************************/
static void command_get_board_settings(void)
{
	uint8_t packet_length = 1 + 1 + 2 + 1 + 72 + 1 + 1;
	uint8_t data[packet_length];
	int index = 0;
	
	float pidRollKp = flight_controller_get_pid_roll_p_factor();
	float pidRollKi = flight_controller_get_pid_roll_i_factor();
	float pidRollKd = flight_controller_get_pid_roll_d_factor();
	float pidRollILimit = flight_controller_get_pid_roll_i_limit();
	float pidPitchKp = flight_controller_get_pid_pitch_p_factor();
	float pidPitchKi = flight_controller_get_pid_pitch_i_factor();
	float pidPitchKd = flight_controller_get_pid_pitch_d_factor();
	float pidPitchILimit = flight_controller_get_pid_pitch_i_limit();
	float pidYawKp = flight_controller_get_pid_yaw_p_factor();
	float pidYawKi = flight_controller_get_pid_yaw_i_factor();
	float pidYawKd = flight_controller_get_pid_yaw_d_factor();
	float pidYawILimit = flight_controller_get_pid_yaw_i_limit();
	float dcmRollPFactor = filter_dcm_get_rollPitch_kp();
	float dcmRollIFactor = filter_dcm_get_rollPitch_ki();
	float dcmPitchPFactor = filter_dcm_get_rollPitch_kp();
	float dcmPitchIFactor = filter_dcm_get_rollPitch_ki();
	float dcmYawPFactor = filter_dcm_get_yaw_kp();
	float dcmYawIFactor = filter_dcm_get_yaw_ki();
	
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x20;
	// Command type
	uint16_t cmdtype = 0x0020;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = 72;
	// PID Roll P-Factor
	memcpy(data + index, &pidRollKp, 4);
	index += 4;
	// PID Roll I-Factor
	memcpy(data + index, &pidRollKi, 4);
	index += 4;
	// PID Roll D-Factor
	memcpy(data + index, &pidRollKd, 4);
	index += 4;
	// PID Roll I-Limit
	memcpy(data + index, &pidRollILimit, 4);
	index += 4;
	// PID Pitch P-Factor
	memcpy(data + index, &pidPitchKp, 4);
	index += 4;
	// PID Pitch I-Factor
	memcpy(data + index, &pidPitchKi, 4);
	index += 4;
	// PID Pitch D-Factor
	memcpy(data + index, &pidPitchKd, 4);
	index += 4;
	// PID Pitch I-Limit
	memcpy(data + index, &pidPitchILimit, 4);
	index += 4;
	// PID Yaw P-Factor
	memcpy(data + index, &pidYawKp, 4);
	index += 4;
	// PID Yaw I-Factor
	memcpy(data + index, &pidYawKi, 4);
	index += 4;
	// PID Yaw D-Factor
	memcpy(data + index, &pidYawKd, 4);
	index += 4;
	// PID Yaw I-Limit
	memcpy(data + index, &pidYawILimit, 4);
	index += 4;
	// DCM Roll P-Factor
	memcpy(data + index, &dcmRollPFactor, 4);
	index += 4;
	// DCM Roll I-Factor
	memcpy(data + index, &dcmRollIFactor, 4);
	index += 4;
	// DCM Pitch P-Factor
	memcpy(data + index, &dcmPitchPFactor, 4);
	index += 4;
	// DCM Pitch I-Factor
	memcpy(data + index, &dcmPitchIFactor, 4);
	index += 4;
	// DCM Yaw P-Factor
	memcpy(data + index, &dcmYawPFactor, 4);
	index += 4;
	// DCM Yaw I-Factor
	memcpy(data + index, &dcmYawIFactor, 4);
	index += 4;
	// CRC
	data[index++] = 0x88;
	// Stop byte
	data[index] = PACKET_STOP_BYTE;
	
	write_packet(data, index+1);
}

/**************************************************************************
* \brief Saves the board settings to the external flash memory.
**************************************************************************/
static void command_save_board_settings(void)
{
	serial_flash_save_settings();
}

/**************************************************************************
* \brief Sets the roll PID coefficients.
**************************************************************************/
static void command_set_roll_pid_coefficients(void)
{
	float p_factor;
	float i_factor;
	float d_factor;
	float i_limit;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	memcpy(&d_factor, rx_command_packet.data + index, sizeof(d_factor));
	index += 4;
	memcpy(&i_limit, rx_command_packet.data + index, sizeof(i_limit));
	
	// Update the PID controller
	flight_controller_update_pid_roll_coefficients(p_factor, i_factor, d_factor, i_limit);
}

/**************************************************************************
* \brief Sets the pitch PID coefficients.
**************************************************************************/
static void command_set_pitch_pid_coefficients(void)
{
	float p_factor;
	float i_factor;
	float d_factor;
	float i_limit;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	memcpy(&d_factor, rx_command_packet.data + index, sizeof(d_factor));
	index += 4;
	memcpy(&i_limit, rx_command_packet.data + index, sizeof(i_limit));
	
	// Update the PID controller
	flight_controller_update_pid_pitch_coefficients(p_factor, i_factor, d_factor, i_limit);
}

/**************************************************************************
* \brief Sets the yaw PID coefficients.
**************************************************************************/
static void command_set_yaw_pid_coefficients(void)
{
	float p_factor;
	float i_factor;
	float d_factor;
	float i_limit;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	memcpy(&d_factor, rx_command_packet.data + index, sizeof(d_factor));
	index += 4;
	memcpy(&i_limit, rx_command_packet.data + index, sizeof(i_limit));
	
	// Update the PID controller
	flight_controller_update_pid_yaw_coefficients(p_factor, i_factor, d_factor, i_limit);
}

/**************************************************************************
* \brief Sets the DCM roll constants.
**************************************************************************/
static void command_set_dcm_roll_coefficients(void)
{
	float p_factor;
	float i_factor;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	
	filter_dcm_update_roll_constants(p_factor, i_factor);
}

/**************************************************************************
* \brief Sets the DCM pitch constants.
**************************************************************************/
static void command_set_dcm_pitch_coefficients(void)
{
	float p_factor;
	float i_factor;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	
	filter_dcm_update_pitch_constants(p_factor, i_factor);
}

/**************************************************************************
* \brief Sets the DCM yaw constants.
**************************************************************************/
static void command_set_dcm_yaw_coefficients(void)
{
	float p_factor;
	float i_factor;
	uint8_t index = 0;
	
	memcpy(&p_factor, rx_command_packet.data + index, sizeof(p_factor));
	index += 4;
	memcpy(&i_factor, rx_command_packet.data + index, sizeof(i_factor));
	index += 4;
	
	filter_dcm_update_yaw_constants(p_factor, i_factor);
}

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
								uint16_t val4, uint16_t val5)
{
	uint8_t packet_length = 1 + 1 + 2 + 1 + 10 + 1 + 1;
	uint8_t data[packet_length];
	int index = 0;
	
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x20;
	// Command type
	uint16_t cmdtype = 0x0100;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = 10;
	// val1
	memcpy(data + index, &val1, 2);
	index += 2;
	// val2
	memcpy(data + index, &val2, 2);
	index += 2;
	// val3
	memcpy(data + index, &val3, 2);
	index += 2;
	// val4
	memcpy(data + index, &val4, 2);
	index += 2;
	// val5
	memcpy(data + index, &val5, 2);
	index += 2;
	// CRC
	data[index++] = 0x88;
	// Stop byte
	data[index] = PACKET_STOP_BYTE;
	
	write_packet(data, index+1);
}

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
								float val5)
{
	uint8_t packet_length = 1 + 1 + 2 + 1 + 20 + 1 + 1;
	uint8_t data[packet_length];
	int index = 0;
	
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x20;
	// Command type
	uint16_t cmdtype = 0x0101;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = 20;
	// val1
	memcpy(data + index, &val1, 4);
	index += 4;
	// val2
	memcpy(data + index, &val2, 4);
	index += 4;
	// val3
	memcpy(data + index, &val3, 4);
	index += 4;
	// val4
	memcpy(data + index, &val4, 4);
	index += 4;
	// val5
	memcpy(data + index, &val5, 4);
	index += 4;
	// CRC
	data[index++] = 0x88;
	// Stop byte
	data[index] = PACKET_STOP_BYTE;
	
	write_packet(data, index+1);
}

/**************************************************************************
* \brief Writes a debug message to the serial line. The message is a string.
*
* \param message	String to write to the serial line.
* \param message_length		The length of the message.
**************************************************************************/
void write_debug_message_string(char* message, uint8_t message_length)
{
	uint16_t packet_length = 1 + 1 + 2 + 1 + message_length + 1 + 1;
	uint8_t data[packet_length];
	int index = 0;
	
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x20;
	// Command type
	uint16_t cmdtype = 0x0102;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = message_length;
	// val1
	memcpy(data + index, message, message_length);
	index += message_length;
	// CRC
	data[index++] = 0x88;
	// Stop byte
	data[index] = PACKET_STOP_BYTE;
	
	write_packet(data, index+1);
}

/**************************************************************************
* This method checks if the given character is a whitespace character.
**************************************************************************/
static char is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}


/**************************************************************************
* \brief Writes the given packet to the serial connection.
*
* \param data		Pointer to the data
* \param size		Length of the data in bytes
**************************************************************************/
void write_packet(uint8_t *data, uint16_t length)
{	
	int i = 0;
	while (i < length)
	{
		bool byteToBuffer;
		byteToBuffer = USART_TXBuffer_PutByte(&USART_data, data[i]);
		if (byteToBuffer)
		{
			i++;
		}
	}
}


/**************************************************************************
* This method executes the appropriate function for the received command.
**************************************************************************/
static void execute_command(void)
{
	uint8_t i;
	
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (commands[i].command_type == rx_command_packet.command &&
			commands[i].function != NULL) {
			// Execute the command function
			commands[i].function();
		}
	}
}


/**************************************************************************
* \brief This method parses a received packet.
**************************************************************************/
static void parse_command_packet(void)
{
	uint16_t index = 0;
	
	// The first byte is the start delimiter
	rx_command_packet.start_delimiter = command_buff.buff[index++];
	
	// The second byte contains the message type
	rx_command_packet.message_type = command_buff.buff[index++];
	
	// Byte four and five contains the command type	
	rx_command_packet.command = (uint16_t)(((command_buff.buff[index+1]) << 8)|command_buff.buff[index]);
	index += 2;
	
	// Byte six contains the data length
	rx_command_packet.data_length = command_buff.buff[index++];
	
	// Get the payload from the buffer
	memcpy(rx_command_packet.data, command_buff.buff + index, rx_command_packet.data_length * sizeof(uint8_t));
	index += rx_command_packet.data_length;
	
	// Get the CRC
	rx_command_packet.crc = command_buff.buff[index++];
	
	// Set the stop delimiter
	rx_command_packet.stop_delimiter = command_buff.buff[index];
	
	// Calculate and check the CRC checksum
	uint8_t crc_data[MAX_PACKET_LENGTH];
	uint16_t crc_data_length = command_buff.index - 2;
	memcpy(crc_data, command_buff.buff, crc_data_length * sizeof(uint8_t));
	
	crc_t crc = crc_init();
	crc_update(crc, crc_data, crc_data_length);
	crc = crc_finalize(crc);
	
	// Checksum error
	//if (crc != received_command_packet.crc)
	//{
		// TODO: Handle error
		//return;
	//}
	
	// If the CRC is correct, execute the command
	execute_command();
}

/**************************************************************************
* \brief Main worker method for the serial API.
**************************************************************************/
void serial_api_task(void)
{
	//uint8_t received_byte = usart_getchar(USART_SERIAL_API);
	uint8_t received_byte;
	
	while(USART_RXBufferData_Available(&USART_data)) {
		received_byte = USART_RXBuffer_GetByte(&USART_data);
		
		//if (command_buff.processing_active) {
			//return;
		//}
		
		// Process the received byte
		switch (received_byte) {
			// Start byte received
			case PACKET_START_BYTE:
			if (!command_buff.start_received) {
				command_buff.index = 0;
				command_buff.start_received = true;
				command_buff.buff[command_buff.index] = received_byte;
				command_buff.index++;
			}
			// New start byte received, but we already received a start byte. In this
			// case we just add the byte to the buffer
			else {
				if (command_buff.index < MAX_PACKET_LENGTH - 1) {
					command_buff.buff[command_buff.index] = received_byte;
					command_buff.index++;
				}
				else {
					// Reset everything
					command_buff.index = 0;
					command_buff.start_received = false;
					memset(command_buff.buff, 0, sizeof(command_buff.buff));
				}
			}
			break;
			
			// Stop byte received
			case PACKET_STOP_BYTE:
			if ((command_buff.start_received)==true) {
				if (command_buff.index > 0) {
					// Simple check if we received an stop byte at the start of a package
					// TODO: Extend this check
					//if (command_buff.index < 4) {
						//command_buff.buff[command_buff.index] = received_byte;
						//command_buff.index++;
						//break;
					//}
					
					command_buff.buff[command_buff.index] = received_byte;
					
					command_buff.processing_active = true;
					
					// Parse received command
					parse_command_packet();
			
					// Reset the command buffer
					command_buff.index = 0;
					command_buff.start_received = false;
					command_buff.processing_active = false;
					memset(command_buff.buff, 0, sizeof(command_buff.buff));
				}
			}
			break;
			
			// Only get the byte if the start byte was received
			default:
			if (command_buff.start_received) {
				if (command_buff.index < MAX_PACKET_LENGTH - 1) {
					command_buff.buff[command_buff.index] = received_byte;
					command_buff.index++;
				}
				else {
					// Reset everything
					command_buff.index = 0;
					command_buff.start_received = false;
					memset(command_buff.buff, 0, sizeof(command_buff.buff));
				}
			}
			break;
		}
	}		
}

/**************************************************************************
* \brief This function initializes the serial interface.
**************************************************************************/
void serial_api_init(void)
{	
	// Enable the system clock for the serial interface
	sysclk_enable_peripheral_clock(USART_SERIAL_API_INTERFACE);
	
	// Use USARTE0 and initialize buffers
	USART_InterruptDriver_Initialize(&USART_data, BOARD_USART_USB_INTERFACE,
									 USART_DREINTLVL_HI_gc);
	
	// USARTE0, 8 data bits, no parity, 1 stop bit
	USART_Format_Set(USART_data.usart, USART_CHSIZE_8BIT_gc,
					 USART_PMODE_DISABLED_gc, false);
	
	// Enable RXC interrupt
	USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_HI_gc);
	
	// Set baudrate to 9600 bps; scale factor is set to zero
	// BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
	// BSEL for 9600 bps => 207
	// BSEL for 57600 bps => 33 (33.722)
	USART_Baudrate_Set(USART_data.usart, 33, 0);
	
	// Initialize command buffer
	command_buff.index = 0;
	command_buff.start_received = false;
	command_buff.processing_active = false;
	
	// Enable both RX and TX
	USART_Rx_Enable(USART_data.usart);
	USART_Tx_Enable(USART_data.usart);
}


/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
//ISR(USARTE0_RXC_vect)
//{
	//USART_RXComplete(&USART_data);
//}

/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
//ISR(USARTE0_DRE_vect)
//{
	//USART_DataRegEmpty(&USART_data);
//}