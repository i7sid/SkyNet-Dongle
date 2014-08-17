/**
 * @file
 *
 * @date	11.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "systick.h"
#include "cpu.h"

#include "stopwatch.h"


#define MAX_DELAYED_EVENTS (32)
volatile bool tick_occured = false;
volatile uint32_t tick_counter = 0;
uint32_t tick_events_t[MAX_DELAYED_EVENTS];
void (*tick_events_f[MAX_DELAYED_EVENTS])(); // array of function pointers


void SysTick_Handler(void) {
	// delayed events
	bool no_delayed = true;
	for (int i = 0; i < MAX_DELAYED_EVENTS; ++i) {
		if (tick_events_t[i] > 0) {
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
	for (int i = 0; i < MAX_DELAYED_EVENTS; ++i) {
		if (tick_events_t[i] == 0) {
			tick_events_t[i] = ms;
			tick_events_f[i] = f;
			enable_systick();
			return;
		}
	}

	DBG("[ERROR] tick_events_delayed OVERFLOW! No free slots!\n");
}

void msDelayCallback() {
	tick_occured = true;
}

void msDelay(uint32_t ms)
{
	//tick_counter = ms;
	tick_occured = false;

	register_delayed_event(ms, msDelayCallback);

	enable_systick();
	while (!tick_occured) {
		cpu_sleep();
	}
}

void msDelayActive(uint32_t ms)
{
	StopWatch_DelayMs(ms);
}

void msDelayActiveUs(uint32_t us)
{
	StopWatch_DelayUs(us);
}

void enable_systick() {
	SystemCoreClockUpdate(); // Generic Initialization
	SysTick_Config(SystemCoreClock/1000); // Enable and setup SysTick Timer at 0.001 HZ
}


