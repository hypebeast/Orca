/**************************************************************************
 * \file telemetry.c
 *
 * \brief Handles all the telemetry of the VTOL objects.
 *
 * Created: 02.02.2013 12:51:54
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "telemetry.h"
#include "usart.h"
#include "vtol_link.h"

// Private variables

static USART_data_t telemetry_usart_data;
static VTOLLinkConnectionData_t vtolLinkConnection;

// Private functions

static uint8_t transmit_data(uint8_t * data, int32_t length);

// TODO


/************************************************************************
* \brief Initializes the telemetry module.
* \return 0		Success.
* \return -1	Error.
************************************************************************/
uint8_t telemetry_init(void)
{
	// Enable the system clock for the serial interface
	sysclk_enable_peripheral_clock(TELEMETRY_USART_INTERFACE);
	
	// Use USARTE0 and initialize buffers
	USART_InterruptDriver_Initialize(&telemetry_usart_data, TELEMETRY_USART_INTERFACE,
										USART_DREINTLVL_HI_gc);
	
	// USARTE0, 8 data bits, no parity, 1 stop bit
	USART_Format_Set(telemetry_usart_data.usart, USART_CHSIZE_8BIT_gc,
						USART_PMODE_DISABLED_gc, false);
	
	// Enable RXC interrupt
	USART_RxdInterruptLevel_Set(telemetry_usart_data.usart, USART_RXCINTLVL_HI_gc);
	
	// TODO: Read telemetry settings from VTOL object
	
	// Set baudrate to 9600 bps; scale factor is set to zero
	// BSEL = ((I/O clock frequency)/(2^(ScaleFactor)*16*Baudrate))-1
	// BSEL for 9600 bps => 207
	// BSEL for 57600 bps => 33 (33.722)
	USART_Baudrate_Set(telemetry_usart_data.usart, 33, 0);
	
	// Initialize connection data
	vtolLinkConnection.rx_packet_length = 0;
	vtolLinkConnection.rx_state = VTOLTALK_STATE_INIT;
	vtolLinkConnection.outputStream = &transmit_data;
	memset(&vtolLinkConnection.rx_buffer, 0, VTOL_MAX_PACKET_LENGTH);
	memset(&vtolLinkConnection.tx_buffer, 0, VTOL_MAX_PACKET_LENGTH);
	memset(&vtolLinkConnection.stats, 0, sizeof(VTOLLinkStats_t));
	
	// Initialize VTOL link
	uint8_t res = vtol_link_init();
	if (res < 0)
		return -1;
	
	// Enable both RX and TX
	USART_Rx_Enable(telemetry_usart_data.usart);
	USART_Tx_Enable(telemetry_usart_data.usart);
	
	return 0;
}

/************************************************************************
* \brief Telemetry RX task.
************************************************************************/
void telemetry_rx_task(void)
{
	// TODO
}

/************************************************************************
* \brief Transmit data buffer to the modem or UCB.
************************************************************************/
static uint8_t transmit_data(uint8_t * data, int32_t length)
{
	// TODO
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