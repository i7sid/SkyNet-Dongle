/**
 * @file
 *
 * @date	29.06.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for charger configuration.
 */

#include "charger.h"


void charger_init() {
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_STAT1_PORT, CHARGER_STAT1_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_STAT2_PORT, CHARGER_STAT2_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_EXTPWR_PORT, CHARGER_EXTPWR_PIN);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN);

	// set port pins for USB port (only used for charging!)
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_USBDP_PORT, CHARGER_USBDP_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, CHARGER_USBDM_PORT, CHARGER_USBDM_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, CHARGER_USBCON_PORT, CHARGER_USBCON_PIN);

	LPC_SYSCTL->EXTMODE |= 0x4;		// edge sensitive
	LPC_SYSCTL->EXTPOLAR |= 0x4;	// rising edge
	LPC_SYSCTL->EXTINT |= 0x4;		// reset INT

	NVIC_SetPriority(CHARGER_EXTPWR_IRQn, 4);
	NVIC_EnableIRQ(CHARGER_EXTPWR_IRQn);

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
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN, false);
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN, false);
		break;
	case USB_HIGH:
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN, false);
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN, true);
		break;
	case USB_CHARGE:
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_H_PORT, CHARGER_SEL_H_PIN, true);
		Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_SEL_L_PORT, CHARGER_SEL_L_PIN, true); // not important
		break;
	}
}

void charger_check_port(void) {
	Chip_GPIO_SetPinState(LPC_GPIO, CHARGER_USBDP_PORT, CHARGER_USBDP_PIN, true);
	msDelayActive(500); // not sure if data lines are connected immeadiately after power line
	if (Chip_GPIO_GetPinState(LPC_GPIO, CHARGER_USBDM_PORT, CHARGER_USBDM_PIN)) {
		DBG("D- is HIGH, DCP deteced, go to full charge mode.\n");
		charger_set_mode(USB_CHARGE);
	}
	else {
		DBG("TODO: Here the USB enumeration should be done, so that 500mA can be drawn.\n");
		charger_set_mode(USB_LOW);
	}

}

void CHARGER_EXTPWR_HANDLER(void) {
	DBG("External power supplied!\n");

	// first set to 100mA mode to be sure not to draw too much current in first ms
	charger_set_mode(USB_LOW);

	// no check if Dedicated Charging Port or Data Port
	charger_check_port();


	// reset EINT flag
	LPC_SYSCTL->EXTINT |= 0x4;
}

