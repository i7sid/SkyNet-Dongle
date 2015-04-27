/*
 * gpio_irq.c
 *
 *  Created on: 27.04.2015
 *      Author: christoph
 */

#include "gpio_irq.h"
#include "radio/skynet_radio.h"
#include "windspeed.h"

void GPIO_IRQ_HANDLER(void) {

	// WARNING: DO NOT MAKE ANY DEBUG OUTPUTS IN HERE!
	// The timing receiving long packets is very fragile!
	// Every DBG takes too much time and leads to broken packet reception!

	if (Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0) & (1 << 19)) {
		radio_packet_handler();
		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	}
	if (Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0) & (1 << 3)) {
		//handels the interrups from the windcups of the basestation
		tickhandler();
		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 3));
	}
	//LPC_SYSCTL->EXTINT |= (1<<EINT3);	// reset IRQ state
	NVIC_ClearPendingIRQ(EINT3_IRQn);

}

