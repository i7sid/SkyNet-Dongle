/*
 * gpio_irq.h
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 */

#ifndef GPIO_IRQ_H_
#define GPIO_IRQ_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

void GPIO_IRQ_HANDLER(void);

#endif /* GPIO_IRQ_H_ */
