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

//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

static USART_data_t telemetry_usart_data;
static VTOLLinkConnectionData_t vtolLinkConnection;

//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static uint8_t transmit_data(uint8_t * data, int32_t length);
static void telemetry_rx_task(void);
static void telemetry_tx_task(void);
static void register_object(VTOLObjHandle obj);
static void process_obj_event(void);
//static void registerObject(VTOLObjHandle obj);
//static void updateObject(VTOLObjHandle obj);
//static void addObject(VTOLObjHandle obj);
//static void setUpdatePeriod(VTOLObjHandle obj, uint32_t updatePeriodMs);


/************************************************************************
* \brief Initializes the telemetry module.
* \return 0		Success.
* \return -1	Error.
************************************************************************/
uint8_t telemetry_init(void)
{
	// Enable the system clock for the serial interface
	sysclk_enable_peripheral_clock(TELEMETRY_USART_INTERFACE);
	
	// TODO: Load telemetry settings from VTOL settings object.
	
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
	vtolLinkConnection.rx_state = VTOLLINK_STATE_SYNC;
	vtolLinkConnection.outputStream = &transmit_data;
	memset(&vtolLinkConnection.rx_buffer, 0, VTOL_LINK_MAX_PAYLOAD_LENGTH);
	memset(&vtolLinkConnection.tx_buffer, 0, VTOL_LINK_MAX_PAYLOAD_LENGTH);
	memset(&vtolLinkConnection.stats, 0, sizeof(VTOLLinkStats_t));
	
	// Initialize VTOL link
	uint8_t res = vtol_link_init();
	if (res < 0)
		return -1;
		
	// TODO: Register objects for telemetry updates
	
	// Enable both RX and TX
	USART_Rx_Enable(telemetry_usart_data.usart);
	USART_Tx_Enable(telemetry_usart_data.usart);
	
	return 0;
}

/************************************************************************
* \brief Main telemetry task.
************************************************************************/
void telemetry_task(void)
{
	telemetry_rx_task();
	telemetry_tx_task();
}

/************************************************************************
* \brief Telemetry RX task.
************************************************************************/
static void telemetry_rx_task(void)
{
	uint8_t received_byte;
	
	while (USART_RXBufferData_Available(&telemetry_usart_data))
	{
		received_byte = USART_RXBuffer_GetByte(&telemetry_usart_data);
		
		VTOLLinkConnection connection = (VTOLLinkConnection)&vtolLinkConnection;
		vtol_link_process_input_stream(connection, received_byte);
	}
}

/************************************************************************
* \brief Telemetry RX task.
************************************************************************/
static void telemetry_tx_task(void)
{
	// TODO: Wait for events and sends objects
	// If event -> process_obj_event();
}

/************************************************************************
* \brief Transmit data buffer to the modem or USB.
************************************************************************/
static uint8_t transmit_data(uint8_t * data, int32_t length)
{
	int i = 0;
	while (i < length)
	{
		bool byteToBuffer = USART_TXBuffer_PutByte(&telemetry_usart_data, data[i]);
		if (byteToBuffer)
			i++;
	}
	
	return i;
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