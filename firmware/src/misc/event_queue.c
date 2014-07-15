/**
 * @file
 *
 * @date	14.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */


#include "event_queue.h"

event_types events_queued[MAX_QUEUED_EVENTS];
uint8_t		events_queue_write_i = 0;
uint8_t		events_queue_read_i = 0;

void events_enqueue(event_types type) {
	events_queued[events_queue_write_i] = type;
	events_queue_write_i = (events_queue_write_i + 1) % MAX_QUEUED_EVENTS;
}

event_types events_dequeue() {
	if (events_queue_write_i == events_queue_read_i) return EVENT_NONE;

	event_types ret = events_queued[events_queue_read_i];
	events_queue_read_i = (events_queue_read_i + 1) % MAX_QUEUED_EVENTS;
	return ret;
}
