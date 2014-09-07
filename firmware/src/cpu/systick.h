/**
 * @file
 *
 * @date	11.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "../misc/misc.h"

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
 * 				Use \ref register_delayed_event() or \ref events_enqueue() for delaying in ISRs.
 */
void msDelayActive(uint32_t ms);

/**
 * @brief		Blocks for a specific amount of time (actively).
 * @param	ms	Microseconds to block.
 *
 * CPU can handle incoming interrupts.
 *
 * @warning		Do not use this function in interrupt handlers!
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
 */
void msDelayCallback(void);

#endif /* SYSTICK_H_ */
