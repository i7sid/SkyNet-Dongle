/**
 * @file
 *
 * @date	14.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to event queue / cyclic buffer.
 */


#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

#include <ring_buffer.h>
#include "../misc/misc.h"


/// @brief 	Defines the maximum amount of events that can wait in the event queue.
#define MAX_QUEUED_EVENTS 	(32)

/**
 * @brief	All possible types of events that can be queued and processed.
 */
typedef enum event_types {
	EVENT_NONE = 0,			///< @brief No event queued (empty)
	EVENT_ERROR,			///< @brief An error occured in software.
	EVENT_RF_GOT_PACKET,	///< @brief Received packet via radio.
	EVENT_SWITCH_SINGLE,	///< @brief Input switch was pressed one time.
	EVENT_SWITCH_DOUBLE,	///< @brief Input switch was pressed two times.
	EVENT_SWITCH_POWER,		///< @brief Input switch was pressed for a long time.
	EVENT_LOW_BATTERY,		///< @brief Battery voltage low.
	EVENT_CRITICAL_BATTERY,	///< @brief Battery empty.
	EVENT_EXT_PWR_PLUGGED,	///< @brief External power supplied, can charge.
	EVENT_RADIO_RESTART,	///< @brief Radio chip needs restart due to error.
	EVENT_USB_RX_MESSAGE,	///< @brief USB CDC data available.
	EVENT_GPS_DATA_AVAILABLE, ///< @brief GPS data available.
	EVENT_DEBUG_1,			///< @brief Reserved for debugging purposes.
	EVENT_DEBUG_2,			///< @brief Reserved for debugging purposes.
	EVENT_DEBUG_3,			///< @brief Reserved for debugging purposes.
	EVENT_DEBUG_4			///< @brief Reserved for debugging purposes.
} event_types;

typedef struct queued_event {
	void*	data;	///< @brief May point to a data structure, must be known.
	char	type;	///< @brief Should be one of \ref event_types .
} queued_event;


/**
 * @brief	Initializes the ringbuffer.
 */
void events_init(void);


/**
 * @brief			Push a new event into the queue.
 * @param	type	\ref one of event_types
 * @param	data	pointer to optional event data (may be NULL)
 */
int events_enqueue(event_types type, void* data);

/**
 * @brief		Take an event from the queue.
 * @param	e	Pointer the event is written to. (can be NULL if not needed)
 */
event_types events_dequeue(queued_event *e);



#endif /* EVENT_QUEUE_H_ */
