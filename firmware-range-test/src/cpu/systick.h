/**
 * @file
 *
 * @date	11.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains the public interface to several delay functionalities.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "../misc/misc.h"


/**
 * @brief	The maximum amount of events that can be enqueued in parallel.
 *
 * This value is used to instantiate the queue, so be aware that a high value
 * may consume unnecessarily much memory.
 */
#define MAX_DELAYED_EVENTS (32)


/**
 * @brief	Handle interrupt from SysTick timer
 */
void SysTick_Handler(void);

/**
 * @brief		Register a function to be executed after a period of time.
 * @param	ms	Milliseconds to wait (passively) before executing f.
 * @param	f	Function that shall be executed.
 */
void register_delayed_event(uint32_t ms, void* f);

/**
 * @brief		Removes a delayed event from the queue to disable it.
 * @param	f	Function pointer to the event that should be removed.
 */
void remove_delayed_event(void* f);

/**
 * @brief		Blocks for a specific amount of time (passively).
 * @param	ms	Milliseconds to block.
 *
 * CPU can sleep in this time or handle incoming interrupts.
 *
 * @warning		Do not use this function in interrupt handlers!
 * 				<b>This will break your code!</b>
 * 				Use \ref register_delayed_event() or \ref events_enqueue() for delaying in ISRs.
 */
void msDelay(uint32_t ms);


/**
 * @brief		Blocks for a specific amount of time (actively).
 * @param	ms	Milliseconds to block.
 *
 * CPU can handle incoming interrupts.
 *
 * @warning		Do not use this function in interrupt handlers!
 * 				This will prevent regular code and possibly
 * 				other interrupt routines from being executed.
 * 				Use \ref register_delayed_event() or \ref events_enqueue() for delaying in ISRs.
 */
void msDelayActive(uint32_t ms);

/**
 * @brief		Blocks for a specific amount of time (actively).
 * @param	us	Microseconds to block.
 *
 * CPU can handle incoming interrupts.
 *
 * @warning		Do not use this function in interrupt handlers!
 * 				This will prevent regular code and possibly
 * 				other interrupt routines from being executed.
 * 				Use \ref register_delayed_event() or \ref events_enqueue() for delaying in ISRs.
 */
void msDelayActiveUs(uint32_t us);

/**
 * @brief	Activates the systick timer to provide cpu functionality for systick.
 */
void enable_systick(void);

/**
 * @brief	Dectivates the systick timer.
 */
void disable_systick(void);

/**
 *  @brief	Callback function for msDelay function.
 *
 *  Not to be called from outside unless you know \b exactly what you're doing
 *  (and if you read this you probably don't).
 */
void msDelayCallback(void);


/**
 * @brief	Returns the current all-time millisecond counter since bootup.
 */
uint32_t skynet_systick_get(void);

#endif /* SYSTICK_H_ */
