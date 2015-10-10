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

#include "../misc/misc.h"

/// @brief 	Defines the maximum amount of events that can wait in the event queue.
#define MAX_QUEUED_EVENTS 	(64)

/**
 * @brief	All possible types of events that can be queued and processed.
 */
typedef enum event_types {
	EVENT_NONE = 0,			///< @brief No event queued (empty)
	EVENT_BT_GOT_PACKET,	///< @brief Received packet via bluetooth.
	EVENT_RF_GOT_PACKET,	///< @brief Received packet via radio.
	EVENT_SWITCH_SINGLE,	///< @brief Input switch was pressed one time.
	EVENT_SWITCH_DOUBLE,	///< @brief Input switch was pressed two times.
	EVENT_SWITCH_POWER,		///< @brief Input switch was pressed for a long time.
	EVENT_LOW_BATTERY,		///< @brief Battery voltage low.
	EVENT_CRITICAL_BATTERY,		///< @brief Battery empty.
	EVENT_EXT_PWR_PLUGGED,	///< @brief External power supplied, can charge.
	EVENT_RADIO_RESTART		///< @brief Radio chip needs restart due to error.
} event_types;

/**
 * @brief	Push a new event into the queue.
 */
void events_enqueue(event_types);

/**
 * @brief	Take an event from the queue.
 */
event_types events_dequeue(void);



#endif /* EVENT_QUEUE_H_ */
