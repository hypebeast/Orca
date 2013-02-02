/**************************************************************************
 * \file vtol_link.c
 *
 * \brief VTOL link implementation.
 *
 * Created: 26.01.2013 23:21:29
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "vtol_link.h"

// Private variables

// Private methods
static uint8_t sendObject(VTOLLinkConnectionData_t connection, VTOLObjHandle obj, uint8_t type);
static uint8_t sendSingleObject(VTOLLinkConnectionData_t connection, VTOLObjHandle obj, uint8_t type);


/************************************************************************
* \brief Initialize the VTOL link module.
************************************************************************/
uint8_t vtol_link_init(void)
{
	// TODO
	return 0;
}

/************************************************************************
* \brief Returns the connection stats for the given connection handle.
************************************************************************/
void vtol_link_get_stats(VTOLLinkConnection conn, VTOLLinkStats_t* statsOut)
{
	VTOLLinkConnectionData_t *connection = (VTOLLinkConnectionData_t*)conn;
	// Copy data
	memcpy(statsOut, &connection->stats, sizeof(VTOLLinkStats_t));
}

uint8_t vtol_link_send_object(VTOLLinkConnection conn, VTOLObjHandle obj)
{
	// TODO
}

uint8_t vtol_link_process_input_stream(VTOLLinkConnection conn, uint8_t rxByte)
{
	// TODO
}

static uint8_t sendObject(VTOLLinkConnectionData_t connection, VTOLObjHandle obj, uint8_t type)
{
	// TODO
}

static uint8_t sendSingleObject(VTOLLinkConnectionData_t connection, VTOLObjHandle obj, uint8_t type)
{
	// TODO
}
