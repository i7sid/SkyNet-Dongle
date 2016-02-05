/**
 * @file
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-05
 */

#include "mac.h"
#include "../radio/skynet_radio.h"
#include "../radio/si446x_defs.h"
#include "../radio/si446x_api_lib.h"

extern union si446x_cmd_reply_union Si446xCmd;

uint8_t nb; ///<@brief How many backoff-times have been needed in current try?
uint8_t be; ///<@brief Backoff exponent, how many backoff periods shall be waited

/// @brief  Returns a pseudo random number (strictly) smaller than max.
inline int random(int max) {
    return (rand() % max);
}

bool channel_idle(void) {
    si446x_get_modem_status(0xFF);
    uint8_t rssi = Si446xCmd.GET_MODEM_STATUS.CURR_RSSI;
    uint8_t latched = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;
    DBG("%d, %d\n", rssi, latched);
    return (latched != 1);
}



bool mac_transmit_packet(uint8_t* data, uint16_t length) {
    // if (is_beacon or is_ack) phy_transmit(data, length) // TODO don't wait!
    
    nb = 0;
    be = MAC_CONST_MIN_BE;

    do {
        // initial delay
        int delay = random((1 << be) - 1);  // is: 2^be - 1
        msDelay(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);

        if (channel_idle()) {
        	radio_send_variable_packet(data, length); 		// send to PHY
            return true;
        }
        else {
            nb++;
            be = MIN(be + 1, MAC_CONST_MAX_BE);
        }
    } while (nb <= MAC_CONST_MAX_CSMA_BACKOFFS);
    return false;
}

