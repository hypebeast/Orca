/**************************************************************************
 * \file event_queue.c
 *
 * \brief Implements a queue that can be used for handling events.
 *
 * Created: 16.02.2013 17:37:53
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "event_queue.h"


//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

static bool free_space(EventQueueHandle queue);


/************************************************************************
* \brief Sends a new event to the queue.
*
* \param queue		Event queue.
* \param info		Event info.
* \return 0			Success
* \return -1		Error
************************************************************************/
uint8_t event_queue_send(EventQueueHandle queue, VTOLObjEvent* info)
{
	uint8_t tmpHead;
	EventQueue_t* evQueue = (EventQueue_t*)queue;
	
	bool freeSpace = free_space(queue);
	if (!freeSpace)
	{
		return -1;
	}
	
	// Add the event object to the queue
	tmpHead = evQueue->head;
	evQueue->list[tmpHead] = info;
	// Advance buffer head
	evQueue->head = (tmpHead + 1) & EVENT_QUEUE_MASK;
	
	return 0;
}

/************************************************************************
* \brief Receives the next event from the event queue.
*
* \param queue		Event queue.
* \param info		Event info.
* \return 0			Success
* \return -1		Error
************************************************************************/
uint8_t event_queue_receive(EventQueueHandle queue, VTOLObjEvent* info)
{
	EventQueue_t* evQueue = (EventQueue_t*)queue;
	info = evQueue->list[evQueue->tail];
	
	// Advance buffer tail	
	evQueue->tail = (evQueue->tail + 1) & EVENT_QUEUE_MASK;
	
	return 0;
}

/************************************************************************
* \brief Return if the queue contains events.
*
* \param queue		Event queue.
* \return True		If the queue contains events.
* \return False		False if the queue contains no events.
************************************************************************/
bool event_queue_event_available(EventQueueHandle queue)
{
	EventQueue_t* evQueue = (EventQueue_t*)queue;
	uint8_t tmpHead = evQueue->head;
	uint8_t tmpTail = evQueue->tail;
	
	// There are data left unless head and tail are equal
	return (tmpHead != tmpTail);
}

/************************************************************************
* \brief Return if the queue has some free space.
*
* \param queue		Event queue.
* \return True		If the queue has free space.
* \return False		False if the queue contains no free space.
************************************************************************/
static bool free_space(EventQueueHandle queue)
{
	EventQueue_t* evQueue = (EventQueue_t*)queue;
	uint8_t tempHead = (evQueue->head + 1) & EVENT_QUEUE_MASK;
	uint8_t tempTail = evQueue->tail;
	
	// There are data left unless head and tail are equal
	return (tempHead != tempTail);
}
