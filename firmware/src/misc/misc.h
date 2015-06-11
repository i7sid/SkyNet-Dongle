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
#define GPIO_IRQ_HANDLER		EINT3_IRQ_HANDLER



// defines for input switch
#define INPUT_SWITCH_HANDLER	EINT1_IRQHandler
#define INPUT_SWITCH_IRQn		EINT1_IRQn
#define INPUT_SWITCH_IRQ		EINT1
#define INPUT_SWITCH_PORT		2	///< PORT and PIN where input switch is connected
#define INPUT_SWITCH_PIN		11	///< @copydoc  INPUT_SWITCH_PORT


// defines for radio communication (some for SI lib)
#define SI_LIB_SDN_PORT		2	///< PORT and PIN where SDN from radio chip is connected
#define SI_LIB_SDN_PIN		8	///< @copydoc  SI_LIB_SDN_PORT
#define SI_LIB_nIRQ_PORT	0	///< PORT and PIN where nIRQ from radio chip is connected
#define SI_LIB_nIRQ_PIN		19	///< @copydoc  SI_LIB_nIRQ_PORT
#define SI_LIB_NSEL_PORT	0	///< PORT and PIN where NSEL from radio chip is connected
#define SI_LIB_NSEL_PIN		16	///< @copydoc  SI_LIB_NSEL_PORT
#define SI_LIB_GPIO0_PORT	0	///< PORT and PIN where GPIO0 from radio chip is connected
#define SI_LIB_GPIO0_PIN	20	///< @copydoc  SI_LIB_GPIO0_PORT
//#define RADIO_IRQ_HANDLER	EINT3_IRQ_HANDLER //irq now handled in gpio_irq
#define RADIO_IRQ_SOURCE	EINT3
#define RADIO_ON_PORT		0
#define RADIO_ON_PIN		9


// Bluetooth configuration
#define BLUETOOTH_DEVICE_NAME		"SKYNET3v1a"			// alphanumeric (max. 31 characters)
#define BLUETOOTH_DEVICE_PIN		"2014"    			// alphanumeric (max. 16 characters)
#define BLUETOOTH_BUFFER_SIZE		512
#define BLUETOOTH_UART 				LPC_UART3
#define BLUETOOTH_UART_IRQn			UART3_IRQn
#define BLUETOOTH_UART_IRQ_HANDLER	UART3_IRQHandler
#define BLUETOOTH_AT_PORT			0
#define BLUETOOTH_AT_PIN			25
#define BLUETOOTH_RESET_PORT		3
#define BLUETOOTH_RESET_PIN			25
#define BLUETOOTH_CONNECTED_PORT	0
#define BLUETOOTH_CONNECTED_PIN		24
#define BLUETOOTH_ON_PORT			3
#define BLUETOOTH_ON_PIN			26

//GPS configuration
#define GPS_UART LPC_UART0
#define GPS_IRQn UART0_IRQn
#define GPS_IRQHandler UART0_IRQHandler

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
#define CHARGER_EXTPWR_HANDLER		EINT2_IRQHandler
#define CHARGER_EXTPWR_IRQn			EINT2_IRQn
#define CHARGER_EXTPWR_IRQ			EINT2
#define CHARGER_USBDP_PORT			0
#define CHARGER_USBDP_PIN			29
#define CHARGER_USBDM_PORT			0
#define CHARGER_USBDM_PIN			30
#define CHARGER_USBCON_PORT			2
#define CHARGER_USBCON_PIN			9


// DCDC configuration
#define DCDC_PS_PORT				0
#define DCDC_PS_PIN					8


// LED configuration
#define LED_R_PORT					1
#define LED_R_PIN					21
#define LED_G_PORT					1
#define LED_G_PIN					20
#define LED_B_PORT					1
#define LED_B_PIN					18


// ADC configuration
#define ADC_PWR_PORT				1
#define ADC_PWR_PIN					17
#define ADC_IN_PORT					0
#define ADC_IN_PIN					23
#define ADC_EXT_IN_PORT				0
#define ADC_EXT_IN_PIN				26
#define ADC_CHANNEL					ADC_CH0
#define ADC_CHANNEL_EXT				ADC_CH3 	// AD0[3]


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
