/**@file
 * gpio_irq.h
 *
 *  Created on: 23.04.2015
 *      Author: christoph
 *
 *      @brief Interface for GPIO Interrupts
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

void enable_gpio_irq(int channel);

void disable_gpio_irq(int channel);

#endif /* GPIO_IRQ_H_ */
