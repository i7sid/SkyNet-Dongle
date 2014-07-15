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
 * @brief	Sets SDN high or low to enable/disable complete radio module.
 * @param	on	\b true for powered (SDN=low), \b false for shutdown (SDN=high)
 */
STATIC void radio_set_powered(bool on);

/**
 * @brief			Sends a packet of variable length
 * @param	pkt		byte array to transmit
 * @param	length	number of bytes to send
 */
void send_variable_packet(uint8_t *pkt, uint8_t length);


/**
 * @brief	Processes a received packet.
 */
STATIC void radio_packet_handler(void);

/**
 * @brief	Interrupt handler for interrupts from radio chip
 */
void RADIO_IRQ_HANDLER(void);


#endif /* SKYNET_RADIO_H_ */
