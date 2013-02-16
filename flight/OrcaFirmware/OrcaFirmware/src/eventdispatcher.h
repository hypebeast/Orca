/**************************************************************************
 * \file eventdispatcher.h
 *
 * \brief Header file for eventdispatcher.c.
 *
 * Created: 03.02.2013 12:10:45
 * Author: Sebastian Ruml <sebastian.ruml@gmail.com>
 **************************************************************************/


#ifndef EVENTDISPATCHER_H_
#define EVENTDISPATCHER_H_

#include <stdio.h>
#include "vtol_object_manager.h"
#include "event_queue.h"

//////////////////////////////////////////////////////////////////////////
// Function declarations
//////////////////////////////////////////////////////////////////////////

int8_t event_dispatcher_init();
int8_t event_dispatcher_task();
int8_t event_dispatcher_create_periodic_callback(VTOLObjEvent* ev, VTOLObjEventCallback cb, uint16_t periodMs);
int8_t event_dispatcher_update_periodic_callback(VTOLObjEvent* ev, VTOLObjEventCallback cb, uint16_t periodMs);
int8_t event_dispatcher_create_periodic_queue(VTOLObjEvent* ev, EventQueueHandle queue, uint16_t periodMs);
int8_t event_dispatcher_update_periodic_queue(VTOLObjEvent* ev, EventQueueHandle queue, uint16_t periodMs);
int8_t event_dispatcher_callback_dispatch(VTOLObjEvent* ev, VTOLObjEventCallback cb);


#endif /* EVENTDISPATCHER_H_ */