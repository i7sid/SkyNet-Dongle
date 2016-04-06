/**
 * @file
 *
 * @date	26.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef MISC_H_
#define MISC_H_

#ifdef CORE_M3
#include <chip.h>
#else
#include "linux_incs.h"
#endif

#include "../misc/debug.h"

#define phy_transmit(x,y) 	radio_send_variable_packet(x,y)

// general handler for EINT3 sources (EINT3, GPIO, ...)
#define GPIO_IRQ_HANDLER 		EINT3_IRQHandler


// defines for radio communication (some for SI lib)
#define SI_LIB_SDN_PORT		2	///< PORT and PIN where SDN from radio chip is connected
#define SI_LIB_SDN_PIN		8	///< @copydoc  SI_LIB_SDN_PORT
#define SI_LIB_nIRQ_PORT	0	///< PORT and PIN where nIRQ from radio chip is connected
#define SI_LIB_nIRQ_PIN		19	///< @copydoc  SI_LIB_nIRQ_PORT
#define SI_LIB_NSEL_PORT	0	///< PORT and PIN where NSEL from radio chip is connected
#define SI_LIB_NSEL_PIN		16	///< @copydoc  SI_LIB_NSEL_PORT
#define SI_LIB_GPIO0_PORT	0	///< PORT and PIN where GPIO0 from radio chip is connected
#define SI_LIB_GPIO0_PIN	20	///< @copydoc  SI_LIB_GPIO0_PORT
#define RADIO_IRQ_HANDLER	EINT3_IRQ_HANDLER
#define RADIO_IRQ_SOURCE	EINT3
#define RADIO_ON_PORT		0
#define RADIO_ON_PIN		9


// LED configuration
#define LED_PORT					1
#define LED_PIN						21


// USB configuration
#define USB_CONNECT_PORT			2
#define USB_CONNECT_PIN				9
#define USB_D_PLUS_PORT				0
#define USB_D_PLUS_PIN				29
#define USB_D_MINUS_PORT			0
#define USB_D_MINUS_PIN				30



// ADC configuration
#define ADC_PWR_PORT				1
#define ADC_PWR_PIN					17
#define ADC_IN_PORT					0
#define ADC_IN_PIN					23
#define ADC_EXT_IN_PORT				0
#define ADC_EXT_IN_PIN				26
#define ADC_CHANNEL					ADC_CH0
#define ADC_CHANNEL_EXT				ADC_CH0 	// AD0[3]


// basestation
#define WINCUPS_PORT				2
#define WINCUPS_PIN					13
#define WINVANE_PORT				0
#define WINVANE_PIN					3
#define COMPASS_I2C_SDA_PORT		0
#define COMPASS_I2C_SDA_PIN			10
#define COMPASS_I2C_SCL_PORT		0
#define COMPASS_I2C_SCL_PIN			11
#define COMPASS_I2C					I2C2
#define COMPASS_I2C_IRQn			I2C2_IRQn
#define GPS_UART_TX_PORT			2
#define GPS_UART_TX_PIN				0
#define GPS_UART_RX_PORT			2
#define GPS_UART_RX_PIN				1



// make some tasks more readable in code.
#define PINGET(port, pin)	(Chip_GPIO_ReadPortBit(LPC_GPIO, port, pin))
#define PINSET(port, pin)	(Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, true))
#define PINCLR(port, pin)	(Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, false))



// additional definitions for more comfortable work
#define EINT0	0x1	///< @brief for use with EXTINT, EXTMODE, EXTPOLAR
#define EINT1	0x2	///< @copydoc EINT0
#define EINT2	0x4	///< @copydoc EINT0
#define EINT3	0x8	///< @copydoc EINT0


/// @brief  Let _delay_ms point to an active waiting routine as expected.
#define _delay_ms 	msDelayActive

#define SI446X_PATCH_CMDS

#endif // MISC_H_
