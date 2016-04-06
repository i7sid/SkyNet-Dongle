/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains public interface to RF module.
 */

#ifndef SKYNET_RADIO_H_
#define SKYNET_RADIO_H_

#include "../misc/misc.h"
#include "../spi/spi.h"

#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "radio.h"
#include "radio_comm.h"
#include "radio_config.h"

#define SKYNET_RADIO_MAX_SIZE (1024)

#define SKYNET_RADIO_TESTPATTERN 	"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
#define SKYNET_RADIO_TESTLENGTH 		80


extern char rf_packet_rx_buf[SKYNET_RADIO_MAX_SIZE+1];
extern volatile bool radio_initialized;


// debug
typedef struct skynet_packet {
	char*	data;
	int		length;
} skynet_packet;


/**
 * @brief	Initiliaze peripheral pin(s).
 */
void radio_pin_init(void);

/**
 * @brief	Initialize radio communication chip and wakes it up.
 */
void radio_init(void);

/**
 * @brief	Shuts the entire radio chip down.
 */
void radio_shutdown(void);

/**
 * @brief	Enables the IRQ reception.
 */
void radio_enable_irq(void);

/**
 * @brief	Disables the IRQ reception.
 */
void radio_set_irq(void);

void radio_unset_irq(void);


/**
 * @brief	Resets the length of field 2 to standard.
 */
void radio_reset_packet_size(void);

/**
 * @brief			Sends an packet of variable length via RF.
 * @param	packet	Bytes to send
 * @param	length	Number of bytes to send
 */
void radio_send_variable_packet(uint8_t *packet, uint16_t length);



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
