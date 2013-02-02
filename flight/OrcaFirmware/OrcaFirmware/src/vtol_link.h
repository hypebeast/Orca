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

// Defines

#define VTOL_MAX_PACKET_LENGTH 2^8-1
#define VTOL_LINK_START_BYTE 0x8D

// Public types
typedef void* VTOLLinkConnection;
typedef int32_t (*VTOLLinkOutputStream)(uint8_t* data, int32_t length);

/************************************************************************
* \brief VTOL RX state.                                                  
************************************************************************/
typedef enum {
	VTOLTALK_STATE_INIT = 0,
	VTOLTALK_STATE_IDLE,
	VTOLTALK_STATE_ACTIVE,
	VTOLTALK_STATE_ERROR 
} VTOLLinkRxState;

// Public structures

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
	uint8_t rx_buffer[VTOL_MAX_PACKET_LENGTH];
	uint8_t tx_buffer[VTOL_MAX_PACKET_LENGTH];
	VTOLLinkRxState rx_state;
	VTOLLinkOutputStream outputStream;
	VTOLLinkStats_t stats;
} VTOLLinkConnectionData_t;


// Public functions

uint8_t vtol_link_init(void);
void vtol_link_get_stats(VTOLLinkConnection conn, VTOLLinkStats_t* statsOut);
uint8_t vtol_link_send_object(VTOLLinkConnection conn, VTOLObjHandle obj);
uint8_t vtol_link_process_input_stream(VTOLLinkConnection conn, uint8_t rxByte);

#endif /* VTOL_LINK_H_ */