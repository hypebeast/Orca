/**************************************************************************
 * \file vtol_link.h
 *
 * \brief Header file for vtol_link.c
 *
 * Created: 26.01.2013 23:21:44
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#ifndef VTOL_LINK_H_
#define VTOL_LINK_H_

#include <string.h>
#include <asf.h>
#include <stdlib.h>

#include "orca.h"
#include "user_board.h"


//////////////////////////////////////////////////////////////////////////
// Public defines
//////////////////////////////////////////////////////////////////////////

#define VTOL_LINK_MAX_PAYLOAD_LENGTH 2^8-1
#define VTOL_LINK_MAX_MESSAGE_LENGTH (1+1+2+2+1+VTOL_LINK_MAX_PAYLOAD_LENGTH+1)
#define VTOL_LINK_HEADER_LENGTH 7
#define VTOL_LINK_START_BYTE 0x8D
#define VTOL_LINK_CHECKSUM_LENGTH 1


//////////////////////////////////////////////////////////////////////////
// Public types
//////////////////////////////////////////////////////////////////////////

typedef void* VTOLLinkConnection;
typedef uint8_t (*VTOLLinkOutputStream)(uint8_t* data, int32_t length);

/************************************************************************
* \brief VTOL RX state.                                                  
************************************************************************/
typedef enum {
	VTOLLINK_STATE_SYNC = 0,
	VTOLLINK_STATE_TYPE,
	VTOLLINK_STATE_LENGTH,
	VTOLLINK_STATE_ID,
	VTOLLINK_STATE_INSTID,
	VTOLLINK_STATE_DATA,
	VTOLLINK_STATE_CRC,
	VTOLLINK_STATE_COMPLETE,
	VTOLLINK_STATE_ERROR 
} VTOLLinkRxState;

typedef enum {
	VTOL_LINK_MESSAGE_TYPE_OBJECT = 0x20,
	VTOL_LINK_MESSAGE_TYPE_OBJECT_REQUEST = 0x21,
	VTOL_LINK_MESSAGE_TYPE_OBJECT_ACK = 0x22,
	VTOL_LINK_MESSAGE_TYPE_ACK = 0x23
} VTOLLINKMessageType;


//////////////////////////////////////////////////////////////////////////
// Public structures
//////////////////////////////////////////////////////////////////////////

/************************************************************************
* \brief Holds stats about the VTOL link status.
************************************************************************/
typedef struct {
	uint32_t txBytes;
	uint32_t rxBytes;
	uint32_t txObjectBytes;
	uint32_t rxObjectBytes;
	uint32_t txObjects;
	uint32_t rxObjects;
	uint32_t txErrors;
	uint32_t rxErrors;
} VTOLLinkStats_t;

/************************************************************************
* \brief Data for a VTOL link connection.
************************************************************************/
typedef struct {
	uint8_t rx_packet_length;
	uint8_t tx_packet_length;
	uint8_t rx_buffer[VTOL_LINK_MAX_MESSAGE_LENGTH];
	uint8_t tx_buffer[VTOL_LINK_MAX_MESSAGE_LENGTH];
	VTOLLinkRxState rx_state;
	uint8_t type;
	uint16_t rx_count;
	uint16_t rx_packet_size;
	uint16_t objId;
	VTOLObjHandle obj;
	uint8_t obj_length;
	VTOLLinkOutputStream outputStream;
	VTOLLinkStats_t stats;
} VTOLLinkConnectionData_t;


//////////////////////////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////////////////////////

uint8_t vtol_link_init(void);
void vtol_link_get_stats(VTOLLinkConnection conn, VTOLLinkStats_t* statsOut);
uint8_t vtol_link_send_object(VTOLLinkConnection conn, VTOLObjHandle obj, uint16_t instId, uint8_t ack_req, uint16_t timeoutMs);
VTOLLinkRxState vtol_link_process_input_stream(VTOLLinkConnection conn, uint8_t rxByte);

#endif /* VTOL_LINK_H_ */