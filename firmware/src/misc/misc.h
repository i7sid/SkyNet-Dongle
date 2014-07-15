/**
 * @file
 *
 * @date	26.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef MISC_H_
#define MISC_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "../misc/debug.h"

// general handler for EINT3 sources (EINT3, GPIO, ...)
#define EINT3_IRQ_HANDLER 		EINT3_IRQHandler


// defines for input switch
#define INPUT_SWITCH_HANDLER	EINT1_IRQHandler
#define INPUT_SWITCH_IRQn		EINT1_IRQn
#define INPUT_SWITCH_IRQ		EINT1
#define INPUT_SWITCH_PORT		2
#define INPUT_SWITCH_PIN		11


// defines for radio communication (some for SI lib)
#define SI_LIB_SDN_PORT		2	///< PORT and PIN where SDN from radio chip is connected
#define SI_LIB_SDN_PIN		8	///< @copydoc  SI_LIB_SDN_PORT
#define SI_LIB_nIRQ_PORT	0	///< PORT and PIN where nIRQ from radio chip is connected
#define SI_LIB_nIRQ_PIN		19	///< @copydoc  SI_LIB_nIRQ_PORT
#define SI_LIB_NSEL_PORT	0	///< PORT and PIN where NSEL from radio chip is connected
#define SI_LIB_NSEL_PIN		16	///< @copydoc  SI_LIB_NSEL_PORT
#define SI_LIB_GPIO0_PORT	0	///< PORT and PIN where GPIO0 from radio chip is connected
#define SI_LIB_GPIO0_PIN	20	///< @copydoc  SI_LIB_GPIO0_PORT
//TODO: IRQ handler anpassen (müsste EINT3 oder so sein)
#define RADIO_IRQ_HANDLER	radio_irq_sub_handler
#define RADIO_IRQ_SOURCE	EINT3


// Bluetooth configuration
#define BLUETOOTH_BUFFER_SIZE		256
#define BLUETOOTH_UART 				LPC_UART3
#define BLUETOOTH_UART_IRQn			UART3_IRQn
#define BLUETOOTH_UART_IRQ_HANDLER	UART3_IRQHandler
#define BLUETOOTH_AT_PORT			2
#define BLUETOOTH_AT_PIN			13
#define BLUETOOTH_RESET_PORT		0
#define BLUETOOTH_RESET_PIN			11


// charger configuration
#define CHARGER_STAT1_PORT			0
#define CHARGER_STAT1_PIN			22
#define CHARGER_STAT2_PORT			0
#define CHARGER_STAT2_PIN			21
#define CHARGER_EXTPWR_PORT			2
#define CHARGER_EXTPWR_PIN			12
#define CHARGER_SEL_H_PORT			2
#define CHARGER_SEL_H_PIN			3
#define CHARGER_SEL_L_PORT			2
#define CHARGER_SEL_L_PIN			4


// LED configuration
#define LED_R_PORT					2
#define LED_R_PIN					2
#define LED_G_PORT					2
#define LED_G_PIN					1
#define LED_B_PORT					2
#define LED_B_PIN					0


// ADC configuration
#define ADC_PWR_PORT				1
#define ADC_PWR_PIN					0
#define ADC_IN_PORT					0
#define ADC_IN_PIN					23
#define ADC_CHANNEL					ADC_CH0


// make some tasks more readable in code.
#define PINGET(port, pin)	(Chip_GPIO_ReadPortBit(LPC_GPIO, port, pin))
#define PINSET(port, pin)	(Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, true))
#define PINCLR(port, pin)	(Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, false))



// additional definitions for more comfortable work
#define EINT0	0x1	///< @brief for use with EXTINT, EXTMODE, EXTPOLAR
#define EINT1	0x2	///< @copydoc EINT0
#define EINT2	0x4	///< @copydoc EINT0
#define EINT3	0x8	///< @copydoc EINT0

#define _delay_ms msDelay


#endif /* MISC_H_ */
