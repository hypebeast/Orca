/**************************************************************************
 * \file event_queue.h
 *
 * \brief Header file for event_queue.c
 *
 * Created: 16.02.2013 17:38:09
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include <string.h>
#include "vtol_object_manager.h"


//////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////

#define EVENT_QUEUE_SIZE 64
#define EVENT_QUEUE_MASK ( EVENT_QUEUE_SIZE - 1 )

#if ( EVENT_QUEUE_SIZE & EVENT_QUEUE_MASK )
#error Event queue size is not a power of 2
#endif


//////////////////////////////////////////////////////////////////////////
// Public types
//////////////////////////////////////////////////////////////////////////

/** Event queue handle */
typedef void* EventQueueHandle;


//////////////////////////////////////////////////////////////////////////
// Structures
//////////////////////////////////////////////////////////////////////////

/************************************************************************
* \brief Structure for keeping info about events.
************************************************************************/
typedef struct {
	VTOLObjEvent ev; /** The event. */
	VTOLObjEventCallback cb; /** The callback function or zero if none. */
	EventQueueHandle queue; /** Handle to a event queue */
} EventCallbackInfo_t;


/************************************************************************
* \brief Event queue structure.
************************************************************************/
typedef struct {
	VTOLObjEvent* list[EVENT_QUEUE_SIZE];
	uint8_t head; /** Pointer to the head of the queue */
	uint8_t tail; /** Pointer to the tail of the queue */
} EventQueue_t;


//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////

uint8_t event_queue_send(EventQueueHandle queue, VTOLObjEvent* info);
uint8_t event_queue_receive(EventQueueHandle queue, VTOLObjEvent* info);
bool event_queue_event_available(EventQueueHandle queue);


#endif /* EVENT_QUEUE_H_ */