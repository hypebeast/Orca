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


//////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))


//////////////////////////////////////////////////////////////////////////
// Variables
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


/* Enabled API commands */
struct api_command commands[] = {
	{ 0x0001, command_set_servo_pos},
	{ 0x0002, command_get_servo_pos},
	{ 0x0003, command_get_all_servo_pos},
	{ 0x0010, command_get_board_status}
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
	//servo_set_pos_degree(servo_nr, 90);
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
	
}

/**************************************************************************
* \brief Returns the board status.
*
* Command Type: 0x0010
**************************************************************************/
static void command_get_board_status(void)
{
	uint8_t data[43];
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
	uint16_t accX = 0;
	uint16_t accY = 0;
	uint16_t accZ = 0;
	uint16_t gyroX = 0;
	uint16_t gyroY = 0;
	uint16_t gyroZ = 0;
		
	// Start byte
	data[index++] = PACKET_START_BYTE;
	// Message type
	data[index++] = 0x10;
	// Command type
	uint16_t cmdtype = 0x0010;
	memcpy(data + index, &cmdtype, 2);
	index += 2;
	// Data Length
	data[index++] = 38;
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
	memcpy(data + index, &accX, 2);
	index += 2;
	// Acceleration Y
	memcpy(data + index, &accY, 2);
	index += 2;
	// Acceleration Z
	memcpy(data + index, &accZ, 2);
	index += 2;
	// Gyro X
	memcpy(data + index, &gyroX, 2);
	index += 2;
	// Gyro Y
	memcpy(data + index, &gyroY, 2);
	index += 2;
	// Gyro Z
	memcpy(data + index, &gyroZ, 2);
	index += 2;
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
	unsigned int i;
	
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

/*! \brief Initializes buffer and selects what USART module to use.
 *
 *  Initializes receive and transmit buffer and selects what USART module to use,
 *  and stores the data register empty interrupt level.
 *
 *  \param usart_data           The USART_data_t struct instance.
 *  \param usart                The USART module.
 *  \param dreIntLevel          Data register empty interrupt level.
 */
void USART_InterruptDriver_Initialize(USART_data_t * usart_data,
                                      USART_t * usart,
                                      USART_DREINTLVL_t dreIntLevel)
{
	usart_data->usart = usart;
	usart_data->dreIntLevel = dreIntLevel;

	usart_data->buffer.RX_Tail = 0;
	usart_data->buffer.RX_Head = 0;
	usart_data->buffer.TX_Tail = 0;
	usart_data->buffer.TX_Head = 0;
}

/*! \brief RX Complete Interrupt Service Routine.
 *
 *  RX Complete Interrupt Service Routine.
 *  Stores received data in RX software buffer.
 *
 *  \param usart_data      The USART_data_t struct instance.
 */
bool USART_RXComplete(USART_data_t * usart_data)
{
	USART_Buffer_t * bufPtr;
	bool ans;

	bufPtr = &usart_data->buffer;
	/* Advance buffer head. */
	uint8_t tempRX_Head = (bufPtr->RX_Head + 1) & USART_RX_BUFFER_MASK;

	/* Check for overflow. */
	uint8_t tempRX_Tail = bufPtr->RX_Tail;
	uint8_t data = usart_data->usart->DATA;

	if(tempRX_Head == tempRX_Tail){
	  	ans = false;
	}else{
		ans = true;
		usart_data->buffer.RX[usart_data->buffer.RX_Head] = data;
		usart_data->buffer.RX_Head = tempRX_Head;
	}
	return ans;
}

/*! \brief Data Register Empty Interrupt Service Routine.
 *
 *  Data Register Empty Interrupt Service Routine.
 *  Transmits one byte from TX software buffer. Disables DRE interrupt if buffer
 *  is empty. Argument is pointer to USART (USART_data_t).
 *
 *  \param usart_data      The USART_data_t struct instance.
 */
void USART_DataRegEmpty(USART_data_t * usart_data)
{
	USART_Buffer_t * bufPtr;
	bufPtr = &usart_data->buffer;

	/* Check if all data is transmitted. */
	uint8_t tempTX_Tail = usart_data->buffer.TX_Tail;
	if (bufPtr->TX_Head == tempTX_Tail){
	    /* Disable DRE interrupts. */
		uint8_t tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
		usart_data->usart->CTRLA = tempCTRLA;

	}else{
		/* Start transmitting. */
		uint8_t data = bufPtr->TX[usart_data->buffer.TX_Tail];
		usart_data->usart->DATA = data;

		/* Advance buffer tail. */
		bufPtr->TX_Tail = (bufPtr->TX_Tail + 1) & USART_TX_BUFFER_MASK;
	}
}

/*! \brief Test if there is data in the receive software buffer.
 *
 *  This function can be used to test if there is data in the receive software
 *  buffer.
 *
 *  \param usart_data         The USART_data_t struct instance
 *
 *  \retval true      There is data in the receive buffer.
 *  \retval false     The receive buffer is empty.
 */
bool USART_RXBufferData_Available(USART_data_t * usart_data)
{
	/* Make copies to make sure that volatile access is specified. */
	uint8_t tempHead = usart_data->buffer.RX_Head;
	uint8_t tempTail = usart_data->buffer.RX_Tail;

	/* There are data left in the buffer unless Head and Tail are equal. */
	return (tempHead != tempTail);
}



/*! \brief Get received data (5-8 bit character).
 *
 *  The function USART_RXBufferData_Available should be used before this
 *  function is used to ensure that data is available.
 *
 *  Returns data from RX software buffer.
 *
 *  \param usart_data       The USART_data_t struct instance.
 *
 *  \return         Received data.
 */
uint8_t USART_RXBuffer_GetByte(USART_data_t * usart_data)
{
	USART_Buffer_t * bufPtr;
	uint8_t ans;

	bufPtr = &usart_data->buffer;
	ans = (bufPtr->RX[bufPtr->RX_Tail]);

	/* Advance buffer tail. */
	bufPtr->RX_Tail = (bufPtr->RX_Tail + 1) & USART_RX_BUFFER_MASK;

	return ans;
}


/*! \brief Test if there is data in the transmitter software buffer.
 *
 *  This function can be used to test if there is free space in the transmitter
 *  software buffer.
 *
 *  \param usart_data The USART_data_t struct instance.
 *
 *  \retval true      There is data in the receive buffer.
 *  \retval false     The receive buffer is empty.
 */
bool USART_TXBuffer_FreeSpace(USART_data_t * usart_data)
{
	/* Make copies to make sure that volatile access is specified. */
	uint8_t tempHead = (usart_data->buffer.TX_Head + 1) & USART_TX_BUFFER_MASK;
	uint8_t tempTail = usart_data->buffer.TX_Tail;

	/* There are data left in the buffer unless Head and Tail are equal. */
	return (tempHead != tempTail);
}


/*! \brief Put data (5-8 bit character).
 *
 *  Stores data byte in TX software buffer and enables DRE interrupt if there
 *  is free space in the TX software buffer.
 *
 *  \param usart_data The USART_data_t struct instance.
 *  \param data       The data to send.
 */
bool USART_TXBuffer_PutByte(USART_data_t * usart_data, uint8_t data)
{
	uint8_t tempCTRLA;
	uint8_t tempTX_Head;
	bool TXBuffer_FreeSpace;
	USART_Buffer_t * TXbufPtr;

	TXbufPtr = &usart_data->buffer;
	TXBuffer_FreeSpace = USART_TXBuffer_FreeSpace(usart_data);


	if(TXBuffer_FreeSpace)
	{
	  	tempTX_Head = TXbufPtr->TX_Head;
	  	TXbufPtr->TX[tempTX_Head]= data;
		/* Advance buffer head. */
		TXbufPtr->TX_Head = (tempTX_Head + 1) & USART_TX_BUFFER_MASK;

		/* Enable DRE interrupt. */
		tempCTRLA = usart_data->usart->CTRLA;
		tempCTRLA = (tempCTRLA & ~USART_DREINTLVL_gm) | usart_data->dreIntLevel;
		usart_data->usart->CTRLA = tempCTRLA;
	}
	return TXBuffer_FreeSpace;
}


/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
ISR(USARTE0_RXC_vect)
{
	USART_RXComplete(&USART_data);
}

/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTE0_DRE_vect)
{
	USART_DataRegEmpty(&USART_data);
}