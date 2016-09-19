/**
 * @file
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-05
 */

#if 0


#include "mac.h"
#include "../radio/skynet_radio.h"
#include "../radio/si446x_defs.h"
#include "../radio/si446x_api_lib.h"

extern union si446x_cmd_reply_union Si446xCmd;

uint8_t nb; ///<@brief How many backoff-times have been needed in current try?
uint8_t be; ///<@brief Backoff exponent, how many backoff periods shall be waited
uint8_t expected_ack; ///<@brief seq_no of ACK that is expected
uint8_t received_ack; ///<@brief seq_no of ACK that was received last

static uint8_t seq_no = 0; ///<@brief Current sequence number

/// @brief  Returns a pseudo random number (strictly) smaller than max.
inline int mac_random(int max) {
    return (rand() % max);
}

void mac_init(void) {
	seq_no = (uint8_t)mac_random(0x100);
}
bool channel_idle(void) {
    si446x_get_modem_status(0xFF);
    uint8_t rssi = Si446xCmd.GET_MODEM_STATUS.CURR_RSSI;
    //uint8_t latched = Si446xCmd.GET_MODEM_STATUS.LATCH_RSSI;
    //skynet_cdc_write_debug("%d, %d\n", rssi, latched);
    //return (latched != 1);
    return (rssi < SKYNET_RADIO_RSSI_CCA_THRESHOLD);
}

#define MAC_SEND_TRIES		(3)

bool mac_transmit_packet(mac_frame_data *frame, bool new_seq) {
	uint16_t est_size = mac_frame_data_estimate_size(frame);
	uint8_t buf[est_size];
	if (new_seq) { frame->mhr.seq_no = seq_no++; }
	memset(buf, 0, sizeof(buf));								// TODO remove, DEBUG!
	uint16_t size = mac_frame_data_pack(frame, buf);
	//mac_frame_calc_crc(buf, size);

	/*
	// TODO DEBUG
	for (uint16_t i = 0; i < size; ++i) {
		DBG("%02x ", buf[i]);
	}
	DBG("\n");
	*/

	bool r = false;
	for (uint8_t i = 0; i < MAC_SEND_TRIES; ++i) {
		r |= mac_transmit_data(buf, size);
		msDelayActive(15);
	}
	if (!r) return false;

	// do not wait for ack if not needed
	if (!MHR_FC_GET_ACK_REQUEST(frame->mhr.frame_control)) return true;

	// wait for ack
	expected_ack = frame->mhr.seq_no;

	uint32_t max_wait = MAC_CONST_ACK_WAIT_DURATION / 10;
	for (uint32_t c = 0; c < max_wait; ++c) {
		msDelayActiveUs(10);

		if (expected_ack == received_ack) return true; 	// correct ACK received
	}

	return false; // no ACK received


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
        int delay = mac_random((1 << be) - 1);  // is: 2^be - 1
        //DBG("mac sleep: %d\n", delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        //msDelay(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        msDelayActiveUs(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
        //DBG("mac slept\n");


        if (channel_idle()) {
        	phy_transmit(data, length); 		// send to PHY
        	DBG("%d\n", nb);
            return true;
        }
        else {
            nb++;
            be = MIN(be + 1, MAC_CONST_MAX_BE);
        }
    } while (nb <= MAC_CONST_MAX_CSMA_BACKOFFS);
    return false;
}

bool mac_transmit_ack(uint8_t seq_no) {
	mac_frame_ack ack;
	mac_frame_ack_init(&ack);
	ack.seq_no = seq_no;
	//mac_frame_ack_calc_crc(&ack);

	uint8_t i = 0;
    while (!channel_idle()) {		// TODO korrektes Timing
    	msDelayActiveUs(10);
    	i++;
    	if (i > 100) return false;
    }
	phy_transmit((uint8_t*)&ack, sizeof(mac_frame_ack));
	return true;
}

#endif
