/**
 * @file
 *
 * @date	14.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for event queue / cyclic buffer.
 */


#include "event_queue.h"

RINGBUFF_T events_ringbuf;
char events_buf[MAX_QUEUED_EVENTS * sizeof(queued_event)];

void events_init(void) {
	//queued_event
	RingBuffer_Init(&events_ringbuf, events_buf, sizeof(queued_event), MAX_QUEUED_EVENTS);
	RingBuffer_Flush(&events_ringbuf);
}


int events_enqueue(event_types type, void* data) {
	//DBG("buf_enc\n");
	queued_event e;
	e.type = type;
	e.data = data;

	int r = RingBuffer_Insert(&events_ringbuf, &e);
#ifdef DEBUG
	if (!r) {
		// TODO do something? (but then remove #ifdef DEBUG !)
		DBG("ERROR: events ringbuffer is full.\n");
	}
#endif
	return r;
}

event_types events_dequeue(queued_event *e) {
	//DBG("buf_deq\n");
	queued_event *ev = e;
	queued_event instead;

	// if e is not given, use temporaray one instead
	if (e == NULL) {
		ev = &instead;
	}

	uint8_t r = RingBuffer_Pop(&events_ringbuf, ev);
	if (r) {
		return ev->type;
	}
	else {
		return EVENT_NONE;
	}
}


/////// old code following



/*


/// @brief Array of enqueued events
event_types events_queued[MAX_QUEUED_EVENTS];

/// @brief Write pointer
uint8_t		events_queue_write_i = 0;

/// @brief Read pointer
uint8_t		events_queue_read_i = 0;



// If there is more load (more packets per second etc.) some synchronisation will be needed!

bool events_is_queued(event_types type) {
	for (int i = 0; i < MAX_QUEUED_EVENTS; ++i) {
		if (events_queued[i] == type) return true;
	}
	return false;
}

void events_enqueue(event_types type) {
	if (events_is_queued(type)) return; // don't enqueue twice

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

*/
