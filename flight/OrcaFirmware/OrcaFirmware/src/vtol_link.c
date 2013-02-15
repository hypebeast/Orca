/**************************************************************************
 * \file vtol_link.c
 *
 * \brief VTOL link implementation.
 *
 * Created: 26.01.2013 23:21:29
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "vtol_link.h"


//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static uint8_t objectTransaction(VTOLLinkConnectionData_t* connection, VTOLObjHandle obj, uint16_t instId,
								uint8_t type, uint16_t timeoutMs);
static uint8_t sendObject(VTOLLinkConnectionData_t* connection, VTOLObjHandle obj, uint16_t instId, uint8_t type);
static uint8_t sendSingleObject(VTOLLinkConnectionData_t* connection, VTOLObjHandle obj, uint8_t type);
static VTOLLinkRxState vtol_link_process_input_byte(VTOLLinkConnection conn, uint8_t rxByte);
static uint8_t receiveObject(VTOLLinkConnectionData_t *connection, uint8_t type, uint16_t objId,
						uint8_t instId, uint8_t* data, uint8_t length);


/************************************************************************
* \brief Initialize the VTOL link module.
************************************************************************/
uint8_t vtol_link_init()
{
	// Nothing todo here, yet.
	return 0;
}

/************************************************************************
* \brief Returns the connection stats for the given connection handle.
************************************************************************/
void vtol_link_get_stats(VTOLLinkConnection conn, VTOLLinkStats_t* statsOut)
{
	// Cast to connection data
	VTOLLinkConnectionData_t *connection = (VTOLLinkConnectionData_t*)conn;
	// Copy data
	memcpy(statsOut, &connection->stats, sizeof(VTOLLinkStats_t));
}

/************************************************************************
* \brief Returns the connection stats for the given connection handle.
*
* \param conn		Connection handle.
* \param obj		Object handle.
* \param instId		Instance ID. Not yet used.
* \param ack_req	If true (1) an acknowledge request is send with the object.
* \param timeoutMs	Time to wait for the ack, when zero it will return immediately.
* return 0 if success, else -1 if error.
************************************************************************/
uint8_t vtol_link_send_object(VTOLLinkConnection conn, VTOLObjHandle obj,
							uint16_t instId, uint8_t ack_req, uint16_t timeoutMs)
{
	VTOLLinkConnectionData_t *connection = (VTOLLinkConnectionData_t*)conn;
	if (ack_req)
	{
		return objectTransaction(connection, obj, instId, VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK, timeoutMs);
	}
	else
	{
		return objectTransaction(connection, obj, instId, VTOL_LINK_MESSAGE_TYPE_OBJECT, timeoutMs);
	}
}

/************************************************************************
* \brief Processes a byte from the telemetry stream.
*
* \param conn	Connection data.
* \param rxByte	Received byte that should be processed.
* \return VTOLLinkRxState
************************************************************************/
static VTOLLinkRxState vtol_link_process_input_byte(VTOLLinkConnection conn, uint8_t rxByte)
{
	VTOLLinkConnectionData_t *connection = (VTOLLinkConnectionData_t *)conn;
	
	connection->stats.rxBytes++;
	
	if (connection->rx_state == VTOLLINK_STATE_ERROR
			|| connection->rx_state == VTOLLINK_STATE_COMPLETE)
	{
		connection->rx_state = VTOLLINK_STATE_SYNC;
	}
	
	switch (connection->rx_state)
	{
		case VTOLLINK_STATE_SYNC:
			if (rxByte == VTOL_LINK_START_BYTE)
			{
				connection->rx_packet_length = 1;
				connection->rx_state = VTOLLINK_STATE_TYPE;
			}
			break;
		
		case VTOLLINK_STATE_TYPE:
			if (rxByte != VTOL_LINK_MESSAGE_TYPE_OBJECT ||
				rxByte != VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST ||
				rxByte != VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK ||
				rxByte != VTOL_LINK_MESSAGE_TYPE_ACK)
			{
				connection->rx_state = VTOLLINK_STATE_ERROR;
				break;
			}
			connection->type = rxByte;
			connection->rx_packet_size = 0;
			connection->rx_count = 0;
			connection->rx_state = VTOLLINK_STATE_LENGTH;
			break;
		
		case VTOLLINK_STATE_LENGTH:
			if (connection->rx_count == 0)
			{
				connection->rx_count++;
				connection->rx_packet_size = rxByte;
				break;
			}
			
			connection->rx_count = 0;
			connection->rx_packet_size += rxByte << 8;
			connection->objId = 0;
			connection->rx_state = VTOLLINK_STATE_ID;
			break;
			
		case VTOLLINK_STATE_ID:
			if (connection->rx_count == 0)
			{
				connection->rx_count++;
				connection->objId = rxByte;
			}
			
			connection->objId += rxByte << 8;
			connection->rx_count = 0;
			
			// Search for object
			connection->obj = vtol_obj_get_by_id(connection->objId);
			
			// Get VTOLO length
			if (connection->obj)
			{
				connection->obj_length = vtol_obj_get_num_bytes(connection->obj);
			}
			else
			{
				connection->rx_state = VTOLLINK_STATE_ERROR;
				break;
			}
			
			// Check length
			if (connection->obj_length > VTOL_LINK_MAX_PAYLOAD_LENGTH)
			{
				connection->stats.rxErrors++;
				connection->rx_state = VTOLLINK_STATE_ERROR;
				break;
			}
			
			// Check if lengths match
			if ((connection->obj_length + VTOL_LINK_HEADER_LENGTH) != connection->rx_packet_size)
			{
				connection->stats.rxErrors++;
				connection->rx_state = VTOLLINK_STATE_ERROR;
				break;
			}
			
			connection->rx_state = VTOLLINK_STATE_INSTID;
			break;
			
		case VTOLLINK_STATE_INSTID:
			// The instance ID is currently not used!
			connection->rx_state = VTOLLINK_STATE_DATA;
			break;
		
		case VTOLLINK_STATE_DATA:
			connection->rx_buffer[connection->rx_count++] = rxByte;
			connection->stats.rxObjectBytes++;
			if (connection->rx_count < connection->obj_length)
				break;
				
			connection->rx_state = VTOLLINK_STATE_CRC;
			connection->rx_count = 0;
			break;
		
		case VTOLLINK_STATE_CRC:
			// TODO: Check CRC
			connection->stats.rxObjects++;
			connection->rx_state = VTOLLINK_STATE_COMPLETE;
			connection->rx_count = 0;
			break;
		
		default:
			connection->stats.rxErrors++;
			connection->rx_state = VTOLLINK_STATE_ERROR;
			break;
	}
	
	// Done
	return connection->rx_state;
}

/************************************************************************
* \brief Processes an byte from the telemetry stream.
*
* \param conn	Connection data.
* \param rxByte	Received byte that should be processed.
* \return VTOLLinkRxState
************************************************************************/
VTOLLinkRxState vtol_link_process_input_stream(VTOLLinkConnection conn, uint8_t rxByte)
{
	VTOLLinkRxState state = vtol_link_process_input_byte(conn, rxByte);
	
	// If we got an complete package, pass it to the VTOLO manager for unpacking
	if (state == VTOLLINK_STATE_COMPLETE)
	{
		VTOLLinkConnectionData_t* connection = (VTOLLinkConnectionData_t*)conn;
		receiveObject(connection, connection->type, connection->objId, 0,
			&connection->rx_buffer, connection->obj_length);
	}
	
	return state;
}

/************************************************************************
* \brief Receive an object. This function processes VTOL objects received
* through the telemetry stream.
*
* \param connection Connection data.
* \param type		Message type.
* \param objId		VTOL object id.
* \param instId		Instance ID. Not yet used.
* \param data		Data buffer.
* \param length		Data buffer length.
* \return 0			Success
* \return -1		Error
************************************************************************/
static uint8_t receiveObject(VTOLLinkConnectionData_t *connection, uint8_t type,
				uint16_t objId, uint8_t instId, uint8_t* data, uint8_t length)
{
	uint8_t ret = 0;
	VTOLObjHandle obj = vtol_obj_get_by_id(objId);
	
	switch (type)
	{
		case VTOL_LINK_MESSAGE_TYPE_OBJECT:
			vtol_unpack(obj, 0, data);
			break;
			
		case VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK:
			vtol_unpack(obj, 0, data);
			sendObject(connection, obj, 0, VTOL_LINK_MESSAGE_TYPE_ACK);
			break;
		
		case VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST:
			// TODO: Send requested VTOL object
			break;
			
		default:
			break;
	}
	
	return ret;
}

/************************************************************************
* \brief Execute the requested transaction on an object.
*
* \param connection	Connection to use.
* \param obj		Object handle.
* \param instId		Instance ID. Not yet used!
* \param type		Transaction type.
* \param timeoutMs	Not yet used!
* \return 0 if success, else -1 if error.
************************************************************************/
static uint8_t objectTransaction(VTOLLinkConnectionData_t *connection, VTOLObjHandle obj,
				uint16_t instId, uint8_t type, uint16_t timeoutMs)
{
	if (type == VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK ||
		type == VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST)
	{
		return sendObject(connection, obj, instId, type);
	}
	else if (type == VTOL_LINK_MESSAGE_TYPE_OBJECT)
	{
		return sendObject(connection, obj, instId, VTOL_LINK_MESSAGE_TYPE_OBJECT);
	}
	else
	{
		return -1;
	}
}

/************************************************************************
* \brief Send an object through the telemetry link.
*
* \param connection Connection to use.
* \param obj		Object handle.
* \param instId		Instance ID. Not yet used!
* \param type		Transaction type.
* \return 0 if success, else -1 if error.
************************************************************************/
static uint8_t sendObject(VTOLLinkConnectionData_t *connection, VTOLObjHandle obj,
				uint16_t instId, uint8_t type)
{
	// Process message type
	if (type == VTOL_LINK_MESSAGE_TYPE_OBJECT ||
		type == VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK)
	{
		return sendSingleObject(connection, obj, type);
	}
	else if (type == VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST)
	{
		return sendSingleObject(connection, obj, VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST);
	}
	else if (type == VTOL_LINK_MESSAGE_TYPE_ACK)
	{
		return sendSingleObject(connection, obj, VTOL_LINK_MESSAGE_TYPE_ACK);
	}
	else
	{
		return -1;
	}
}

/************************************************************************
* \brief Send an object through the telemetry link.
*
* \param connection Connection to use.
* \param obj		Object handle.
* \param type		Message type.
* \return 0			Success
* \return -1		Error
************************************************************************/
static uint8_t sendSingleObject(VTOLLinkConnectionData_t *connection,
				VTOLObjHandle obj, uint8_t type)
{
	// Check if a callback is specified
	if (!connection->outputStream)
		return -1;
	
	uint16_t objId = vtol_obj_get_id(obj);
	connection->tx_buffer[0] = VTOL_LINK_START_BYTE;
	connection->tx_buffer[1] = type;
	// Data length will be inserted later
	connection->tx_buffer[4] = (uint8_t)(objId & 0xFF);
	connection->tx_buffer[5] = (uint8_t)((objId >> 8) & 0xFF);
	
	uint8_t data_offset = 6;
	
	// Get the object data length
	uint16_t length;
	if (type == VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST ||
		type == VTOL_LINK_MESSAGE_TYPE_ACK)
	{
		length = 0;
	}
	else
	{
		length = vtol_obj_get_num_bytes(obj);	
	}
	
	if (length >= VTOL_LINK_MAX_PAYLOAD_LENGTH)
	{
		return -1;
	}
	
	// Copy data (if any)
	if (length > 0)
	{
		if (vtol_pack(obj, connection->tx_buffer[data_offset]) < 0)
		{
			return -1;
		}
	}
	
	// Store the packet length
	connection->tx_buffer[2] = (uint8_t)(length & 0xFF);
	connection->tx_buffer[3] = (uint8_t)((length >> 8) & 0xFF);
	
	// Checksum
	// TODO: Calculate checksum
	connection->tx_buffer[data_offset+length] = 0xEE;
	
	uint16_t tx_msg_len = data_offset+length+VTOL_LINK_CHECKSUM_LENGTH;
	uint16_t rc = (*connection->outputStream)(connection->tx_buffer, tx_msg_len);
	
	if (rc == tx_msg_len)
	{
		// Update stats
		++connection->stats.txObjects;
		connection->stats.txBytes += tx_msg_len;
		connection->stats.txObjectBytes += length;
	}
	
	// Done
	return 0;
}
