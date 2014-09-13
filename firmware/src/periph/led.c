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

void skynet_led_init() {
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_R_PORT, LED_R_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_G_PORT, LED_G_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LED_B_PORT, LED_B_PIN);

	PINSET(LED_R_PORT, LED_R_PIN);
	PINSET(LED_G_PORT, LED_G_PIN);
	PINSET(LED_B_PORT, LED_B_PIN);
}


void skynet_led_red(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_R_PORT, LED_R_PIN, !on);
}

void skynet_led_green(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_G_PORT, LED_G_PIN, !on);
}

void skynet_led_blue(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, LED_B_PORT, LED_B_PIN, !on);
}
