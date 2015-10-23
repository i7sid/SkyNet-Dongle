/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality to control the on-board LED.
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "led.h"
#include "../cpu/systick.h"

void skynet_led_init() {
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_PORT, LED_PIN);
	skynet_led(false);
}


INLINE void skynet_led(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_PORT, LED_PIN, !on);
}

STATIC void skynet_led_off() {
	skynet_led(false);
}

void skynet_led_blink_passive(uint32_t ms) {
	skynet_led(true);
	register_delayed_event(ms, skynet_led_off);
}

void skynet_led_blink_active(uint32_t ms) {
	skynet_led(true);
	msDelayActive(ms);
	skynet_led(false);
}
