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
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_R_PORT, LED_R_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_G_PORT, LED_G_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_B_PORT, LED_B_PIN);

	PINSET(LED_R_PORT, LED_R_PIN);
	PINSET(LED_G_PORT, LED_G_PIN);
	PINSET(LED_B_PORT, LED_B_PIN);
}


INLINE void skynet_led_red(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_R_PORT, LED_R_PIN, !on);
}

INLINE void skynet_led_green(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_G_PORT, LED_G_PIN, !on);
}

INLINE void skynet_led_blue(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_B_PORT, LED_B_PIN, !on);
}

STATIC void skynet_led_red_off() {
	skynet_led_red(false);
}

STATIC void skynet_led_green_off() {
	skynet_led_green(false);
}

STATIC void skynet_led_blue_off() {
	skynet_led_blue(false);
}

void skynet_led_blink_red_passive(uint32_t ms) {
	skynet_led_red(true);
	register_delayed_event(ms, skynet_led_red_off);
}

void skynet_led_blink_green_passive(uint32_t ms) {
	skynet_led_green(true);
	register_delayed_event(ms, skynet_led_green_off);
}

void skynet_led_blink_blue_passive(uint32_t ms) {
	skynet_led_blue(true);
	register_delayed_event(ms, skynet_led_blue_off);
}

void skynet_led_blink_red_active(uint32_t ms) {
	skynet_led_red(true);
	msDelayActive(ms);
	skynet_led_red(false);
}

void skynet_led_blink_green_active(uint32_t ms) {
	skynet_led_green(true);
	msDelayActive(ms);
	skynet_led_green(false);
}

void skynet_led_blink_blue_active(uint32_t ms) {
	skynet_led_blue(true);
	msDelayActive(ms);
	skynet_led_blue(false);
}
