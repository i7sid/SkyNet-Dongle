/**@file
 * gpio_irq.c
 *
 *  Created on: 27.04.2015
 *      Author: christoph
 *
 *      @brief Functionality for GPIO Interrupts
 */

#include "gpio_irq.h"
#include "../radio/skynet_radio.h"
#include "../basestation/windspeed/windspeed.h"

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

void enable_gpio_irq(int channel){
	NVIC_DisableIRQ(EINT3_IRQn);
	uint32_t irqmaskset = Chip_GPIOINT_GetIntFalling(LPC_GPIOINT,GPIOINT_PORT0); // get gpio irq mask
	irqmaskset |= (1 << channel); //set bit channel
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, irqmaskset); //enable irq
	NVIC_EnableIRQ(EINT3_IRQn);

}

void disable_gpio_irq(int channel){
	NVIC_DisableIRQ(EINT3_IRQn);
	uint32_t irqmaskclear = Chip_GPIOINT_GetIntFalling(LPC_GPIOINT,GPIOINT_PORT0); // get gpio irq mask
	irqmaskclear &= ~(1 << channel); // clear bit channel
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, irqmaskclear); //disable irq
	NVIC_EnableIRQ(EINT3_IRQn);

}

