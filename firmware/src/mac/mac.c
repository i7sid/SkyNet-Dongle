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

uint8_t seq_no = 0; ///<@brief Current sequence number

/// @brief  Returns a pseudo random number (strictly) smaller than max.
static inline int random(int max) {
    return (rand() % max);
}

uint64_t mac_mhr_get_dest_addr(uint8_t *pkt) {
	// TODO
}

bool channel_idle(void) {
    si446x_get_modem_status(0xFF);
    //uint8_t rssi = Si446xCmd.GET_MODEM_STATUS.CURR_RSSI;
    uint8_t latched = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;
    //DBG("%d, %d\n", rssi, latched);
    return (latched != 1);
}

bool mac_transmit_packet(mac_frame_data *frame) {
	uint16_t est_size = mac_frame_data_estimate_size(frame);
	uint8_t buf[est_size];
	frame->mhr.seq_no = seq_no++;
	memset(buf, 0, sizeof(buf));								// TODO remove, DEBUG!
	uint16_t size = mac_frame_data_pack(frame, buf);
	mac_frame_calc_crc(buf, size);
	return mac_transmit_data(buf, size);
	/*
	uint16_t size = mac_frame_data_get_size(frame);
	uint8_t buf[size];
	mac_frame_data_pack(frame, buf);
	mac_frame_calc_crc(buf, sizeof(buf));
	return mac_transmit_data(buf, size);
	*/
}

bool mac_transmit_data(uint8_t* data, uint16_t length) {
    // if (is_beacon or is_ack) phy_transmit(data, length) // TODO don't wait!
    
    nb = 0;
    be = MAC_CONST_MIN_BE;

    do {
        // initial delay
        int delay = random((1 << be) - 1);  // is: 2^be - 1
        //DBG("mac sleep: %d\n", delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        //msDelay(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        msDelayActiveUs(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        //DBG("mac slept\n");


        if (channel_idle()) {
        	phy_transmit(data, length); 		// send to PHY
            return true;
        }
        else {
            nb++;
            be = MIN(be + 1, MAC_CONST_MAX_BE);
        }
    } while (nb <= MAC_CONST_MAX_CSMA_BACKOFFS);
    return false;
}

