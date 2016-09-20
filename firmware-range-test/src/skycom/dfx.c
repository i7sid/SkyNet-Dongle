/**
 *  @file
 *
 *  @date 	29.07.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <string.h>
#include <../misc/misc.h>

#include "dfx.h"
#include "LPC_SWU.h"
#include "../cpu/systick.h"

void dfx_init(void) {
	// init pins
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, DFX_TX_PORT, DFX_TX_PIN, IOCON_FUNC3);

	// debug
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	Chip_TIMER_Init(LPC_TIMER2);

	swu_init(LPC_TIMER2);
}
