/**
 *  @file
 *
 *  @date 	29.07.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "dfx.h"
#include "LPC_SWU.h"
#include "../cpu/systick.h"

void dfx_init(void) {
	// init pins
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	// debug
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	/*
	// test which pin is which
	Chip_GPIO_SetPinOutHigh(LPC_GPIO, DFX_RX_PORT, DFX_RX_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
	*/


	Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
	Chip_IOCON_PinMuxSet(LPC_IOCON, DFX_TX_PORT, DFX_TX_PIN, IOCON_FUNC3);
	//Chip_GPIO_SetPinDIROutput(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_TIMER2);
	//Chip_TIMER_Init(LPC_TIMER2);


	swu_init(LPC_TIMER2);

	/*
	while(1) {
		Chip_GPIO_SetPinOutLow(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
		msDelayActive(50);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO, DFX_TX_PORT, DFX_TX_PIN);
		msDelayActive(50);
	}
	*/

	unsigned char i = 0;
	while(1) {
		swu_tx_chr(i);
		i++;
		if (i > 127) i = 0;
		msDelayActive(1000);
	}


}
