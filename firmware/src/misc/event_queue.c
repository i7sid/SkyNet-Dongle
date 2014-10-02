/**
 * @file
 *
 * @date	14.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for event queue / cyclic buffer.
 */


#include "event_queue.h"

/// @brief Array of enqueued events
event_types events_queued[MAX_QUEUED_EVENTS];

/// @brief Write pointer
uint8_t		events_queue_write_i = 0;

/// @brief Read pointer
uint8_t		events_queue_read_i = 0;



// If there is more load (more packets per second etc.) some synchronisation will be needed!

void events_enqueue(event_types type) {
	events_queued[events_queue_write_i] = type;
	events_queue_write_i = (events_queue_write_i + 1) % MAX_QUEUED_EVENTS;

	if (events_queue_write_i == events_queue_read_i) {
		DBG("[ERROR] EVENT QUEUE OVERFLOW!\n");
	}
}

event_types events_dequeue(void) {
	if (events_queue_write_i == events_queue_read_i) return EVENT_NONE;

	event_types ret = events_queued[events_queue_read_i];
	events_queue_read_i = (events_queue_read_i + 1) % MAX_QUEUED_EVENTS;
	return ret;
}
