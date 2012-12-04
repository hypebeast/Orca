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

/* \brief  Receive buffer size: 2,4,8,16,32,64,128 or 256 bytes. */
#define USART_RX_BUFFER_SIZE 256
/* \brief Transmit buffer size: 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 512
/* \brief Receive buffer mask. */
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
/* \brief Transmit buffer mask. */
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )


#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif
#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

/* \brief USART transmit and receive ring buffer. */
typedef struct USART_Buffer
{
	/* \brief Receive buffer. */
	volatile uint8_t RX[USART_RX_BUFFER_SIZE];
	/* \brief Transmit buffer. */
	volatile uint8_t TX[USART_TX_BUFFER_SIZE];
	/* \brief Receive buffer head. */
	volatile uint8_t RX_Head;
	/* \brief Receive buffer tail. */
	volatile uint8_t RX_Tail;
	/* \brief Transmit buffer head. */
	volatile uint8_t TX_Head;
	/* \brief Transmit buffer tail. */
	volatile uint8_t TX_Tail;
} USART_Buffer_t;

/*! \brief Struct used when interrupt driven driver is used.
*
*  Struct containing pointer to a usart, a buffer and a location to store Data
*  register interrupt level temporary.
*/
typedef struct Usart_and_buffer
{
	/* \brief Pointer to USART module to use. */
	USART_t * usart;
	/* \brief Data register empty interrupt level. */
	USART_DREINTLVL_t dreIntLevel;
	/* \brief Data buffer. */
	USART_Buffer_t buffer;
} USART_data_t;

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

/* Macros. */

/*! \brief Macro that sets the USART frame format.
 *
 *  Sets the frame format, Frame Size, parity mode and number of stop bits.
 *
 *  \param _usart        Pointer to the USART module
 *  \param _charSize     The character size. Use USART_CHSIZE_t type.
 *  \param _parityMode   The parity Mode. Use USART_PMODE_t type.
 *  \param _twoStopBits  Enable two stop bit mode. Use bool type.
 */
#define USART_Format_Set(_usart, _charSize, _parityMode, _twoStopBits)         \
	(_usart)->CTRLC = (uint8_t) _charSize | _parityMode |                      \
	                  (_twoStopBits ? USART_SBMODE_bm : 0)

/*! \brief Set USART baud rate.
 *
 *  Sets the USART's baud rate register.
 *
 *  UBRR_Value   : Value written to UBRR
 *  ScaleFactor  : Time Base Generator Scale Factor
 *
 *  Equation for calculation of BSEL value in asynchronous normal speed mode:
 *  	If ScaleFactor >= 0
 *  		BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
 *  	If ScaleFactor < 0
 *  		BSEL = (1/(2^(ScaleFactor)*16))*(((I/O clock frequency)/Baudrate)-1)
 *
 *	\note See XMEGA manual for equations for calculation of BSEL value in other
 *        modes.
 *
 *  \param _usart          Pointer to the USART module.
 *  \param _bselValue      Value to write to BSEL part of Baud control register.
 *                         Use uint16_t type.
 *  \param _bScaleFactor   USART baud rate scale factor.
 *                         Use uint8_t type
 */
#define USART_Baudrate_Set(_usart, _bselValue, _bScaleFactor)                  \
	(_usart)->BAUDCTRLA =(uint8_t)_bselValue;                                           \
	(_usart)->BAUDCTRLB =(_bScaleFactor << USART_BSCALE0_bp)|(_bselValue >> 8)
					  
/*! \brief Set USART RXD interrupt level.
 *
 *  Sets the interrupt level on RX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _rxdIntLevel  Interrupt level of the RXD interrupt.
 *                       Use USART_RXCINTLVL_t type.
 */
#define USART_RxdInterruptLevel_Set(_usart, _rxdIntLevel)                      \
	((_usart)->CTRLA = ((_usart)->CTRLA & ~USART_RXCINTLVL_gm) | _rxdIntLevel)


/*! \brief Set USART TXD interrupt level.
 *
 *  Sets the interrupt level on TX Complete interrupt.
 *
 *  \param _usart        Pointer to the USART module.
 *  \param _txdIntLevel  Interrupt level of the TXD interrupt.
 *                       Use USART_TXCINTLVL_t type.
 */
#define USART_TxdInterruptLevel_Set(_usart, _txdIntLevel)                      \
	(_usart)->CTRLA = ((_usart)->CTRLA & ~USART_TXCINTLVL_gm) | _txdIntLevel

/*! \brief Enable USART receiver.
 *
 *  \param _usart    Pointer to the USART module
 */
#define USART_Rx_Enable(_usart) ((_usart)->CTRLB |= USART_RXEN_bm)


/*! \brief Disable USART receiver.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Rx_Disable(_usart) ((_usart)->CTRLB &= ~USART_RXEN_bm)


/*! \brief Enable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Enable(_usart)	((_usart)->CTRLB |= USART_TXEN_bm)


/*! \brief Disable USART transmitter.
 *
 *  \param _usart Pointer to the USART module.
 */
#define USART_Tx_Disable(_usart) ((_usart)->CTRLB &= ~USART_TXEN_bm)


/* Function declarations */

void USART_InterruptDriver_Initialize(USART_data_t * usart_data,
									  USART_t * usart,
									  USART_DREINTLVL_t dreIntLevel );
									  
void USART_InterruptDriver_DreInterruptLevel_Set(USART_data_t * usart_data,
												 USART_DREINTLVL_t dreIntLevel);

bool USART_TXBuffer_FreeSpace(USART_data_t * usart_data);
bool USART_TXBuffer_PutByte(USART_data_t * usart_data, uint8_t data);
bool USART_RXBufferData_Available(USART_data_t * usart_data);
uint8_t USART_RXBuffer_GetByte(USART_data_t * usart_data);												 
bool USART_RXComplete(USART_data_t * usart_data);
void USART_DataRegEmpty(USART_data_t * usart_data);

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