/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality of input switch.
 */


#include "input.h"
#include "../cpu/cpu.h"
#include "../cpu/systick.h"
#include "../misc/event_queue.h"

/// @brief Amount of times switch has been checked in current action
volatile int switch_checked_times = 0;

/// @brief Current state of finite state machine
switch_check_state switch_state = SWITCH_FSM_NOT_PRESSED;


void input_init() {
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, INPUT_SWITCH_PORT, INPUT_SWITCH_PIN);

	LPC_SYSCTL->EXTMODE |= 0x2;	// edge sensitive
	LPC_SYSCTL->EXTINT |= 0x2;

	NVIC_SetPriority(INPUT_SWITCH_IRQn, 4);
	NVIC_EnableIRQ(INPUT_SWITCH_IRQn);

}

bool input_state() {
	return !Chip_GPIO_GetPinState(LPC_GPIO, INPUT_SWITCH_PORT, INPUT_SWITCH_PIN);
}

void input_pressed(switch_pressed_type type) {
	switch (type) {
		case SWITCH_SHORT:
			events_enqueue(EVENT_SWITCH_SINGLE);
			break;
		case SWITCH_POWER:
			events_enqueue(EVENT_SWITCH_POWER);
			break;
		case SWITCH_DOUBLE:
			events_enqueue(EVENT_SWITCH_DOUBLE);
			break;
		default:
			break;
	}
}

void recheck_switch_state() {
	switch (switch_state) {
		case SWITCH_FSM_NOT_PRESSED:
			// should not happen, something's wrong
			DBG("[ASSERT] Switch state was SWITCH_FSM_NOT_PRESSED in recheck_switch_state.\n");
			break;
		case SWITCH_FSM_FIRST:
			if (input_state() && switch_checked_times >= SWITCH_PRESS_POWER_TIMES) {
				// long enough pressed for POWER event
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
				input_pressed(SWITCH_POWER);
			}
			else if (input_state()) {
				//DBG("SWITCH_FSM_FIRST + TRUE, waiting...\n");
				// still pressing, but not long enough for POWER event - wait!
				switch_checked_times++;
				register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
			}
			else if (!input_state() && switch_checked_times < SWITCH_PRESS_SINGLE_TIMES) {
				//DBG("SWITCH_FSM_FIRST + FALSE, aborting...\n");
				// not long enough pressed for anything - bumping?
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
			}
			else if (!input_state() && switch_checked_times >= SWITCH_PRESS_SINGLE_TIMES) {
				//DBG("SWITCH_FSM_FIRST + FALSE, long enough...\n");
				// long enough pressed for single click - does now a second follow?
				switch_checked_times = 0;
				switch_state = SWITCH_FSM_SECOND_WAITING;
				register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
			}
			break;

		case SWITCH_FSM_SECOND_WAITING:
			if (input_state()) {
				// pressed - now check if second is long enough...
				switch_state = SWITCH_FSM_SECOND;
				switch_checked_times = 0;
				register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
			}
			else if (!input_state() && switch_checked_times < SWITCH_PRESS_DOUBLE_TIMES) {
				// still not pressed, not waited long enough - waiting
				switch_checked_times++;
				register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
			}
			else if (!input_state() && switch_checked_times >= SWITCH_PRESS_DOUBLE_TIMES) {
				// still not pressed, waited long enough - trigger SINGLE press event
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
				input_pressed(SWITCH_SHORT);
			}
			break;

		case SWITCH_FSM_SECOND:
			 if (input_state() && switch_checked_times < SWITCH_PRESS_POWER_TIMES) {
				 // pressing for the second time,
				 // but not long enough for POWER event - wait
				 switch_checked_times++;
				 register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
			 }
			 else if (input_state() && switch_checked_times >= SWITCH_PRESS_POWER_TIMES) {
				 // pressing for the second time, long enough for POWER event - POWER
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
				input_pressed(SWITCH_POWER);
			 }
			 else if (!input_state() && switch_checked_times < SWITCH_PRESS_SINGLE_TIMES) {
				 // not pressed long enough - trigger SINGLE press event
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
				input_pressed(SWITCH_SHORT);
			 }
			 else if (!input_state() && switch_checked_times >= SWITCH_PRESS_SINGLE_TIMES) {
				 // pressed long enough - trigger DOUBLE press event
				switch_state = SWITCH_FSM_NOT_PRESSED;
				NVIC_EnableIRQ(INPUT_SWITCH_IRQn);
				switch_checked_times = 0;
				input_pressed(SWITCH_DOUBLE);
			 }
			 break;

		default:
			break;
	}
}

void INPUT_SWITCH_HANDLER(void) {
	DBG("INPUT pressed\n");

	// if we are in sleep mode, just do nothing
	// otherwise, handle event
	if (!cpu_powered_down) {
		// button pressed in normal operation mode
		switch_state = SWITCH_FSM_FIRST;

		// disable interrupt
		NVIC_DisableIRQ(INPUT_SWITCH_IRQn);

		// enqueue next check if button is still pressed
		register_delayed_event(SWITCH_CHECK_WAIT_MS, recheck_switch_state);
	}
	// reset EINT flag
	LPC_SYSCTL->EXTINT |= 0x2;
}
