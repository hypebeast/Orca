/**************************************************************************
 * \file telemetry.c
 *
 * \brief Handles all the telemetry of the VTOL objects.
 *
 * Created: 02.02.2013 12:51:54
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "orca.h"

//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

/** USART data */
static USART_data_t telemetry_usart_data;
/** VTOL link connection data */
static VTOLLinkConnectionData_t vtolLinkConnection;
/** Event queue used for periodic updates */
static EventQueue_t periodic_event_queue;
/** Handle to the queue for periodic updates */
static EventQueueHandle periodic_queue_handle;


//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static uint8_t transmit_data(uint8_t * data, int32_t length);
static void telemetry_rx_task();
static void telemetry_tx_task();
static void register_object(VTOLObjHandle obj);
static void process_obj_event(VTOLObjEvent* ev);
static void hw_settings_initialize();
static void registerObject(VTOLObjHandle obj);
static int8_t updateObject(VTOLObjHandle obj, int32_t eventType);
static int8_t addObject(VTOLObjHandle obj);
//static void setUpdatePeriod(VTOLObjHandle obj, uint32_t updatePeriodMs);


/************************************************************************
* \brief Initializes the telemetry module.
* \return 0		Success.
* \return -1	Error.
************************************************************************/
uint8_t telemetry_init()
{
	// TODO: Read telemetry settings from VTOL object and initialize the hardware
	//hw_settings_initialize();
	
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
		
	// Event queue
	periodic_event_queue.head = 0;
	periodic_event_queue.tail = 0;
	// Save the handle to the event queue
	periodic_queue_handle = &periodic_event_queue;
		
	// Register VTOL objects for periodic updates
	vtol_obj_iterate(&register_object);
	
	// Enable both RX and TX
	USART_Rx_Enable(telemetry_usart_data.usart);
	USART_Tx_Enable(telemetry_usart_data.usart);
	
	return 0;
}

/************************************************************************/
/* \brief Initialize the hardware settings for the UART.                                                                     */
/************************************************************************/
static void hw_settings_initialize()
{
	// TODO
}

/************************************************************************
* \brief Main telemetry task.
************************************************************************/
void telemetry_task()
{
	telemetry_rx_task();
	telemetry_tx_task();
}

/************************************************************************
* \brief Telemetry RX task.
************************************************************************/
static void telemetry_rx_task()
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
static void telemetry_tx_task()
{
	// Check if one or more events are waiting
	while (event_queue_event_available(periodic_queue_handle))
	{
		VTOLObjEvent* vtolEv;
		event_queue_receive(periodic_queue_handle, vtolEv);
		process_obj_event(vtolEv);
	}
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

/************************************************************************/
/* \brief	Registers the given VTOL object for periodic updates (if update
			mode is periodic.
*
* \param obj	The object handle.
/************************************************************************/
static void register_object(VTOLObjHandle obj)
{
	if (obj == NULL)
		return;
		
	// Only register object if update mode is periodic
	// Note: This can be changed in the future!
	VTOLObjMetaData metadata;
	VTOLObjUpdateMode updateMode;
	vtol_get_metadata(obj, &metadata);
	updateMode = vtol_get_telemetry_update_mode(&metadata);
	
	if (updateMode == UPDATE_MODE_PERIODIC)
	{
		// Register object for periodic updates
		addObject(obj);
		
		// Setup object for telemetry updates (set timers, etc)
		updateObject(obj, EV_NONE);
	}
}

/************************************************************************/
/* \brief Register object for periodic updates.
*	
* \param obj	VTOL object handle.
* \return 0		Success
* \return -1	Error
/************************************************************************/
static int8_t addObject(VTOLObjHandle obj)
{
	VTOLObjEvent ev;
	
	// Add object for periodic updates
	ev.obj = obj;
	ev.instId = 0;
	ev.event = EV_UPDATED_PERIODIC;
	return event_dispatcher_create_periodic_queue(&ev, periodic_queue_handle, 0);
}

/************************************************************************/
/* \brief Update object's timers, depending on object's settings.
*
* \param obj		VTOL object handle.
* \param eventType	Event type.
* \return 0			Success
* \return -1		Error
/************************************************************************/
static int8_t updateObject(VTOLObjHandle obj, int32_t eventType)
{
	VTOLObjMetaData metadata;
	VTOLObjUpdateMode updateMode;
	
	// Get metadata
	vtol_get_metadata(obj, &metadata);
	updateMode = vtol_get_telemetry_update_mode(&metadata);
	
	// Currently only periodic update mode is supported
	// Set the timer only if the update mode is periodic
	if (updateMode == UPDATE_MODE_PERIODIC)
	{
		VTOLObjEvent ev;
		ev.obj = obj;
		ev.instId = 0;
		ev.event = EV_UPDATED_PERIODIC;
		
		// Set update period
		event_dispatcher_update_periodic_queue(&ev, periodic_queue_handle, metadata.telemetryUpdatePeriod);
	}
	
	return 0;
}

/************************************************************************/
/* \brief Process object events.
/************************************************************************/
static void process_obj_event(VTOLObjEvent* ev)
{
	// Check if connection is established
	
	// Check which event type we got
	if (ev->event == EV_UPDATED_PERIODIC)
	{
		struct VTOLObjectMeta meta;
		vtol_get_metadata(ev->obj, &meta);
		VTOLObjMetaData *metaData = &meta.instance;
		uint8_t ack = vtol_get_ack_required(metaData);
		vtol_link_send_object((VTOLLinkConnection)&vtolLinkConnection, ev->obj, 0, ack, 1000);
	}
}

/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
ISR(USARTE0_RXC_vect)
{
	USART_RXComplete(&telemetry_usart_data);
}

/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTE0_DRE_vect)
{
	USART_DataRegEmpty(&telemetry_usart_data);
}