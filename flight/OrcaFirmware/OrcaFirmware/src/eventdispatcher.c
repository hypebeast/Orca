/**************************************************************************
 * \file eventdispatcher.c
 *
 * \brief The event dispatcher handles periodic updates of VTOL objects.
 *		  Moreover, it's responsible for manually updated objects.
 *
 * Created: 03.02.2013 12:10:45
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/

#include "eventdispatcher.h"
#include "system_time.h"


//////////////////////////////////////////////////////////////////////////
// Private constants
//////////////////////////////////////////////////////////////////////////

#define EVENT_DISPATCHER_PERIODIC_OBJECT_LIST_SIZE	100
#define MAX_UPDATE_PERIOD 1000 // ms


//////////////////////////////////////////////////////////////////////////
// Private types
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* \brief This struct defines properties needed for handling periodic
 *        updates.
/************************************************************************/
typedef struct {
	EventCallbackInfo_t evInfo; /** Event info. */
	uint16_t updatePeriodeMs; /** Update period in ms or 0 if no periodic updates are needed */
	int32_t timeToNextUpdateMs; /** Time to the next update */
} PeriodicObject_t;

/************************************************************************/
/* \brief List with all objects that should be periodic updated.
/************************************************************************/
typedef struct {
	PeriodicObject_t list[EVENT_DISPATCHER_PERIODIC_OBJECT_LIST_SIZE];
	uint16_t index;
} PeriodicObjectList_t;


//////////////////////////////////////////////////////////////////////////
// Private variables
//////////////////////////////////////////////////////////////////////////

/** List with all objects for periodic updates (telemetry module) */
static PeriodicObjectList_t periodicObjectList;
/** Time to next update */
static int32_t timeToNextUpdateMs;

// Event queue for manually dispatched events
static EventQueue_t queue;
static EventQueueHandle queue_handle;


//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////

static int8_t process_periodic_updates();
static int8_t event_periodic_create(VTOLObjEvent* ev, VTOLObjEventCallback cb, EventQueueHandle queue, uint16_t periodMs);
static int8_t event_periodic_update(VTOLObjEvent* ev, VTOLObjEventCallback cb, EventQueueHandle queue, uint16_t periodMs);
static uint16_t randomizePeriod(uint16_t periodMs);


/************************************************************************
* \brief Initializes the event dispatcher.
*
* \return 0		Success
* \return -1	Error
************************************************************************/
int8_t event_dispatcher_init()
{
	// Initialize the object list
	memset(&periodicObjectList, 0, sizeof(periodicObjectList));
	periodicObjectList.index = 0;
	
	// Initialize queue
	queue.head = 0;
	queue.tail = 0;
	queue_handle = &queue;
	
	// Initialize variables
	timeToNextUpdateMs = system_time_get_ticks() * SYSTEM_TIME_TICK_RATE_MS;
	
	// Done
	return 0;
}

/************************************************************************
* \brief Event dispatcher main task.
*
* This function dispatches all events. There are two different main events:
* periodic and non-periodic. Periodic events are handled either with a callback
* function or an external event queue. The non-periodic events are handled with
* a local event queue added by the \ref event_dispatcher_callback_dispatch
* function.
*
* \return 0		Success
* \return -1	Error
************************************************************************/
int8_t event_dispatcher_task()
{
	// Get the current time
	int32_t currentTime = system_time_get_ticks() * SYSTEM_TIME_TICK_RATE_MS;
	
	// TODO: Dispatch events for non-periodic events (-> local queue)
	if (event_queue_event_available(queue_handle))
	{
		VTOLObjEvent* vtolEv;
		event_queue_receive(queue_handle, vtolEv);
		
		//VTOLObjectData obj = (VTOLObjectData*)vtolEv->obj;
		// TODO: Find the callback and execute it
		//if ()
		//{
		//}
	}
	
	// Process periodic events
	if (currentTime >= timeToNextUpdateMs)
	{
		timeToNextUpdateMs = process_periodic_updates();
	}
	
	return 0;
}

/************************************************************************/
/* \brief Dispatch an event by invoking the supplied callback. The function
*		  returns immediately, the callback is invoked by the event task.
*
* \param ev		The event to be dispatched.
* \param cb		The callback function.
* \return		0, if success; otherwise, -1.
/************************************************************************/
int8_t event_dispatcher_callback_dispatch(VTOLObjEvent* ev, VTOLObjEventCallback cb)
{
	EventCallbackInfo_t evInfo;
	memcpy(&evInfo.ev, ev, sizeof(VTOLObjEvent));
	evInfo.cb = cb;
	evInfo.queue = 0;
	
	// Add new periodic event
	//PeriodicObject_t periodicObj = periodicObjectList.list[periodicObjectList.index++];
	//periodicObj.evInfo = evInfo;
	//periodicObj.timeToNextUpdateMs = 0;
	//periodicObj.updatePeriodeMs = 0;
	
	// Add object event to event queue (queue for non-periodic updates)
	event_queue_send(queue_handle, &evInfo.ev);
	
	return 0;
}

/************************************************************************
* \brief Adds the given VTOL object to the periodic update queue.
*
* \param ev			TODO
* \param cb			TODO
* \param periodMs	TODO
* \note				This function is not yet implemented!
************************************************************************/
int8_t event_dispatcher_create_periodic_callback(VTOLObjEvent* ev, VTOLObjEventCallback cb, uint16_t periodMs)
{
	return event_periodic_create(ev, cb, 0, periodMs);
}

/************************************************************************
* \brief TODO
*
* \note			This function is not yet implemented!
************************************************************************/
int8_t event_dispatcher_update_periodic_callback(VTOLObjEvent* ev, VTOLObjEventCallback cb, uint16_t periodMs)
{
	return event_periodic_update(ev, cb, 0, periodMs);
}

/************************************************************************/
/* \brief Dispatch an event at periodic intervals.
*
* \param ev			The event to be dispatched.
* \param queue		Event queue.
* \param periodMs	The update interval
* \return 0			Success
* \return -1		Error
/************************************************************************/
int8_t event_dispatcher_create_periodic_queue(VTOLObjEvent* ev, EventQueueHandle queue, uint16_t periodMs)
{
	return event_periodic_create(ev, 0, queue, periodMs);
}

/************************************************************************/
/* \brief Update the period of a periodic event.
*
* \param ev			The event to be dispatched.
* \param queue		Event queue.
* \param periodMs	The period the event is generated.
* \return 0			Success
* \return -1		Error
/************************************************************************/
int8_t event_dispatcher_update_periodic_queue(VTOLObjEvent* ev, EventQueueHandle queue, uint16_t periodMs)
{
	return event_periodic_update(ev, 0, queue, periodMs);
}

/************************************************************************/
/* \brief Handle periodic updates for all objects.
*
* \return The system time until the next update (in ms) or -1 if failed.
/************************************************************************/
static int8_t process_periodic_updates()
{
	int32_t timeToNextUpdateMs;
	int32_t timeNow;
	
	// Iterate through each object and updates its timer, if zero then
	// add object to event queue and/or call the callback function. Also
	// calculate smallest delay to next update.
	timeToNextUpdateMs = (system_time_get_ticks() * SYSTEM_TIME_TICK_RATE_MS) + MAX_UPDATE_PERIOD;
	for (int i = 0; i < periodicObjectList.index; i++)
	{
		PeriodicObject_t objEntry = periodicObjectList.list[i];
		// Check if time for the next update
		timeNow = system_time_get_ticks() * SYSTEM_TIME_TICK_RATE_MS;
		if (objEntry.timeToNextUpdateMs <= timeNow)
		{
			// Reset timer
			int32_t offset = (timeNow - objEntry.timeToNextUpdateMs) % objEntry.updatePeriodeMs;
			objEntry.timeToNextUpdateMs = timeNow + objEntry.updatePeriodeMs - offset;
				
			// Invoke callback, if any
			if (objEntry.evInfo.cb != NULL)
			{
				objEntry.evInfo.cb(&objEntry.evInfo.ev);
			}
				
			// Push event to queue, if any
			if (objEntry.evInfo.queue != NULL)
			{
				// Push event to the queue
				event_queue_send(objEntry.evInfo.queue, &objEntry.evInfo.ev);
			}
			
			if (objEntry.timeToNextUpdateMs < timeToNextUpdateMs)
			{
				timeToNextUpdateMs = objEntry.timeToNextUpdateMs;
			}
		}
	}
	
	return timeToNextUpdateMs;
}

/************************************************************************/
/* \brief Dispatch an event through a callback at periodic intervals.
*
* \param ev			The event to be dispatched.
* \param cb			Callback that should be periodically called.
* \param queue		Event queue to use.
* \param periodMs	Update period.
* \return 0			Success
* \return -1		Error or VTOL object is already connected
/************************************************************************/
static int8_t event_periodic_create(VTOLObjEvent* ev, VTOLObjEventCallback cb, EventQueueHandle queue, uint16_t periodMs)
{
	// Check if the object is not already connected
	for (int i = 0; i < EVENT_DISPATCHER_PERIODIC_OBJECT_LIST_SIZE; i++)
	{
		PeriodicObject_t objEntry = periodicObjectList.list[i];
		if (objEntry.evInfo.ev.obj == ev->obj &&
			objEntry.evInfo.ev.instId == ev->instId &&
			objEntry.evInfo.ev.event == ev->event &&
			objEntry.evInfo.cb == cb &&
			objEntry.evInfo.queue == queue)
		{
			return -1;
		}
	}
	
	// Check for overrun
	if (periodicObjectList.index >= EVENT_DISPATCHER_PERIODIC_OBJECT_LIST_SIZE)
	{
		return -1;
	}
	
	// Add the object event to the object list
	PeriodicObject_t newObj = periodicObjectList.list[periodicObjectList.index++];
	newObj.evInfo.ev.obj = ev->obj;
	newObj.evInfo.ev.instId = ev->instId;
	newObj.evInfo.ev.event = ev->event;
	newObj.evInfo.cb = cb;
	newObj.evInfo.queue = queue;
	newObj.updatePeriodeMs = periodMs;
	newObj.timeToNextUpdateMs = randomizePeriod(periodMs);
	
	// Done
	return 0;
}

/************************************************************************/
/* \brief Update the period of a periodic event.
*
* \param ev			The event to be dispatched.
* \param cb			Callback that should be periodically called.
* \param queue		Event queue to use.
* \param periodMs	Update period.
* \return 0			Success
* \return -1		Error
/************************************************************************/
static int8_t event_periodic_update(VTOLObjEvent* ev, VTOLObjEventCallback cb, EventQueueHandle queue, uint16_t periodMs)
{
	// Find object
	for (int i = 0; i < EVENT_DISPATCHER_PERIODIC_OBJECT_LIST_SIZE; i++)
	{
		PeriodicObject_t listObj = periodicObjectList.list[i];
		if (listObj.evInfo.ev.obj == ev->obj &&
			listObj.evInfo.ev.event == ev->event &&
			listObj.evInfo.ev.instId == ev->instId &&
			listObj.evInfo.cb == cb &&
			listObj.evInfo.queue == queue)
		{
			// Object found, update period
			listObj.updatePeriodeMs = periodMs;
			listObj.timeToNextUpdateMs = randomizePeriod(periodMs);
			return 0;
		}
	}
	
	// Object was not found
	return -1;
}

/**
 * Return a pseudorandom integer from 0 to periodMs
 * Based on the Park-Miller-Carta Pseudo-Random Number Generator
 * http://www.firstpr.com.au/dsp/rand31/
 */
static uint16_t randomizePeriod(uint16_t periodMs)
{
	static uint32_t seed = 1;
	uint32_t hi, lo;
	lo = 16807 * (seed & 0xFFFF);
	hi = 16807 * (seed >> 16);
	lo += (hi & 0x7FFF) << 16;
	lo += hi >> 15;
	if (lo > 0x7FFFFFFF) lo -= 0x7FFFFFFF;
	seed = lo;
	return (uint16_t)( ((float)periodMs * (float)lo) / (float)0x7FFFFFFF );
}