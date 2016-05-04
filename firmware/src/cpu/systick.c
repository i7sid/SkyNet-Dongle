/**
 * @file
 *
 * @date	11.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains several delay-methods.
 */

#include "systick.h"
#include "cpu.h"

#include "stopwatch.h"

/// @brief Temporary variable to determine if really a tick occurred while waiting passively.
volatile bool tick_occured = false;

/// @brief Array holding remaining times for delayed events.
uint32_t tick_events_t[MAX_DELAYED_EVENTS];

/// @brief Array holding function pointers for delayed events.
void (*tick_events_f[MAX_DELAYED_EVENTS])();


void SysTick_Handler(void) {
	// delayed events
	bool no_delayed = true;
	for (int i = 0; i < MAX_DELAYED_EVENTS; ++i) {
		if (tick_events_t[i] > 0 && tick_events_f[i] > 0x0) {
			no_delayed = false;
			tick_events_t[i] -= 1;
			if (tick_events_t[i] == 0) {
				tick_events_f[i]();
			}
		}
	}

	if (no_delayed) {
		SysTick->CTRL  = 0; // disable Systick if not needed any more
	}
}



void register_delayed_event(uint32_t ms, void* f) {
	__disable_irq();
	for (int i = 0; i < MAX_DELAYED_EVENTS; ++i) {
		if (tick_events_t[i] == 0) {
			tick_events_t[i] = ms;
			tick_events_f[i] = f;
			enable_systick();
			__enable_irq();
			return;
		}
	}
	__enable_irq();


	DBG("[ERROR] tick_events_delayed OVERFLOW! No free slots!\n");
}

void remove_delayed_event(void* f) {
	__disable_irq();
	bool stop_systick = true;
	for (int i = 0; i < MAX_DELAYED_EVENTS; ++i) {
		if (tick_events_f[i] == f) {
			tick_events_t[i] = 0;
			tick_events_f[i] = 0x0;
		}
		else if (tick_events_f[i] != 0x0) {
			stop_systick = false;
		}
	}

	if (stop_systick) {
		disable_systick();
	}
	__enable_irq();

	//DBG("[ERROR] tick_events_delayed OVERFLOW! No free slots!\n");
}

void msDelayCallback(void) {
	tick_occured = true;
}


void msDelay(uint32_t ms)
{
	msDelayActive(ms);
	/*
	tick_occured = false;

	register_delayed_event(ms, msDelayCallback);

	enable_systick();
	while (!tick_occured) {
		cpu_sleep();
	}
	*/
}


void msDelayActive(uint32_t ms)
{
	StopWatch_DelayMs(ms);
}

void msDelayActiveUs(uint32_t us)
{
	StopWatch_DelayUs(us);
}

void enable_systick(void) {
	SystemCoreClockUpdate(); // Generic Initialization
	SysTick_Config(SystemCoreClock/1000); // Enable and setup SysTick Timer at 0.001 HZ
}

INLINE void disable_systick(void) {
	SysTick->CTRL  = 0;
}


