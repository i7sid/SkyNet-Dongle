/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for charger configuration.
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "charger.h"

void charger_init() {
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_STAT1_PORT, CHARGER_STAT1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_STAT2_PORT, CHARGER_STAT2_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_EXTPWR_PORT, CHARGER_EXTPWR_PIN);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN);

	//TODO: register interrupt for external power supplied?

	charger_set_mode(USB_LOW);
}

bool charger_status1() {
	return Chip_GPIO_GetPinState(LPC_GPIO, CHARGER_STAT1_PORT, CHARGER_STAT1_PIN);
}

bool charger_status2() {
	return Chip_GPIO_GetPinState(LPC_GPIO, CHARGER_STAT2_PORT, CHARGER_STAT2_PIN);
}

bool charger_external_power() {
	return Chip_GPIO_GetPinState(LPC_GPIO, CHARGER_EXTPWR_PORT, CHARGER_EXTPWR_PIN);
}

void charger_set_mode(charge_mode mode) {
	switch(mode) {
	case USB_LOW:
		PINCLR(CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN);
		PINCLR(CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN);
		break;
	case USB_HIGH:
		PINCLR(CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN);
		PINSET(CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN);
		break;
	case USB_CHARGE:
		PINSET(CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN);
		PINSET(CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN); // not important
		break;
	}
}
