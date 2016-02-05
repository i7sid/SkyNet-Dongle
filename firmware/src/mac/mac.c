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

void phy_transmit(uint8_t* data, uint16_t length);
void random(int max);
bool channel_idle(void);

uint8_t nb; ///<@brief How many backoff-times have been needed in current try?
uint8_t be; ///<@brief Backoff exponent, how many backoff periods shall be waited

bool mac_transmit_packet(uint8_t* data, uint16_t length) {
    // if (is_beacon or is_ack) phy_transmit(data, length) // don't wait!
    
    nb = 0;
    be = MAC_CONST_MIN_BE;

    do {
        // initial delay
        int delay = random((1 << be) - 1);
        msDelay(delay * MAC_CONST_A_UNIT_BACKOFF_PERIOD);

        if (channel_idle()) {
            phy_transmit(data, length);
            return true;
        }
        else {
            nb++;
            be = min(be + 1, MAC_CONST_MAX_BE);
        }

        else {
            phy_transmit(data, length);
            return true;
        }
    } while (nb <= MAC_CONST_MAX_CSMA_BACKOFFS);
    return false;
}

