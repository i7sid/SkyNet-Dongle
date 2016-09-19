/**
 * @file
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-08-31
 */

#ifndef RF_PACKET_HANDLER_H
#define RF_PACKET_HANDLER_H

#include "skymac.h"
#include "usb_message.h"
#include "config.h"
#include "rf_packet_handler.h"


void rf_handler(uint8_t* data, uint8_t length);


#endif /* !RF_PACKET_HANDLER_H */
