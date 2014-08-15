/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SKYNET_RADIO_H_
#define SKYNET_RADIO_H_

#include "../misc/misc.h"
#include "../spi/spi.h"
#include "../bluetooth/bluetooth.h"

#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "radio.h"
#include "radio_comm.h"
#include "radio_config.h"

extern uint8_t packet_rx_buf[RADIO_MAX_PACKET_LENGTH];

/**
 * @brief	Initialize radio communication chip
 */
void radio_init(void);

/**
 * @brief	Shuts the entire radio chip down.
 */
void radio_shutdown(void);

/**
 * @brief	Wakes the radio chip up to resume normal operation.
 */
void radio_wakeup(void);

/**
 * @brief	Enables the IRQ reception.
 */
void radio_enable_irq(void);

/**
 * @brief	Disables the IRQ reception.
 */
void radio_disable_irq(void);

/**
 * @brief	Sets SDN high or low to enable/disable complete radio module.
 * @param	on	\b true for powered (SDN=low), \b false for shutdown (SDN=high)
 */
void radio_set_powered(bool on);

/**
 * @brief			Sends a packet of variable length
 * @param	pkt		byte array to transmit
 * @param	length	number of bytes to send
 */
void send_variable_packet(uint8_t *pkt, uint8_t length);


/**
 * @brief	Processes a received packet.
 */
void radio_packet_handler(void);

/**
 * @brief	Get status of GPIO0 pin.
 * @return	\b true for \b HIGH; \b false for \b LOW
 */
bool radio_get_gpio0(void);

/**
 * @brief	Configures the RF module for clock measurement at pin GPIO0.
 */
void radio_config_for_clock_measurement(void);

/**
 * @brief	Interrupt handler for interrupts from radio chip
 */
void RADIO_IRQ_HANDLER(void);


/**
 * @brief	Send SET_PROPERTY command to the radio.
 *
 * Properties are read from Si446xCmd.GET_PROPERTY.DATA{0..15}
 *
 * @param GROUP       Property group.
 * @param NUM_PROPS   Number of property to be set.
 * @param START_PROP  Start sub-property address.
 */
void si446x_set_property_lpc(uint8_t GROUP, uint8_t NUM_PROPS, uint8_t START_PROP);

#endif /* SKYNET_RADIO_H_ */
